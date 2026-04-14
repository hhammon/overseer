#ifndef __ARENA_HPP__
#define __ARENA_HPP__

#include "core.hpp"

struct Arena {
	u8* base;
	u64 reserved;
	u64 committed;
	u64 used;
	u64 frame_restore_point;
	u32 frame_depth;
	u32 frame_error_depth;
	b8  frame_had_error;
	b8  arena_had_error;
};

#define SCRATCH_RESERVE_SIZE (1 * GIGABYTE)
thread_local Arena scratch_arena;

api_method void scratch_init(u64 reserve = SCRATCH_RESERVE_SIZE);

api_method Arena arena_create(u64 reserve);
api_method void  arena_frame_begin(Arena* arena);
api_method void  arena_frame_end  (Arena* arena);
api_method void* arena_alloc_item (Arena* arena, u32 align, u64 size);
api_method void  arena_alloc_array(Arena* arena, u32 align, u64 item_size, void** base, u64* len);
api_method void  arena_destroy    (Arena* arena);

#define alloc_item(arena_ptr, Type) \
((Type*)arena_alloc_item((arena_ptr), alignof(Type), sizeof(Type)))

#define alloc_array(arena_ptr, view_ptr) \
	arena_alloc_array(                   \
		(arena_ptr),                     \
		alignof((view_ptr)->ptr[0]),     \
		sizeof((view_ptr)->ptr[0]),      \
		(void**)&((view_ptr)->ptr),      \
		&((view_ptr)->len)               \
	)

#define scratch_begin() arena_frame_begin(&scratch_arena)
#define scratch_end()   arena_frame_end(  &scratch_arena)

#define scratch_alloc_item(Type)      alloc_item( &scratch_arena, Type)
#define scratch_alloc_array(view_ptr) alloc_array(&scratch_arena, view_ptr)

#endif // __ARENA_HPP__
