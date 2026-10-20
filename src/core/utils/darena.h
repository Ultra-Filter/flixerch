#pragma once 
#include "defines.h"

typedef struct darena* darena_t;

typedef enum darena_type : u8 {
    // Moves a pointer forward for every new request
    // Advantages:
    //     - Constant time allocation (just pointer += size)
    //     - Ultra fast
    // Disadvantages:
    //      - Cannot free individual objects 
    //      - can only free entire arena
    DARENA_TYPE_LINEAR, 
    // Operates like linear arena but supports Last-In-First-Out
    // Advantages:
    //     - Constant time allocation (just pointer += size)
    //     - Ultra fast
    //     - Can free individual objects - only in reversed order of allocation
    // Disadvantages:
    //      - Cannot free individual arbitrary objects 
    DARENA_TYPE_STACK,
    // Allocating chunk (or more then one) at a time.
    // Advantages:
    //      - Can free individual arbitrary request
    //      - Very fast
    // Disadvantages:
    //      - Internal-Fragmentation - allocating a chunk at a time regardless of size requested by user.
    //      - Memory usage is greater then asked unless user only allocates sizes of chuncks.
    DARENA_TYPE_CHUNKED,
    DARENA_TYPE_COUNT
} darena_type;

darena_t darena_create  (const darena_type arena_type, const u64 reserve_size);
void*    darena_malloc  (darena_t arena, u64 size);
void*    darena_calloc  (darena_t arena, u64 size);
void*    darena_realloc (darena_t arena, void* block, u64 size);
void     darena_free    (darena_t arena, void* block);
void     darena_clear   (darena_t arena);
void     darena_destroy (darena_t arena);
u64      darena_commit_offset (darena_t arena);
u64      darena_total_allocations_size (darena_t arena);
u64      darena_total_allocations_count (darena_t arena);

#define DKB(x) ((u64)(x) << 10) 
#define DMB(x) ((u64)(x) << 20)
#define DGB(x) ((u64)(x) << 30)

#if defined(DARENA_IMPLEMENTATION)

#define DARENA_TINY   (1ULL << 5)
#define DARENA_SMALL  (1ULL << 8)
#define DARENA_MEDIUM (1ULL << 12)
#define DARENA_LARGE  (1ULL << 18)
#define DARENA_HUGE   (1ULL << 21)

#define SIZE_TO_CHUNK(s) s < DARENA_TINY ? CHUNK_TYPE_TINY :\
     s < DARENA_SMALL ? CHUNK_TYPE_SMALL: \
     s < DARENA_MEDIUM ? CHUNK_TYPE_MEDIUM : \
     s < DARENA_LARGE ? CHUNK_TYPE_LARGE : \
     s < DARENA_HUGE ? CHUNK_TYPE_HUGE : \
    CHUNK_TYPE_COUNT

#define IS_OCCUPIED(b) (b & (1U << 7)) 
#define SET_CHUNK_HEAD(type, occupied) (occupied) ? ((1U << 7) | (type)) : (type)
#define MAX(a,b) a > b ? a : b
#define MIN(a,b) a < b ? a : b

#define CHUNK_SIZE(type) ((type) == CHUNK_TYPE_TINY ? DARENA_TINY : \
    (type )== CHUNK_TYPE_SMALL ? DARENA_SMALL : \
    (type )== CHUNK_TYPE_MEDIUM ? DARENA_MEDIUM : \
    (type )== CHUNK_TYPE_LARGE ? DARENA_LARGE : \
    (type )== CHUNK_TYPE_HUGE ? DARENA_HUGE : \
    0)
#define SKIP_CHUNK(p)  (p) = (u8*)(p) + (CHUNK_SIZE(((*((u8*)p)) & 31U))) 
#define AS_U8PTR(p) ((u8*)(p))

#define IS_ENOUGH_SPACE(p, size)  (CHUNK_SIZE((*(p) & 31U)) >= (size))
#define BIGGEST_CHUNK_IN(size) size > DARENA_HUGE ? CHUNK_TYPE_HUGE :\
                size > DARENA_LARGE  ? CHUNK_TYPE_LARGE :\
                size > DARENA_MEDIUM ? CHUNK_TYPE_MEDIUM :\
                size > DARENA_SMALL  ? CHUNK_TYPE_SMALL :\
                size > DARENA_TINY   ?  CHUNK_TYPE_TINY :\
                0

typedef enum chunk_type : u8 {
    CHUNK_TYPE_TINY   = 1U << 0,
    CHUNK_TYPE_SMALL  = 1U << 1,
    CHUNK_TYPE_MEDIUM = 1U << 2,
    CHUNK_TYPE_LARGE  = 1U << 3,
    CHUNK_TYPE_HUGE   = 1U << 4,
    CHUNK_TYPE_COUNT  = 1U << 5
} chunk_type;

#include <memoryapi.h>
#include <sysinfoapi.h>

typedef struct darena
{
    darena_type type;
    unsigned long page_size;
    u64 reserve_capacity, commit_offset;
    u64 offset;
    u64 total_allocations, allocations_count;
} * darena_t;

static inline DWORD _darena_get_platform_page_size_(void)
{
    SYSTEM_INFO si = { 0 };
    GetSystemInfo(&si);
    return si.dwPageSize;
}

static inline void* _darena_platform_reserve_(u64 size)
{
    void* ret = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
    return ret;
}

static inline bool _darena_platform_commit_(void* ptr, u64 size)
{
    void * ret = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return ret != NULL;
}

static inline bool _darena_platform_decommit_(void* ptr, u64 size)
{
    return VirtualFree(ptr, size, MEM_DECOMMIT);
}

static inline bool _darena_platform_release_(void* ptr, u64 size)
{
    return VirtualFree(ptr, size, MEM_RELEASE);
}

static inline u64 _darena_round_up_(u64 x, u64 to)
{
    u64 res = x;
    u64 mod = x % to;
    if (mod) res += to - mod;
    return res;
}

static inline void _darena_chop_to_chunks_(u8* ptr, u64 size)
{
    u8* z = ptr;
    while (z < ptr + size)
    {
        chunk_type type = BIGGEST_CHUNK_IN(size - (u64)(z - ptr));
        if (!type) 
        {
            memset(z, 0, size - (u64)(z - ptr));
            return;
        }
        *z = SET_CHUNK_HEAD(type, 0);
        z += CHUNK_SIZE(type);
    }
} 

darena_t darena_create(darena_type arena_type, u64 reserve_size)
{
    DWORD page_size = _darena_get_platform_page_size_();

    u64 size = _darena_round_up_(reserve_size, (u64)page_size);

    void* block = _darena_platform_reserve_(size);
    _darena_platform_commit_(block, page_size);
    darena_t arena = (darena_t)block;

    arena->type              = arena_type;
    arena->reserve_capacity  = reserve_size;
    arena->commit_offset     = page_size;
    arena->page_size         = page_size;
    arena->total_allocations = 0;
    arena->allocations_count = 1;

    switch (arena_type)
    {
        case DARENA_TYPE_LINEAR:
        {
            arena->offset = sizeof(struct darena);

        } break;
        case DARENA_TYPE_STACK:
        {
            arena->offset = sizeof(struct darena) + sizeof(u64);
            
        } break;
        case DARENA_TYPE_CHUNKED:
        {
            arena->offset = sizeof(struct darena);
            _darena_chop_to_chunks_(
                AS_U8PTR(arena) + sizeof(struct darena), 
                arena->commit_offset - sizeof(struct darena)
            ); 
        } break;
        default: break; 
    }
    return arena;
}

void *darena_malloc(darena_t arena, u64 size)
{
    arena->total_allocations += size;
    arena->allocations_count += 1;
    switch (arena->type)
    {
        case DARENA_TYPE_LINEAR:
        {
            if (arena->offset + size >= arena->commit_offset)
            {
               u64 size_to_commit = _darena_round_up_(size, (u64)arena->page_size);
               if (arena->offset + size_to_commit >= arena->reserve_capacity) return NULL;
               _darena_platform_commit_(AS_U8PTR(arena) + arena->commit_offset, size_to_commit);
               arena->commit_offset += size_to_commit;
            }
            void* block = AS_U8PTR(arena) + arena->offset;
            arena->offset += size;
            return block;
        } break;
        case DARENA_TYPE_STACK:
        {
            if (arena->offset + size >= arena->commit_offset)
            {
               u64 size_to_commit = _darena_round_up_(size, (u64)arena->page_size);
               if (arena->offset + size_to_commit >= arena->reserve_capacity) return NULL;
               _darena_platform_commit_(AS_U8PTR(arena) + arena->commit_offset, size_to_commit);
               arena->commit_offset += size_to_commit;
            }
            
            u8* p = (AS_U8PTR(arena) + arena->offset); 
            void* block = p;
            p += size;
            *(u64*)p = size;
            arena->offset += size + sizeof(u64);
            return block;
        } break;
        case DARENA_TYPE_CHUNKED:
        {
            chunk_type type = SIZE_TO_CHUNK((size + 1));
            if (type == CHUNK_TYPE_COUNT) return NULL;
            u64 chunk_size = CHUNK_SIZE(type);
            u8* p = AS_U8PTR(arena) + sizeof(struct darena);
            while (IS_OCCUPIED(*p) || !IS_ENOUGH_SPACE(p, chunk_size))
            {
                SKIP_CHUNK(p);
                if (p >= AS_U8PTR(arena) + arena->commit_offset)
                {
                    u64 size_to_commit = _darena_round_up_(
                        MAX((u64)(p - arena->commit_offset), chunk_size), 
                        arena->page_size
                    );
                    if (arena->commit_offset + size_to_commit >= arena->reserve_capacity) return NULL;
                    _darena_platform_commit_(AS_U8PTR(arena) + arena->commit_offset, size_to_commit);
                    _darena_chop_to_chunks_(AS_U8PTR(arena) + arena->commit_offset, size_to_commit);
                    arena->commit_offset += size_to_commit;
                }
            }
            chunk_type new_type = (*p & 31U);
            u64 size_left = (CHUNK_SIZE(new_type)) - chunk_size;
            *p = SET_CHUNK_HEAD(type, 1);
            void * block = AS_U8PTR(p) + 1;
            p = AS_U8PTR(p) +  chunk_size;
            if (size_left) _darena_chop_to_chunks_(p, size_left);
            return block;
        } break;
        default: break; 
    }
    return NULL;
}

void *darena_calloc(darena_t arena, u64 size)
{
    void* block = darena_malloc(arena, size);
    if (block) memset(block, 0, size);
    return block;
}

void *darena_realloc(darena_t arena, void *block, u64 size)
{
    if (!block) return block;
    switch (arena->type)
    {
        case DARENA_TYPE_LINEAR:
        {
            void* new_block = darena_malloc(arena, size);
            if (new_block)
            {
                memcpy(new_block, block, MIN(size, arena->commit_offset - sizeof(struct darena)));
            }
            return new_block;
        } break;
        case DARENA_TYPE_STACK:
        {
            u64* p = (u64*)(AS_U8PTR(arena) + arena->offset);
            if ((u8*)block + *(p - 1) + sizeof(u64) == (u8*)p) 
            {
                if ((u64)((u8*)block - AS_U8PTR(arena)) + size > arena->commit_offset)
                {
                    u64 size_to_commit = _darena_round_up_(
                        (u64)((u8*)block - AS_U8PTR(arena)) + size - arena->commit_offset, 
                        arena->page_size
                    );
                    if (arena->commit_offset + size_to_commit >= arena->reserve_capacity) return NULL;
                    _darena_platform_commit_(AS_U8PTR(arena) + arena->commit_offset, size_to_commit);
                    arena->commit_offset += size_to_commit;
                }
                *(u64*)((u8*)block + size) = size;
                arena->offset = (u64)((u8*)block - AS_U8PTR(arena)) + size + sizeof(u64);
                return block;
            }
            else
            {
                void* new_block = darena_malloc(arena, size);
                if (new_block)
                {
                    memcpy(new_block, block, MIN(size, *(p - 1)));
                }
                return new_block; 
            }
        } break;
        case DARENA_TYPE_CHUNKED:
        {
            void * new_block = darena_malloc(arena, size);
            if (new_block)
            {
                u64 old_size = CHUNK_SIZE(((*(((u8*)block) - 1)) & 31U));
                memcpy(new_block, block, MIN(size, old_size));
                darena_free(arena, block);
            }
            return new_block;
        } break;
        default: break; 
    }
    return NULL;
}

void darena_free(darena_t arena, void *block)
{
    // TODO: If commit_offset is too big -> decommit it.
    switch (arena->type)
    {
        case DARENA_TYPE_LINEAR: return;
        case DARENA_TYPE_STACK:
        {
            u64* p = (u64*)(AS_U8PTR(arena) + arena->offset);
            u64 size = *(p - 1);
            arena->offset -= size + sizeof(u64);
            memset(AS_U8PTR(arena) + arena->offset, 0, size);
        } break;
        case DARENA_TYPE_CHUNKED:
        {
            if (!block) return;
            u8* p = (u8*)(block);
            if (!IS_OCCUPIED((*(p-1)))) return;
            *(p - 1) &= 31U; 
        } break;
        default: return;
    }
}

void darena_clear(darena_t arena)
{
    switch (arena->type)
    {
        case DARENA_TYPE_LINEAR:
        {
            memset(AS_U8PTR(arena) + sizeof(struct darena), 0, arena->offset - sizeof(struct darena));
            arena->offset = sizeof(struct darena);
            arena->total_allocations = 0;
        } break;
        case DARENA_TYPE_STACK:
        {
            arena->offset = sizeof(struct darena) + sizeof(u64);
        } break;
        case DARENA_TYPE_CHUNKED:
        {
            arena->offset = sizeof(struct darena);
            memset(AS_U8PTR(arena) + sizeof(struct darena), 0, arena->commit_offset - sizeof(struct darena));
            _darena_chop_to_chunks_(
                AS_U8PTR(arena) + sizeof(struct darena), 
                arena->commit_offset - sizeof(struct darena)
            );
        } break;
        default: break; 
    }
}

void darena_destroy(darena_t arena)
{
    _darena_platform_release_(arena, arena->reserve_capacity);
}

inline u64 darena_commit_offset(darena_t arena)
{
    return arena->commit_offset;
}

inline u64 darena_total_allocations_size(darena_t arena)
{
    return arena->total_allocations;
}

inline u64 darena_total_allocations_count(darena_t arena)
{
    return arena->allocations_count;
}

#endif