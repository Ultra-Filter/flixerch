#include "hash_table.h"

typedef u64 (*hash_function_ptr)(const char*);

typedef struct hash_table_s {
    hash_type_e hash_type;
    u32 item_size_in_bytes;
    u64 capacity;
    void * data;
} hash_table_s;

// =============================================================
//                      FNV1A Hash Function
// =============================================================

#define FNV1A_PRIME 1099511628211UL
#define FNV1A_OFFSET_PRIME 14695981039346656037UL

#define NULL_TERMINATOR '\0'

static u64 fnv1a_hash(const char* key)
{
    u64 hash_value = FNV1A_OFFSET_PRIME;
    for (const char* p = key; *p != NULL_TERMINATOR; p++)
    {
        hash_value ^= (u64)((u8)(*p));
        hash_value *= FNV1A_PRIME;
    }
    return hash_value;
}

// =============================================================

static hash_function_ptr get_hash_func(hash_type_e hash_type)
{
    switch (hash_type)
    {
        case HASH_TYPE_FNV1A: return fnv1a_hash;
        default:  break;
    }
    return fnv1a_hash; // default hash_function
}


hash_table_s * hash_table_create(u64 table_capacity, u32 item_size_in_bytes, hash_type_e hash_type)
{
    hash_table_s* table = malloc(sizeof(hash_table_s));
    assert(table != NULL);

    table->capacity           = table_capacity;
    table->hash_type          = hash_type;
    table->item_size_in_bytes = item_size_in_bytes;
    table->data = calloc(table_capacity, (item_size_in_bytes + sizeof(bool)));
    assert(table->data != NULL);
    
    return table;
}

void hash_table_destroy(hash_table_s* table)
{
    if (table)
    {
        if (table->data)
        {
            free(table->data);
        }
        free(table);
    }
}

bool hash_table_insert(hash_table_s* table, const char* key, void* value)
{
    assert(table != NULL);
    hash_function_ptr hash_func = get_hash_func(table->hash_type);
    u64 index = hash_func(key) % table->capacity;

    bool* occupied = table->data + (table->item_size_in_bytes + sizeof(bool)) * index;
    if (*occupied)
    {
        return false;
    }
    *occupied = true;
    u8* data = (u8*)occupied + 1;

    memcpy(data, value, table->item_size_in_bytes);
    
    return true;
}

bool hash_table_get(hash_table_s* table, const char* key, void* out_value)
{
    assert(table != NULL && out_value != NULL);
    hash_function_ptr hash_func = get_hash_func(table->hash_type);
    u64 index = hash_func(key) % table->capacity;
   
    bool* occupied = table->data + (table->item_size_in_bytes + sizeof(bool)) * index;
    u8* data = (u8*)occupied + 1;
    if (*occupied)
    {
        memcpy(out_value, data, table->item_size_in_bytes);
        return true;
    }

    return false;
}

bool hash_table_remove(hash_table_s* table, const char* key, void* out_value)
{
    assert(table != NULL);
    hash_function_ptr hash_func = get_hash_func(table->hash_type);
    u64 index = hash_func(key) % table->capacity;
   
    bool* occupied = table->data + (table->item_size_in_bytes + sizeof(bool)) * index;
    u8* data = (u8*)occupied + 1;
    if (*occupied)
    {
        *occupied = false;

        if (out_value != NULL)
        {
            memcpy(out_value, data, table->item_size_in_bytes);
        }
        return true;
    }
    return false;
}
