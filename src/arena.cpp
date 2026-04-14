#include "arena.hpp"

#include "windows.hpp"
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