#include "arena.hpp"

#include "windows.hpp"
#include "stb_sprintf.h"
#include <string.h>

struct ArenaFrameRestoreData {
	u64 restore_point;
	b8  had_error;
};

// TODO(hhammon) This isn't really safe.
#define SYS_PAGE_SIZE 4096

#define MIN_COMMIT_SIZE (1 * MEGABYTE)

api_method void scratch_init(u64 reserve) {
	scratch_arena = arena_create(reserve);
}

api_method void scratch_destroy() {
	arena_destroy(&scratch_arena);
}

api_method Arena arena_create(u64 reserve) {
	reserve = align_up(reserve, SYS_PAGE_SIZE);
	Arena arena = { };

	arena.base = (u8*)VirtualAlloc(NULL, reserve, MEM_RESERVE, PAGE_READWRITE);
	if (!arena.base) return arena;

	arena.reserved = reserve;
	return arena;
}

api_method void arena_frame_begin(Arena* arena) {
	arena->frame_depth++;

	if (!arena->frame_error_depth) {
		ArenaFrameRestoreData restore = {
			.restore_point = arena->frame_restore_point,
			.had_error     = arena->frame_had_error,
		};
		u64 used = arena->used;
		ArenaFrameRestoreData* restore_push = alloc_item(arena, ArenaFrameRestoreData);
		if (restore_push) {
			*restore_push              = restore;
			arena->frame_restore_point = used;
			arena->frame_had_error     = false;
		} else {
			arena->frame_error_depth = 1;
		}
	} else {
		arena->frame_error_depth++;
	}
}

api_method void arena_frame_end(Arena* arena) {
	assert(arena->frame_depth);

	arena->frame_depth--;
	if (!arena->frame_error_depth) {
		u64 restore_offset = align_up(arena->frame_restore_point, alignof(ArenaFrameRestoreData));
		ArenaFrameRestoreData* restore = (ArenaFrameRestoreData*)(arena->base + restore_offset);
		arena->used                    = arena->frame_restore_point;
		arena->frame_restore_point     = restore->restore_point;
		arena->frame_had_error         = restore->had_error;
	} else {
		arena->frame_error_depth--;
	}
}

api_method void* arena_alloc_item(Arena* arena, u32 align, u64 size) {
	assert(is_power_of_two(align) && align <= SYS_PAGE_SIZE);

	u64 addr_offset = align_up(arena->used, align);
	u64 new_used    = addr_offset + size;

	if (new_used > arena->reserved || arena->frame_error_depth) {
		arena->arena_had_error = true;
		arena->frame_had_error = true;
		return NULL;
	}

	if (new_used > arena->committed) {
		u64 commit_needed = align_up(new_used - arena->committed, SYS_PAGE_SIZE);
		u64 to_commit     = max(commit_needed, MIN_COMMIT_SIZE);

		if (!VirtualAlloc(arena->base + arena->committed, to_commit, MEM_COMMIT, PAGE_READWRITE)) {
			arena->arena_had_error = true;
			arena->frame_had_error = true;
			return NULL;
		}

		arena->committed += to_commit;
	}

	void* ptr = arena->base + addr_offset;

	debug_only {
		memset(ptr, 0xcd, size);
	}

	arena->used = new_used;
	return ptr;
}

api_method void arena_alloc_array(Arena* arena, u32 align, u64 item_size, void** base, u64* len) {
	assert(is_power_of_two(align) && align <= SYS_PAGE_SIZE);

	*base = arena_alloc_item(arena, align, item_size * *len);

	if (*base == NULL) {
		*len = 0;
	}
}

api_method void arena_destroy(Arena* arena) {
	VirtualFree(arena->base, arena->reserved, MEM_RELEASE);
	*arena = {};
}

internal u8* arena_sprintf_cb(const u8* buf, void* user, s32 len) {
	(void)buf;

	Arena* arena = (Arena*)user;

	if (len == STB_SPRINTF_MIN) {
		return (u8*)arena_alloc_item(arena, 1, STB_SPRINTF_MIN);
	} else {
		arena->used -= STB_SPRINTF_MIN - len;
		return NULL;
	}
}

api_method StringZ arena_sprintf_va(Arena* arena, CString fmt, __builtin_va_list args) {
	u64 used_before = arena->used;

	u8* buf = (u8*)arena_alloc_item(arena, 1, STB_SPRINTF_MIN);

	if (buf) {
		stbsp_vsprintfcb((STBSP_SPRINTFCB*)arena_sprintf_cb, arena, (char*)buf, (char*)fmt, args);
	}

	u8* null_terminator = alloc_item(arena, u8);
	if (null_terminator) {
		*null_terminator = 0;
	}

	if (arena->frame_had_error) {
		arena->used = used_before;
		return (StringZ){ };
	} else {
		return (StringZ) {
			.ptr = (char*)arena->base + used_before,
			.len =        arena->used - used_before - 1,
		};
	}
}

api_method StringZ arena_sprintf(Arena* arena, CString fmt, ...) {
	__builtin_va_list args;
	__builtin_va_start(args, fmt);

	StringZ result = arena_sprintf_va(arena, fmt, args);

	__builtin_va_end(args);

	return result;
}