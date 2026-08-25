#pragma once

#include "../defines.h"

typedef struct hash_table_s hash_table_s;

typedef enum hash_type_e {
    HASH_TYPE_FNV1A, // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
    HASH_TABLE_TYPE_COUNT
} hash_type_e;

hash_table_s * hash_table_create(u64 table_capacity, u32 item_size_in_bytes, hash_type_e hash_type);
void hash_table_destroy(hash_table_s* table);
// Inserts copy of value into the table if the table[hash(key)] is empty otherwise fails.
// Returns true if insert false otherwise
bool hash_table_insert(hash_table_s* table, const char* key, void* value);
// Populate the out_value with table[hash(key)] if not empty otherwise fails.
// Returns true if table[hash(value)] is not empty.
bool hash_table_get(hash_table_s* table, const char* key, void* out_value);
// Make the table[hash(key)] empty and if it wasnt empty already and out_value is not NULL then populate the out_value with the content and returns true.
// out_value CAN be NULL if you dont care what was in table[hash(key)]
bool hash_table_remove(hash_table_s* table, const char* key, void* out_value);


#define DEFAULT_HASH_TYPE HASH_TYPE_FNV1A
// Macro helper for easier init. you can now use new_hash_tabe(1024, int);
#define new_hash_table(capacity, type) hash_table_create(capacity, sizeof(type), DEFAULT_HASH_TYPE)
