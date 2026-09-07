#pragma once
#include "core/utils/defines.h"

typedef struct hash_tabel_mem_allocator {
    void* (*allocate)(u64);
    void* (*reallocate)(void*, u64);
    void (*free)(void*);
} dhash_tabel_mem_allocator;

typedef enum dhash_collision_resolution_type {
    DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST, // https://en.wikipedia.org/wiki/Hash_table#Separate_chaining
    DHASH_COLLISION_RESOLUTION_TYPE_BINARY_SEARCH_TREE, // https://en.wikipedia.org/wiki/Hash_table#Separate_chaining
    DHASH_COLLISION_RESOLUTION_TYPE_OPEN_ADDRESSING, // https://en.wikipedia.org/wiki/Hash_table#Open_addressing
    DHASH_COLLISION_RESOLUTION_TYPE_COUNT,
} dhash_collision_type;

typedef enum dhash_type_e {
    HASH_TYPE_MURMUR3_32, // https://en.wikipedia.org/wiki/MurmurHash#Algorithm
    HASH_TYPE_FNV1A, // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
    HASH_TYPE_AP, 
    HASH_TYPE_DEK, 
    HASH_TYPE_DJB, 
    HASH_TYPE_SDBM, 
    HASH_TYPE_COUNT
} dhash_type_e;

typedef struct dhash_table_t dhash_table_t;
typedef struct dhash_table_iter {
    dhash_table_t* table;
    u64 index;
} dhash_table_iter;


dhash_table_t* dhash_table_create(
    u64 capacity, 
    u32 value_size_in_bytes, 
    dhash_type_e hash_type, 
    dhash_collision_type collision_resolution_type,
    dhash_tabel_mem_allocator* allocator
);

void dhash_table_destroy(dhash_table_t*table);
bool dhash_table_add (dhash_table_t* table, const char* key, void* value);
bool dhash_table_remove (dhash_table_t* table, const char* key);
bool dhash_table_get    (dhash_table_t* table, const char* key, void* out_value);
bool dhash_table_set    (dhash_table_t* table, const char* key, void* new_value);
bool dhash_table_is_exists(dhash_table_t* table, const char* key);

#if defined(DHASH_TABLE_IMPLEMENTATION) 

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define _DHASH_LINKED_CHAIN_GET_ENTRY(t,i) *((dhash_chain_entry**)((u8*)BLOCK_FROM_TABLE(t) + i * sizeof(dhash_chain_entry*)))
#define DEFAULT_HASH_TABLE_ALLOCATOR (dhash_tabel_mem_allocator){ .allocate = malloc, .reallocate = realloc, .free =  free }
#define BLOCK_FROM_TABLE(t) (u8*)((t) + 1)
#define TABLE_JUMP_TO(t, i) (u8*)(BLOCK_FROM_TABLE(t) + (i) * ( t->type_size + sizeof(bool) )) 

typedef struct dhash_table_t { 
    dhash_type_e type;
    dhash_collision_type collision_type;
    u32 value_size;
    dhash_tabel_mem_allocator allocator;
    u64 capacity;
} dhash_table_t;

typedef struct dhash_chain_entry {
    char * key;
    void* value;
    u64 hash;
    struct dhash_chain_entry* next;
} dhash_chain_entry;
typedef struct dhash_addressing_entry {
    char * key;
    void* value;
    u64 hash;
    struct dhash_addressing_entry* next;
} dhash_addressing_entry;

static u64 _DHASH_(dhash_type_e type ,const char* key)
{
    u64 h = 0;
    switch (type)
    {
        case HASH_TYPE_MURMUR3_32:
        {
            h = (u64)(key); // Seed ~ random by pointer
            u32 k;
            u32 keylen = strlen(key);
            const u8* ptr = (const u8*)key;
            for (u64 i = keylen >> 2; i > 0; i--)
            {
                memcpy(&k, ptr, sizeof(u32));
                ptr += sizeof(u32);
                u32 r = k;
                r *= 0xcc9e2d51;
                r = (r << 15) | (r >> 17);
                r *= 0x1b873593;
                h ^= r;
                h = (h << 13) | (h >> 19);
                h = h * 5 + 0xe6546b64; 
            }
            
            k = 0;
            for (u64 i = keylen & 3; i > 0; i--)
            {
                k <<= 8;
                k |= (u32)key[ keylen - i - 1 ];
            }
            
            u32 r = k;
            r *= 0xcc9e2d51;
            r = (r << 15) | (r >> 17);
            r *= 0x1b873593;
            h ^= r;
            h ^= (h >> 16);
            h *= 0x85ebca6b;
            h ^= (h >> 13);
            h *= 0xc2b2ae35;
            h ^= (h >> 16);
        } break;
        case HASH_TYPE_FNV1A:
        {
            const u64 prime = 1099511628211ULL;
            const u64 offset = 14695981039346656037ULL;
            h = offset;
            for (const u8* p = (const u8*)key; p < (u8*)key + strlen(key); p++)
            {
                h ^= (u64)(unsigned char)(*p);
                h *= prime;
            }
        } break;
        case HASH_TYPE_AP:
        {
            h = 0xAAAAAAAA;
            for (u64 i = 0; i < strlen(key); i++) {
                if ((i & 1) == 0) {
                    h ^= ((h << 7) ^ (u8)key[i] * (h >> 3));
                } else {
                    h ^= (~((h << 11) + ((u8)key[i] ^ (h >> 5))));
                }
            }
        } break;
        case HASH_TYPE_DEK:
        {
            h = strlen(key);
            u32 i;
            
            for (i = 0; i < strlen(key); ++i)
            {
                h = ((h << 5) ^ (h >> 27)) ^ (u8)(key[i]);
            }
        } break;
        case HASH_TYPE_DJB:
        {
            h = 5381;
            u32 i;
            
            for (i = 0; i < strlen(key); ++i)
            {
                h = ((h << 5) + h) + (u8)(key[i]);
            }
        } break;
        case HASH_TYPE_SDBM:
        {
            h = 0;
            u32 i;
            
            for (i = 0; i < strlen(key); ++i)
            {
                h = (u8)key[0] + (h << 6) + (h << 16) - h;
            }
        } break;
        default: break;
    }
    return h;
}

static inline dhash_table_t* _dhash_linked_chain_create_(u64 capacity, u32 value_size, dhash_tabel_mem_allocator allocator)
{
    dhash_table_t* table;
    table = (dhash_table_t*)allocator.allocate(sizeof(dhash_table_t) + capacity * sizeof(dhash_chain_entry*));
    assert(table);
    table->allocator = allocator;
    table->capacity = capacity;
    table->value_size = value_size;

    memset(BLOCK_FROM_TABLE(table), 0, capacity * sizeof(dhash_chain_entry*));

    return table;
}

static inline void _dhash_linked_chain_destroy_(dhash_table_t* table)
{
    if (table)
    {
        for (u32 i = 0; i < table->capacity; i++)
        {
            dhash_chain_entry* entry = _DHASH_LINKED_CHAIN_GET_ENTRY(table, i);

            while (entry != NULL)
            {
                dhash_chain_entry* t = entry;
                entry = entry->next;
                if (t)
                {
                    if (t->key) table->allocator.free(t->key);
                    if (t->value) table->allocator.free(t->value);
                    table->allocator.free(t);
                }
            }
        }
    }
}

static inline bool _dhash_linked_chain_insert_(dhash_table_t* table, const char * key, void* value)
{
    assert(table && key && value);
    
    u64 hash = _DHASH_(table->type, key) % table->capacity;
    dhash_chain_entry* entry = _DHASH_LINKED_CHAIN_GET_ENTRY(table, hash);
    
    if (entry != NULL)
    {
        while (entry->next != NULL){
            if (strcmp(entry->key, key) == 0)
            {
                // Key duplicate
                return false;
            }
            entry = entry->next;
        } 
        if (strcmp(entry->key, key) == 0)
        {
            // Key duplicate
            return false;
        }
        entry->next = (dhash_chain_entry*)table->allocator.allocate(sizeof(dhash_chain_entry));
        entry = entry->next;
    }
    else
    {
        entry = (dhash_chain_entry*)table->allocator.allocate(sizeof(dhash_chain_entry));
    }
    u32 keylen = strlen(key);
    entry->hash  = hash;
    entry->key   = table->allocator.allocate(sizeof(char) * (keylen + 1));
    memcpy(entry->key, key, keylen);
    entry->key[keylen] = '\0';
    entry->next  = NULL;
    entry->value = table->allocator.allocate(table->value_size);
    memcpy(entry->value, value, table->value_size);

    _DHASH_LINKED_CHAIN_GET_ENTRY(table, hash) = entry;
    
    return true;
}

static inline bool _dhash_linked_chain_delete_(dhash_table_t* table, const char * key)
{
    assert(table && key);
    
    u64 hash = _DHASH_(table->type, key) % table->capacity;
    dhash_chain_entry* entry = _DHASH_LINKED_CHAIN_GET_ENTRY(table, hash);
    
    if (!entry) return false;

    if (strcmp(entry->key, key) == 0)
    {
        if (entry->next)
        {
            dhash_chain_entry* temp = entry;
            entry = entry->next;
            if (temp->value) table->allocator.free(temp->value);
            if (temp->key) table->allocator.free(temp->key);
            table->allocator.free(temp);
        }
        else
        {
            if (entry->value) table->allocator.free(entry->value);
            if (entry->key) table->allocator.free(entry->key);
            table->allocator.free(entry);
            entry = NULL;
        }
        return true;
    }
    
    while (entry->next != NULL && strcmp(entry->next->key, key) != 0) entry = entry->next;
    
    if (entry->next != NULL)
    {
        dhash_chain_entry* temp = entry->next;
        entry->next = entry->next->next;
        if (temp->value) table->allocator.free(temp->value);
        if (temp->key) table->allocator.free(temp->key);
        table->allocator.free(temp);
        return true;
    }
    
    return false;
}

static inline bool _dhash_linked_chain_get_(dhash_table_t* table, const char* key, void* out_value)
{
    assert(table && key && out_value);
    
    u64 hash = _DHASH_(table->type, key) % table->capacity;
    dhash_chain_entry* entry = _DHASH_LINKED_CHAIN_GET_ENTRY(table, hash);

    if (!entry) return false;
    
    if (strcmp(entry->key, key) == 0) 
    {
        memcpy(out_value, entry->value, table->value_size);
        return true;
    }
    
    while (entry != NULL && strcmp(entry->key, key) != 0) entry = entry->next;

    if (!entry) return false;
    
    memcpy(out_value, entry->value, table->value_size);
    return true;
}

static inline bool _dhash_linked_chain_set_(dhash_table_t* table, const char* key, void* new_value)
{
    assert(table && key && new_value);
    
    u64 hash = _DHASH_(table->type, key) % table->capacity;
    dhash_chain_entry* entry = _DHASH_LINKED_CHAIN_GET_ENTRY(table, hash);
    
    if (!entry) return false;

    if (strcmp(entry->key, key) == 0) 
    {
        memcpy(entry->value, new_value, table->value_size);
        return true;
    }
    
    while (entry != NULL && strcmp(entry->key, key) != 0) entry = entry->next;

    if (!entry) return false;

    memcpy(entry->value, new_value, table->value_size);
    return true;
}

bool _dhash_linked_chain_is_exists_(dhash_table_t* table, const char* key)
{
    u64 hash = _DHASH_(table->type, key) % table->capacity;
    dhash_chain_entry* entry = _DHASH_LINKED_CHAIN_GET_ENTRY(table, hash);

    if (!entry) return false;

    if (strcmp(entry->key, key) == 0) 
    {
        return true;
    }
    
    while (entry != NULL && strcmp(entry->key, key) != 0) entry = entry->next;

    if (!entry) return false;

    return true;
}

dhash_table_t* dhash_table_create(u64 capacity, u32 value_size, dhash_type_e hash_type, dhash_collision_type collision_type, dhash_tabel_mem_allocator* _allocator)
{
    dhash_tabel_mem_allocator allocator = _allocator != NULL ? *_allocator : DEFAULT_HASH_TABLE_ALLOCATOR;
    dhash_table_t* table;
    
    switch (collision_type)
    {
        case DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST: 
        {
            table = _dhash_linked_chain_create_(capacity, value_size, allocator);
        } break;
        default: assert(0 && "UNREACHABLE - unrecognized collision type"); return NULL;
    }
    
    table->type           = hash_type;
    table->collision_type = collision_type;
    
    return table;
}

inline bool dhash_table_add(dhash_table_t *table, const char *key, void *value)
{
    switch (table->collision_type)
    {
        case DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST: return _dhash_linked_chain_insert_(table, key, value);
        case DHASH_COLLISION_RESOLUTION_TYPE_OPEN_ADDRESSING: return true; // TODO:
        default: assert(0 && "UNREACHABLE - unrecognized hash collision type."); return false; // UNREACHABLE
    }
    return false; // Compiler warnings - Unreachable 
}

inline bool dhash_table_remove(dhash_table_t *table, const char *key)
{
    switch (table->collision_type)
    {
        case DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST: return _dhash_linked_chain_delete_(table, key);
        case DHASH_COLLISION_RESOLUTION_TYPE_OPEN_ADDRESSING: return true; // TODO:
        default: assert(0 && "UNREACHABLE - unrecognized hash collision type."); return false; // UNREACHABLE
    }
    return false; // Compiler warnings - Unreachable
}

inline bool dhash_table_get(dhash_table_t *table, const char *key, void *out_value)
{
    assert(table);
    switch (table->collision_type)
    {
        case DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST: return _dhash_linked_chain_get_(table, key, out_value);
        case DHASH_COLLISION_RESOLUTION_TYPE_OPEN_ADDRESSING: return true; // TODO:
        default: assert(0 && "UNREACHABLE - unrecognized hash collision type."); return false; // UNREACHABLE
    }
    return false; // Compiler warnings - Unreachable
}

inline bool dhash_table_set(dhash_table_t *table, const char *key, void *new_value)
{
    switch (table->collision_type)
    {
        case DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST: return _dhash_linked_chain_set_(table, key, new_value);
        case DHASH_COLLISION_RESOLUTION_TYPE_OPEN_ADDRESSING: return true; // TODO:
        default: assert(0 && "UNREACHABLE - unrecognized hash collision type."); return false; // UNREACHABLE
    }
    return false; // Compiler warnings - Unreachable
}

inline bool dhash_table_is_exists(dhash_table_t *table, const char *key)
{
    switch (table->collision_type)
    {
        case DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST: return _dhash_linked_chain_is_exists_(table, key);
        case DHASH_COLLISION_RESOLUTION_TYPE_OPEN_ADDRESSING: return true; // TODO:
        default: assert(0 && "UNREACHABLE - unrecognized hash collision type."); return false; // UNREACHABLE
    }
    return false;
}

void dhash_table_destroy(dhash_table_t*table)
{
    switch (table->collision_type)
    {
        case DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST: _dhash_linked_chain_destroy_(table); break;
        case DHASH_COLLISION_RESOLUTION_TYPE_OPEN_ADDRESSING: break; // TODO:
        default: assert(0 && "UNREACHABLE - unrecognized hash collision type."); break; // UNREACHABLE
    }
}

#endif