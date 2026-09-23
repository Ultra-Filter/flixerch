#pragma once

#include "../defines.h"

#define DSTRING_NULL_TERMINATOR '\0'

typedef char * dstr_t;
typedef struct _dstr_builder* dstr_builder_t;
typedef struct dstr_reader* dstr_reader_t; 

typedef struct dstr_memory_allocator
{
    void* (*allocate)(u64 size);
    void* (*reallocate)(void* block, u64 new_size);
    void  (*free)(void* block);
} dstr_memory_allocator;

/*
    TODO: Think if empty strings are allowed (FOR NOW IT IS NULL) because dstr is mutable it doesnt sounds right but think it through.
    TODO: Add support for conerting strings to other types (dstr to int, to float and so on)
    TODO: Add option for thread safe compilation of this header (and implementation ofcourse) 
    TODO: Add option to create dstr using external functions.
        For example: 
            To use this method: `GetCurrentDirectory(buffer, buffer_length)` from <windows.h>
            You need to think how to implement this so it can create directly a dstr without the need to do:
                char * _path = NULL;
                dstr_t path = NULL;
                DWORD res = GetCurrentDirectory(_path, 0);
                if (res)
                {
                    _path = malloc(res);
                    if (_path)
                    {
                        GetCurrentDirectory(_path, res);
                        path = dstr_create_from_cstr(_path, NULL);
                        free(_path);
                    }
                }
                ...

    TODO: Add dstr compare as an int using lexicographic ordering.
    TODO: Decide whether the dstr should also contain ref to the allocator for safier free.
    TODO: Expand builder & Reader functionalities.
    TODO: Add support for wide chars and codepoints.    
    TODO: Add definitions (macros) for using those functions without providing an allocator for simplicity.
    
        ==============
        Example 
        Implementation should look like...
        ==============
        #if !defined(DSTRING_NO_FUNCTIONS_WRAPPERS)

            #define DSTR_EXPAND(x) x
            #define DSTR_SELECT_MACRO(args, m2, m1, ...) m1

            #define DSTR_CREATE_FROM_CSTR_NAME _dstr_create_from_cstr

            #define _dtr_from_cstr_2(cstr, allocator) DSTR_CREATE_FROM_CSTR_NAME(cstr, allocator)
            #define _dtr_from_cstr_1(cstr) DSTR_CREATE_FROM_CSTR_NAME(cstr, NULL)
            #define dstr_create_from_cstr(...) DSTR_EXPAND(DSTR_SELECT_MACRO(__VA_ARGS__, _dtr_from_cstr_2, _dtr_from_cstr_1))(__VA_ARGS__)
            ...
        #else

            #define DSTR_CREATE_FROM_CSTR_NAME dstr_create_from_cstr
            ...

        #endif // !defined(DSTRING_NO_FUNCTIONS_WRAPPERS)

            dstr_t DSTR_CREATE_FROM_CSTR_NAME(const char* cstr, const dstr_memory_allocator* allocator);
            ...

        #if defined(DSTRING_IMPLEMENTATION)

            dstr_t DSTR_CREATE_FROM_CSTR_NAME(const char* cstr, const dstr_memory_allocator* allocator){ ... } 
            ...
        #endif // defined(DSTRING_IMPLEMENTATION)
        ===============================================
*/

dstr_t dstr_create_from_cstr(const char* cstr, const dstr_memory_allocator* allocator); // Allocates dstr_t with size `strlen(cstr)` - from null-terminated cstr
dstr_t dstr_create_from_chars(const char chars[], u64 count, const dstr_memory_allocator* allocator); // Allocates dstr_t with size `count` - from char[] chars
dstr_t dstr_create_from_dstr(const dstr_t dstr, const dstr_memory_allocator* allocator); // Allocates dstr_t with size `dstring___get__length(dstr)` - from dstr_t dstr
dstr_t dstr_create_from_cstr_format(const dstr_memory_allocator* allocator, const char* format, ...); // Allocates dstr_t with size `snprintf(format, ...)` 
dstr_t dstr_create_from_dstrs_concat(const dstr_t first, const dstr_t second, const dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with second
dstr_t dstr_create_from_cstrs_concat(const char *first, const char *second, const dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with second, NOTE: both are null-terminated strings
dstr_t dstr_create_from_chars_concat(const char first[], const char second[], u64 first_count, u64 second_count, const dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with second
dstr_t dstr_create_from_dstrs_concat_with_delim(const dstr_t first, const dstr_t second, const char join_delim, const dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with join delim which then concat with second
dstr_t dstr_create_from_cstrs_concat_with_delim(const char* first, const char* second, const char join_delim, const dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with join delim which then concat with second
dstr_t dstr_create_from_chars_concat_with_delim(const char first[], const char second[], u64 first_count, u64 second_count, const char join_delim, const dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with join delim which then concat with second
dstr_t dstr_create_substring_by_delim(const dstr_t dstr, const char delim, bool from_start, const dstr_memory_allocator* allocator); // return prefix of `dstr` to `delim` or suffix from `delim` based on from_start, NOTE return value does not contain the delim
dstr_t dstr_create_substring_by_delims(const dstr_t dstr, const char delims[], u64 delims_count, bool from_start, const dstr_memory_allocator* allocator); // return prefix of `dstr` to `delims` or suffix from `delims` based on from_start, NOTE return value does not contain the delim
bool   dstr_is_contain_char(const dstr_t dstr, const char c, u64 * index); // Returns true if there exists index where `dstring[index] == c`, And if `index != NULL` then set index = first index where `dstring[index] == c`
bool   dstr_is_contain_dstr(const dstr_t text, const dstr_t pattern, u64* index); // Returns true if there exists a substring of text wich matches pattern, And if `index != NULL` then set index = first index of first match
bool   dstr_is_contain_cstr(const dstr_t text, const char* pattern, u64* index); // Returns true if there exists a substring of text wich matches pattern, And if `index != NULL` then set index = first index of first match, NOTE: cstring must be null-terminated
bool   dstr_is_prefix_dstr(const dstr_t text, const dstr_t pattern); // Returns true if text[0...pattern_length] == pattern
bool   dstr_is_prefix_cstr(const dstr_t text, const char* pattern); // Returns true if text[0...pattern_length] == pattern, NOTE: cstring must be null-terminated
bool   dstr_is_suffix_dstr(const dstr_t text, const dstr_t pattern); // Returns true if text[text_len - pattern_length - 1 ... text_length - 1] == pattern
bool   dstr_is_suffix_cstr(const dstr_t text, const char* pattern); // Returns true if text[text_len - pattern_length - 1 ... text_length - 1] == pattern, NOTE: cstring must be null-terminated
bool   dstr_is_equal_dstrs(const dstr_t dstr1, const dstr_t dstr2); // return true <=> dstr1 == dstr2
bool   dstr_is_equal_cstrs(const dstr_t dstr, const char* cstr); // return true <=> dstr1 == dstr2
void   dstr_destroy(dstr_t dstr, const dstr_memory_allocator* allocator); // Free the allocation of `dstr`
void   dstr_to_lower(dstr_t dstr); // Convert dstr uppercase to lowercase and all other characters dont change
void   dstr_to_upper(dstr_t dstr); // Convert dstr lowercase to uppercase and all other characters dont change
void   dstr_replace_char(dstr_t dstr, char to_replace, char replacement); // replace the first `char == to_replace` in dstr to `replacement`
void   dstr_replace_chars(dstr_t dstr, char to_replace[], char replacement[], u64 length); // replace the first substring of dstr which equles to `to_replace` with `replacement`
void   dstr_break_by_delim(dstr_t dstr, char delim, dstr_t* out_array, u64* out_array_length, const dstr_memory_allocator* allocator); // breaks dstr to out_array = { d1, d2, d3, .., dk } where dstr = d1 + delim + d2 + delim + ... + delim + dk. out_array can be NULL in order to check what is out_array_length, NOTE: caller must allocate and free out_array  
void   dstr_break_by_delim_start_end(dstr_t dstr, char delim_start, char delim_end, char** out_array, const dstr_memory_allocator* allocator); // breaks dstr to out_array = { d1, d2, d3} where dstr = d1 + start_delim + d2 + end_delim + d3, NOTE: (out_array is dstr_t [3]), if di is empty then out_array[i] = NULL, if start_delim exists more then once we choose the first, and if end_delim exists more then once we choose the first that comes after delim_start.

dstr_builder_t  dstr_builder_create(u64 initial_capacity, const dstr_memory_allocator* allocator);
dstr_t          dstr_builder_generate_dstr(dstr_builder_t builder, const dstr_memory_allocator* allocator);
dstr_t          dstr_builder_chop_prefix_by_delim(dstr_builder_t builder, const char delim, const dstr_memory_allocator* allocator);
dstr_t          dstr_builder_chop_suffix_by_delim(dstr_builder_t builder, const char delim, const dstr_memory_allocator* allocator);
void            dstr_builder_destroy(dstr_builder_t builder);
void            dstr_builder_append_dstr(dstr_builder_t builder, dstr_t dstr);
void            dstr_builder_append_cstr(dstr_builder_t builder, const char* cstring);
void            dstr_builder_append_cstr_format(dstr_builder_t builder, const char* format, ...);
void            dstr_builder_append_chars(dstr_builder_t builder, const char chars[], u64 count);
void            dstr_builder_append_char(dstr_builder_t builder, const char c);
void            dstr_builder_append_dstrs(dstr_builder_t builder, const dstr_t* dstrs, u32 count);
void            dstr_builder_append_cstrs(dstr_builder_t builder, const char** cstrs, u32 count);
void            dstr_builder_append_dstrs_sepreate(dstr_builder_t builder, const dstr_t* dstrs, u32 count, const char seperator);
void            dstr_builder_append_cstrs_sepreate(dstr_builder_t builder, const char** cstrs, u32 count, const char seperator);
void            dstr_builder_replace_at_dstr(dstr_builder_t builder, dstr_t replacement ,u64 start_index);
void            dstr_builder_replace_at_cstr(dstr_builder_t builder, const char* replacement ,u64 start_index);
void            dstr_builder_replace_at_chars(dstr_builder_t builder, const char replacement[] , u64 count, u64 start_index);
void            dstr_builder_reserve(dstr_builder_t builder, u64 amount);
void            dstr_builder_remove_at_char(dstr_builder_t builder, u64 index);
void            dstr_builder_remove_at_chars(dstr_builder_t builder, u64 index, u64 count);
void            dstr_builder_remove_all_char(dstr_builder_t builder, const char c);
void            dstr_builder_replace_all_char(dstr_builder_t builder, const char to_replace, const char replacement);
void            dstr_builder_remove_prefix(dstr_builder_t builder, u64 prefix_len);
void            dstr_builder_remove_suffix(dstr_builder_t builder, u64 suffix_len);
void            dstr_builder_clear(dstr_builder_t builder);
char*           dstr_builder_get_chars(dstr_builder_t builder);
u64             dstr_builder_get_capacity(dstr_builder_t builder);
u64             dstr_builder_get_chars_count(dstr_builder_t builder);

dstr_reader_t  dstr_reader_create(dstr_t dstr, const dstr_memory_allocator* allocator);
void           dstr_reader_destroy(dstr_reader_t reader);
void           dstr_reader_skip_spaces(dstr_reader_t reader);
bool           dstr_reader_check_and_skip_char(dstr_reader_t reader, char c);
bool           dstr_reader_check_and_skip_dstr(dstr_reader_t reader, const dstr_t dstr);
bool           dstr_reader_check_and_skip_cstr(dstr_reader_t reader, const char *cstr);
bool           dstr_reader_check_and_skip_chars(dstr_reader_t reader, const char *chars, u32 count);
dstr_t         dstr_reader_read_and_skip_chars(dstr_reader_t reader, u32 count);
dstr_t         dstr_reader_read_and_skip_chars_to_delim(dstr_reader_t reader, char delim);
void           dstr_reader_skip_to_next_line(dstr_reader_t reader);
void           dstr_reader_go_back(dstr_reader_t reader, u32 amount);
bool           dstr_reader_find_first_char(dstr_reader_t reader, char c, u32* out_len);
bool           dstr_reader_is_finish(dstr_reader_t reader);
bool           dstr_reader_read_and_skip_i32(dstr_reader_t reader, i32* out);
bool           dstr_reader_read_and_skip_i64(dstr_reader_t reader, i64* out);
bool           dstr_reader_read_and_skip_f32(dstr_reader_t reader, f32* out);
bool           dstr_reader_read_and_skip_f64(dstr_reader_t reader, f64* out);
bool           dstr_reader_read_and_skip_bool(dstr_reader_t reader, bool* out);

#define dstrlen(dstr) (*(((const u64*)(dstr)) - 1))
       
#if defined(DSTRING_IMPLEMENTATION) || 1

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#define char_to_lower(c) (char)('A' <= (c) && (c) <= 'Z') ? ((c) + 'a' - 'A') : (c) 
#define char_to_upper(c) (char)('a' <= (c) && (c) <= 'z') ? ((c) - 'a' + 'A') : (c) 
#define DSTRING_DEFAULT_ALLOCATOR (dstr_memory_allocator){ .allocate = malloc, .reallocate = realloc, .free = free }

typedef struct _dstr_builder {
    char * chars;
    u64 capacity, count;
    dstr_memory_allocator allocator;
} _dstr_builder; 

typedef struct dstr_reader {
    char* curr;
    const char * end;
    const char * const data;
    dstr_memory_allocator allocator;
} dstr_reader;

#define DSTRING_ALLOCATE(size, allocator) ((allocator) == NULL) ? malloc((size)) : (allocator)->allocate((size))
#define DSTRING_REALLOCATE(block, size, allocator) ((allocator) == NULL) ? realloc((block), (size)) : (allocator)->reallocate((block), (size))
#define DSTRING_FREE(block, allocator) ((allocator) == NULL) ? free((block)) : (allocator)->free((block))

dstr_t dstr_create_from_cstr(const char* cstr, const dstr_memory_allocator* allocator)
{
    if (!cstr) return NULL;

    u64 cstr_len = strlen(cstr);

    if (!cstr_len) return NULL; // empty string
    
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + cstr_len + sizeof(char), allocator);
    if (!ptr) return NULL;
    ptr[ 0 ] = cstr_len;
    
    dstr_t dstr = (dstr_t)(ptr + 1);
    memcpy(dstr, cstr, cstr_len * sizeof(char));
    dstr[cstr_len] = DSTRING_NULL_TERMINATOR;

    return dstr;
}

dstr_t dstr_create_from_chars(const char chars[], u64 count, const dstr_memory_allocator* allocator)
{
    if (!chars) return NULL;

    if (!count) return NULL; // empty string

    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + count + sizeof(char), allocator);
    if (!ptr) return NULL;
    ptr[ 0 ] = count;

    dstr_t dstr = (dstr_t)(ptr + 1);
    memcpy(dstr, chars, count * sizeof(char));
    dstr[count] = DSTRING_NULL_TERMINATOR;

    return dstr;
}

dstr_t dstr_create_from_dstr(const dstr_t dstr, const dstr_memory_allocator* allocator)
{
    if (!dstr) return NULL;
    if (!dstrlen(dstr)) return NULL; // empty string - should not even be possible to have dstr with len 0 but in any case.

    u64* old_ptr = ((u64*)(dstr) - 1);
    u64* new_ptr = DSTRING_ALLOCATE(sizeof(u64) + old_ptr[0] + sizeof(char), allocator);
    if (!new_ptr) return NULL;
    dstr_t new_dstr = (dstr_t)(new_ptr + 1);

    memcpy(new_ptr, old_ptr, sizeof(u64) + old_ptr[0] + sizeof(char));

    return new_dstr;
}

dstr_t dstr_create_from_cstr_format(const dstr_memory_allocator* allocator, const char *format, ...)
{
    if (!format) return NULL;

    va_list args;
    va_list args_copy;
    va_start(args, format);
    va_start(args_copy, format);

    u64 str_len = vsnprintf(NULL, 0, format, args_copy);
    if (!str_len)
    {
        va_end(args); va_end(args_copy); 
        return NULL; // empty string
    }

    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + str_len + sizeof(char), allocator);
    if (!ptr) { va_end(args); va_end(args_copy);  return NULL;}
    dstr_t dstr = (dstr_t)(ptr + 1);
    
    ptr[ 0 ] = str_len;
    
    vsnprintf(dstr, str_len + 1, format, args);
    dstr[str_len] = DSTRING_NULL_TERMINATOR;

    va_end(args);
    va_end(args_copy);

    return dstr;
}

dstr_t dstr_create_from_dstrs_concat(const dstr_t first, const dstr_t second, const dstr_memory_allocator* allocator)
{
    if (!first || !second) return NULL;

    u64 first_len  = dstrlen(first);
    u64 second_len = dstrlen(second);

    if (first_len + second_len == 0) return NULL; // empty string - should not be possible for two dstrings to be with length 0 but in any case

    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + second_len + sizeof(char), allocator);
    if (!ptr) return NULL;
    ptr[ 0 ] = first_len + second_len;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_len * sizeof(char));
    memcpy(dstr + first_len, second, second_len * sizeof(char));

    dstr[first_len + second_len] = DSTRING_NULL_TERMINATOR;

    return dstr;
}

dstr_t dstr_create_from_cstrs_concat(const char *first, const char *second, const dstr_memory_allocator* allocator)
{
    if (!first || !second) return NULL;

    u64 first_len  = strlen(first);
    u64 second_len = strlen(second);

    if (first_len + second_len == 0) return NULL; // empty string 

    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + second_len + sizeof(char), allocator);
    if (!ptr) return NULL;
    ptr[ 0 ] = first_len + second_len;
    
    dstr_t dstr = (dstr_t)(ptr + 1);
    
    memcpy(dstr, first, first_len * sizeof(char));
    memcpy(dstr + first_len, second, second_len * sizeof(char));

    dstr[first_len + second_len] = DSTRING_NULL_TERMINATOR;
    
    return dstr;
}

dstr_t dstr_create_from_chars_concat(const char first[], const char second[], u64 first_count, u64 second_count, const dstr_memory_allocator* allocator)
{
    if (!first || !second) return NULL;

    if (first_count + second_count == 0) return NULL; // empty string 

    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_count + second_count + sizeof(char), allocator);
    if (!ptr) return NULL;

    ptr[ 0 ] = first_count + second_count;
    
    dstr_t dstr = (dstr_t)(ptr + 1);
    
    memcpy(dstr, first, first_count * sizeof(char));
    memcpy(dstr + first_count, second, second_count * sizeof(char));

    dstr[first_count + second_count] = DSTRING_NULL_TERMINATOR;
    
    return dstr;
}

dstr_t dstr_create_from_dstrs_concat_with_delim(dstr_t first, dstr_t second, char join_delim, const dstr_memory_allocator* allocator)
{
    if (!first || !second) return NULL;

    u64 first_len  = dstrlen(first);
    u64 second_len = dstrlen(second);
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + sizeof(char) + second_len + sizeof(char), allocator);
    if (!ptr) return NULL;

    ptr[ 0 ] = first_len + second_len + 1;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_len * sizeof(char));
    dstr[first_len] = join_delim;
    memcpy(dstr + first_len + 1, second, second_len * sizeof(char));

    dstr[first_len + second_len + 1] = DSTRING_NULL_TERMINATOR;

    return dstr;    
}

dstr_t dstr_create_from_cstrs_concat_with_delim(const char *first, const char *second, const char join_delim, const dstr_memory_allocator* allocator)
{
    if (!first || !second) return NULL;

    u64 first_len  = strlen(first);
    u64 second_len = strlen(second);
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + sizeof(char) + second_len + sizeof(char), allocator);
    if (!ptr) return NULL;

    ptr[ 0 ] = first_len + second_len + 1;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_len * sizeof(char));
    dstr[first_len] = join_delim;
    memcpy(dstr + first_len + 1, second, second_len * sizeof(char));

    dstr[first_len + second_len + 1] = DSTRING_NULL_TERMINATOR;

    return dstr; 
}

dstr_t dstr_create_from_chars_concat_with_delim(const char first[], const char second[], u64 first_count, u64 second_count, const char join_delim, const dstr_memory_allocator* allocator)
{
    if (!first || !second) return NULL;

    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_count + sizeof(char) + second_count + sizeof(char), allocator);
    if (!ptr) return NULL;

    ptr[ 0 ] = first_count + second_count + 1;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_count * sizeof(char));
    dstr[first_count] = join_delim;
    memcpy(dstr + first_count + 1, second, second_count * sizeof(char));

    dstr[first_count + second_count + 1] = DSTRING_NULL_TERMINATOR;

    return dstr; 
}

dstr_t dstr_create_substring_by_delim(const dstr_t dstr, const char delim, bool from_start, const dstr_memory_allocator* allocator)
{
    if (!dstr) return NULL;

    u64 dstr_len = dstrlen(dstr);
    const char * ptr = dstr;

    if (from_start)
    {
        u64 index = 0;
        while (*ptr++ != delim && index < dstr_len) index++;
        
        return dstr_create_from_chars(dstr, index, allocator);
    }
    else
    {
        u64 i = dstr_len;
        while (i > 0 && dstr[i - 1] != delim) i--;

        return dstr_create_from_chars(dstr + i, dstr_len - i, allocator);
    }
}

dstr_t dstr_create_substring_by_delims(const dstr_t dstr, const char delims[], u64 delims_count, bool from_start, const dstr_memory_allocator* allocator)
{
    if (!dstr || !delims_count) return NULL;

    u64 dstr_len = dstrlen(dstr);
    u64 start = from_start ? 0 : dstr_len - delims_count;
    u64 didx  = from_start ? 1 : -1;

    if (delims_count >= dstr_len) return NULL;

    for (u64 i = start; i <= dstr_len - delims_count; i += didx) // handeling overflow by i <= dstr_len - delims_count - infinite when dstring length is exactly U64_MAX - 1 (2^4 - 1) which is OK. 
    {
        bool match = true;

        for (u64 j = 0; j < delims_count; j++)
        {
            if (dstr[i + j] != delims[j])
            {
                match = false;
                break;
            }
        }
        
        if (match)
        {
            if (from_start)
            {
                return dstr_create_from_chars(dstr, i, allocator);
            }
            else
            {
                return dstr_create_from_chars(dstr + i + delims_count, dstr_len - i - delims_count, allocator);
            }
        }
    }

    return NULL; 
}

bool dstr_is_contain_char(const dstr_t dstr, const char c, u64 *index)
{
    if (!dstr) return false;

    u64 dstr_len = dstrlen(dstr); 

    u64 idx = 0;
    for (char * at = dstr; idx < dstr_len; at++, idx++)
    {
        if (*at == c)
        {
            if (index != NULL)
            {
                *index = idx;
            }
            return true;
        }
    }
    
    return false;
}

bool dstr_is_contain_dstr(const dstr_t text, const dstr_t pattern, u64 *index)
{
    if (!text || !pattern) return false;

    u64 text_len = dstrlen(text);
    u64 pattern_len = dstrlen(pattern);

    if (!pattern_len || pattern_len > text_len) return false;

    for (u64 i = 0; i <= text_len - pattern_len; i++)
    {
        bool found = true;
        for (char * t = text + i, * p = pattern; p < pattern + pattern_len; p++, t++)
        {
            if (*t != *p)
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            if (index != NULL)
            {
                *index = i;
            }
            return true;
        }
    }

    return false;
}

bool dstr_is_contain_cstr(const dstr_t text, const char *pattern, u64 *index)
{
    if (!text || !pattern) return false;

    u64 text_len = dstrlen(text);
    u64 pattern_len = strlen(pattern);

    if (!pattern_len || pattern_len > text_len) return false;

    for (u64 i = 0; i <= text_len - pattern_len; i++)
    {
        bool found = true;
        for (const char * t = text + i, * p = pattern; p < pattern + pattern_len; p++, t++)
        {
            if (*t != *p)
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            if (index != NULL)
            {
                *index = i;
            }
            return true;
        }
    }

    return false;
}

bool dstr_is_prefix_dstr(const dstr_t text, const dstr_t pattern)
{
    if (!text || !pattern) return false;

    u64 text_len = dstrlen(text);
    u64 pattern_len = dstrlen(pattern);

    if (pattern_len > text_len) return false;

    for (char * t = text, * p = pattern; p < pattern + pattern_len; p++, t++)
    {
        if (*t != *p)
        {
            return false;
        }
    }

    return true;
}

bool dstr_is_prefix_cstr(const dstr_t text, const char *pattern)
{
    if (!text || !pattern) return false;

    u64 text_len = dstrlen(text);
    u64 pattern_len = strlen(pattern);

    if (pattern_len > text_len) return false;

    for (const char * p = pattern , *t = text; p < pattern + pattern_len; p++, t++)
    {
        if (*t != *p)
        {
            return false;
        }
    }

    return true;
}

bool dstr_is_suffix_dstr(const dstr_t text, const dstr_t pattern)
{
    if (!text || !pattern) return false;

    u64 text_len = dstrlen(text);
    u64 pattern_len = dstrlen(pattern);
    
    if (pattern_len > text_len) return false;
    
    for (char* t = (text + text_len - pattern_len), *p = pattern; p < pattern + pattern_len; p++, t++)
    {
        if (*p != *t) return false;
    }
    
    return true;
}

bool dstr_is_suffix_cstr(const dstr_t text, const char *pattern)
{
    if (!text || !pattern) return false;

    u64 text_len = dstrlen(text);
    u64 pattern_len = strlen(pattern);
    
    if (pattern_len > text_len) return false;

    const char* p = pattern;
    for (char* t = (text + text_len - pattern_len); p < pattern + pattern_len; p++, t++)
    {
        if (*p != *t) return false;
    }
    
    return true;
}

bool dstr_is_equal_dstrs(const dstr_t dstr1, const dstr_t dstr2)
{
    if (!dstr1 || !dstr2) return false;

    u64 len1 = dstrlen(dstr1);
    u64 len2 = dstrlen(dstr2);

    if (len1 != len2) return false;

    const char * d1 = dstr1;
    const char * d2 = dstr2;

    for (u64 i = 0; i < len1; i++)
    {
        if (*d1++ != *d2++)
        {
            return false;
        }
    }

    return true;
}

bool dstr_is_equal_cstrs(const dstr_t dstr, const char *cstr)
{
    if (!dstr || !cstr) return false;

    u64 dstr_len = dstrlen(dstr);
    u64 cstr_len = strlen(cstr);

    if (dstr_len != cstr_len) return false;

    const char * d1 = dstr;
    const char * d2 = cstr;

    for (u64 i = 0; i < dstr_len; i++)
    {
        if (*d1++ != *d2++)
        {
            return false;
        }
    }

    return true;
}

void dstr_destroy(dstr_t dstr, const dstr_memory_allocator* allocator)
{
    if (!dstr) return;
    DSTRING_FREE((((u64*)dstr) - 1), allocator);
}

void dstr_to_lower(dstr_t dstr)
{
    if (!dstr) return;

    u64 dstr_len = dstrlen(dstr);
    
    for (char * d = dstr; d < dstr + dstr_len; d++)
    {
        *d = char_to_lower((*d));
    }
}

void dstr_to_upper(dstr_t dstr)
{
    if (!dstr) return;
    
    u64 dstr_len = dstrlen(dstr);
    
    for (char * d = dstr; d < dstr + dstr_len; d++)
    {
        *d = char_to_upper((*d));
    }
}

void dstr_replace_char(dstr_t dstr, char to_replace, char replacement)
{
    if (!dstr) return;

    u64 dstr_len = dstrlen(dstr);
    
    for (char * d = dstr; d < dstr + dstr_len; d++)
    {
        if (*d == to_replace)
        {
            *d = replacement;
            return;
        } 
    }
}

void dstr_replace_chars(dstr_t dstr, char to_replace[], char replacement[], u64 length)
{
    if (!dstr || !to_replace || !replacement) return;

    u64 dstr_len = dstrlen(dstr);
    
    if (length > dstr_len) return;

    for (u64 i = 0; i <= dstr_len - length; i++)
    {
        bool match = true;

        for (u64 j = 0; j < length; j++)
        {
            if (dstr[i + j] != to_replace[j])
            {
                match = false;
                break;
            }
        }

        if (match)
        {
            for (u64 j = 0; j < length; j++)
            {
                dstr[i + j] = replacement[j];
            }
            return;
        }
    }
}

void dstr_break_by_delim(dstr_t dstr, char delim, dstr_t *out_array, u64* out_array_length, const dstr_memory_allocator* allocator)
{
    if (!dstr) return;

    u64 dstr_len = dstrlen(dstr);
    u64 count = 0;
    const char* last = dstr;
    for (const char* p = dstr; p < dstr + dstr_len; p++)
    {
        if (*p == delim)
        {
            u64 len = (u64)(p - last);
            if (len > 0) // more then one delims in a raw - ignore
            {
                if (out_array != NULL) 
                {
                    out_array[ count ] = dstr_create_from_chars(last, len , allocator);
                } 
                count++;
            }
            last = p + 1; // ignores delim
        }
    }
    if (last < dstr + dstr_len)
    {
        u64 len = (u64)(dstr + dstr_len - last);
        if (out_array != NULL) 
        {
            out_array[ count ] = dstr_create_from_chars(last, len , allocator);
        } 
        count++;
    }
    if (out_array_length != NULL) *out_array_length = count;
}

void dstr_break_by_delim_start_end(dstr_t dstr, char delim_start, char delim_end, char **out_array, const dstr_memory_allocator *allocator)
{
    if (!dstr) return;

    u64 dstr_len = dstrlen(dstr);
    const char* start = NULL;
    const char* end = NULL;
    for (const char* p = dstr; p < dstr + dstr_len; p++)
    {
        if (start == NULL && *p == delim_start)
        {
            start = p + 1; // ignores delim
        }
        else if (start != NULL && *p == delim_end)
        {
            end = p - 1; // ignores delim
            break;
        }
    }

    if (start == NULL || end == NULL)
    {
        if (out_array)
        {
            out_array[ 0 ] = NULL;
            out_array[ 1 ] = NULL;
            out_array[ 2 ] = NULL;
        }
        return;
    }

    if (out_array != NULL)
    {
        u64 start_len = (u64)(start - dstr) - 1;
        if (start > dstr + 1)
        {
            out_array[ 0 ] = dstr_create_from_chars(dstr, start_len, allocator);
        }
        else 
        {
            out_array[ 0 ] = NULL;
        }
        u64 mid_len = (u64)(end - start) + 1;
        if (mid_len > 0)
        {
            out_array[ 1 ] = dstr_create_from_chars(start, mid_len, allocator);
        }
        else 
        {
            out_array[ 1 ] = NULL;
        }
        u64 end_len = (u64)(dstr + dstr_len - end) - 2;
        if (dstr + dstr_len > end + 2)
        {
            out_array[ 2 ] = dstr_create_from_chars(end + 2, end_len, allocator);
        }
        else 
        {
            out_array[ 2 ] = NULL;
        }
    }
}

dstr_builder_t dstr_builder_create(u64 initial_capacity, const dstr_memory_allocator* allocator)
{
    dstr_memory_allocator _allocator = allocator == NULL ? DSTRING_DEFAULT_ALLOCATOR : *allocator; 

    dstr_builder_t builder = _allocator.allocate(sizeof(_dstr_builder));
    if (!builder) return NULL;

    builder->capacity = initial_capacity == 0 ? 32 : initial_capacity;
    builder->chars    = DSTRING_ALLOCATE(sizeof(char) * builder->capacity, allocator);
    if (!builder->chars)
    {
        _allocator.free(builder);
        return NULL;
    }
    builder->count    = 0;
    builder->allocator = _allocator;

    return builder;
}

dstr_t dstr_builder_generate_dstr(dstr_builder_t builder, const dstr_memory_allocator* allocator)
{
    if (!builder) return NULL;
    return dstr_create_from_chars(builder->chars, builder->count, allocator);
}

dstr_t dstr_builder_chop_prefix_by_delim(dstr_builder_t builder, const char delim, const dstr_memory_allocator* allocator)
{
    if (!builder) return NULL;

    u64 delim_index = 0;
    for (; delim_index < builder->count && builder->chars[delim_index] != delim; delim_index++);

    dstr_t prefix = dstr_create_from_chars(builder->chars, delim_index, allocator);

    dstr_builder_remove_prefix(builder, delim_index + 1); // include delim as removed

    return prefix;
}

dstr_t dstr_builder_chop_suffix_by_delim(dstr_builder_t builder, const char delim, const dstr_memory_allocator* allocator)
{
    if (!builder) return NULL;

    u64 delim_index = builder->count - 1;
    for (; delim_index < builder->count && builder->chars[delim_index] != delim; delim_index--); // underflow checked by delim_index < builder->count - infinite loop if builder->count = 2^64-1 && delim not in builder->chars which is OK

    dstr_t dstr = dstr_create_from_chars(builder->chars + delim_index + 1, builder->count - delim_index - 1, allocator);

    dstr_builder_remove_suffix(builder, builder->count - delim_index);

    return dstr;
}

void dstr_builder_destroy(dstr_builder_t builder)
{
    if(builder != NULL)
    {
        if (builder->chars) builder->allocator.free(builder->chars);
        builder->allocator.free(builder);
    }
}

void dstr_builder_append_dstr(dstr_builder_t builder, dstr_t dstr)
{
    if (!builder || !dstr) return;
    u64 dstr_len = dstrlen(dstr);
    if (builder->capacity <= builder->count + dstr_len)
    {
        u64 new_capacity = builder->capacity + builder->count + dstr_len;
        void* block = builder->allocator.reallocate(builder->chars, new_capacity); 
        if (!block) return; // Silent Error.
        builder->chars = block;
        builder->capacity = new_capacity;
    }

    memcpy(builder->chars + builder->count, dstr, dstr_len);
    builder->count += dstr_len;
}

void dstr_builder_append_cstr(dstr_builder_t builder, const char *cstring)
{
    if (!builder || !cstring) return;

    u64 dstr_len = strlen(cstring);
    if (builder->capacity <= builder->count + dstr_len)
    {
        u64 new_capacity = builder->capacity + builder->count + dstr_len;
        void* block = builder->allocator.reallocate(builder->chars, new_capacity); 
        if (!block) return; // Silence Error
        builder->chars = block;
        builder->capacity = new_capacity;
    }
    
    memcpy(builder->chars + builder->count, cstring, dstr_len);
    builder->count += dstr_len;
}

void dstr_builder_append_cstr_format(dstr_builder_t builder, const char *format, ...)
{
    if (!builder || !format) return;

    va_list args;
    va_list args_copy;
    va_start(args, format);
    va_start(args_copy, format);

    u64 str_len = vsnprintf(NULL, 0, format, args_copy);

    if (builder->capacity <= builder->count + str_len)
    {
        u64 new_capacity = builder->capacity;
        while (new_capacity <= builder->count + str_len) new_capacity = new_capacity > 0 ? new_capacity * 2 : (1ULL << 5);
        void* block = builder->allocator.reallocate(builder->chars, new_capacity);
        
        if (!block) { va_end(args); va_end(args_copy); return; }
        builder->chars = block;
        builder->capacity = new_capacity;

    }
    
    vsnprintf(builder->chars + builder->count, str_len + 1, format, args);
    builder->count += str_len;

    va_end(args);
    va_end(args_copy);

}

void dstr_builder_append_chars(dstr_builder_t builder, const char chars[], u64 count)
{
    if (!builder || !chars) return;

    if (builder->capacity <= builder->count + count)
    {
        u64 new_capacity = builder->capacity + builder->count + count;
        void * block = builder->allocator.reallocate(builder->chars, new_capacity); 
        if (!block) return; // Silence Error
        builder->chars = block;
        builder->capacity = new_capacity;
    }
    
    memcpy(builder->chars + builder->count, chars, count);
    builder->count += count;
}

void dstr_builder_append_char(dstr_builder_t builder, const char c)
{
    if (!builder) return;

    if (builder->capacity <= builder->count + 1)
    {
        u64 new_cpapcity  = builder->capacity > 0 ? builder->capacity * 2 : (1ULL << 5);
        void* block = builder->allocator.reallocate(builder->chars, new_cpapcity); 
        if (!block) return; // Silence Error
        builder->chars = block;
        builder->capacity = new_cpapcity;
    }
    
    builder->chars[builder->count++] = c;
}

void dstr_builder_append_dstrs(dstr_builder_t builder, const dstr_t *dstrs, u32 count)
{
    if (!builder || !dstrs || !count) return;

    for (u32 i = 0; i < count; i++)
    {
        if (dstrs[ i ]) dstr_builder_append_dstr(builder, dstrs[ i ]);
    }
}

void dstr_builder_append_cstrs(dstr_builder_t builder, const char **cstrs, u32 count)
{
    if (!builder || !cstrs || !count) return;

    for (u32 i = 0; i < count; i++)
    {
        if (cstrs[ i ]) dstr_builder_append_cstr(builder, cstrs[ i ]);
    }
}

void dstr_builder_append_dstrs_sepreate(dstr_builder_t builder, const dstr_t *dstrs, u32 count, const char seperator)
{
    if (!builder || !dstrs || !count) return;

    for (u32 i = 0; i < count; i++)
    {
        if (dstrs[ i ]) 
        { 
            dstr_builder_append_dstr(builder, dstrs[ i ]);
            if (i + 1 < count)
            {
                dstr_builder_append_char(builder, seperator);
            }
        }
    }
}

void dstr_builder_append_cstrs_sepreate(dstr_builder_t builder, const char **cstrs, u32 count, const char seperator)
{
    if (!builder || !cstrs || !count) return;

    for (u32 i = 0; i < count; i++)
    {
        if (cstrs[ i ]) 
        { 
            dstr_builder_append_cstr(builder, cstrs[ i ]);
            if (i + 1 < count)
            {
                dstr_builder_append_char(builder, seperator);
            }
        }
    }
}

void dstr_builder_replace_at_dstr(dstr_builder_t builder, dstr_t replacement, u64 start_index)
{
    if (!builder || !replacement) return;

    if (dstrlen(replacement) + start_index > builder->count) return; // Silence error

    u64 replacement_len = dstrlen(replacement);
    memcpy(builder->chars + start_index, replacement, replacement_len);
}

void dstr_builder_replace_at_cstr(dstr_builder_t builder, const char *replacement, u64 start_index)
{
    if (!builder || !replacement) return;
    if (strlen(replacement) + start_index > builder->count) return; // Silence error
    u64 replacement_len = strlen(replacement);
    memcpy(builder->chars + start_index, replacement, replacement_len);
}

void dstr_builder_replace_at_chars(dstr_builder_t builder, const char replacement[], u64 count, u64 start_index)
{
    if (!builder || !replacement) return;
    if (count + start_index > builder->count) return; // Silence error
    memcpy(builder->chars + start_index, replacement, count);
}

u64 dstr_builder_get_capacity(dstr_builder_t builder)
{
    if (!builder) return 0ULL;
    return builder->capacity;
}

u64 dstr_builder_get_chars_count(dstr_builder_t builder)
{
    if (!builder) return 0ULL;
    return builder->count;
}

void dstr_builder_reserve(dstr_builder_t builder, u64 amount)
{
    if (!builder) return;
    u64 new_cpapcity = builder->capacity + amount;
    void* block = builder->allocator.reallocate(builder->chars, new_cpapcity);
    if (!block) return; // Silence Error
    builder->chars = block;
    builder->capacity = new_cpapcity;
}

void dstr_builder_remove_at_char(dstr_builder_t builder, u64 index)
{
    if (!builder) return;
    if (index >= builder->count) return; // Silence error
    memmove(builder->chars + index, builder->chars + index + 1, (--builder->count) - index);
}

void dstr_builder_remove_at_chars(dstr_builder_t builder, u64 index, u64 count)
{
    if (!builder) return;

    if (index + count > builder->count) 
    {
        count = index < builder->count ? builder->count - index : 0;
    }

    if (count > 0)
    {
        memmove(builder->chars + index, builder->chars + index + count, builder->count - index - count);
        builder->count -= count;
    }
}

void dstr_builder_remove_all_char(dstr_builder_t builder, const char c)
{
    if (!builder) return;

    char * ptr = builder->chars;
    u64 iteration = 0;
    u64 deleted_count = 0;
    
    while(iteration < builder->count)
    {
        if(*ptr == c)
        {
            memmove(ptr, ptr + 1, builder->count - (deleted_count++) - (u64)(ptr - builder->chars));
        }
        else
        {
            ptr++;
        }
        
        iteration++;
    }

    builder->count -= deleted_count;
}

void dstr_builder_replace_all_char(dstr_builder_t builder, const char to_replace, const char replacement)
{
    if (!builder) return;

    for (char * p = builder->chars; p < builder->chars + builder->count; p++)
    {
        *p = *p == to_replace ? replacement : *p;
    }
}

void dstr_builder_remove_prefix(dstr_builder_t builder, u64 prefix_len)
{
    if (!builder) return;

    if (prefix_len >= builder->count)
    {
        builder->count = 0;
        return;
    }
    memmove(builder->chars, builder->chars + prefix_len, builder->count - prefix_len);
    builder->count -= prefix_len;
}

void dstr_builder_remove_suffix(dstr_builder_t builder, u64 suffix_len)
{
    if (!builder) return;

    if (suffix_len >= builder->count)
    {
        builder->count = 0;
        return;
    }
    builder->count -= suffix_len;
}

void dstr_builder_clear(dstr_builder_t builder)
{
    if (builder)
    {
        builder->count = 0;
    }
}

char *dstr_builder_get_chars(dstr_builder_t builder)
{
    if (!builder) return NULL;
    return builder->chars;
}

dstr_reader_t dstr_reader_create(dstr_t dstr, const dstr_memory_allocator* _allocator)
{
    if (!dstr || !dstrlen(dstr)) return NULL;
    dstr_memory_allocator allocator = _allocator == NULL ? DSTRING_DEFAULT_ALLOCATOR : *_allocator;
    dstr_reader_t reader = (dstr_reader_t)allocator.allocate(sizeof(struct dstr_reader));
    if (!reader) return NULL; // Silence Error
    struct dstr_reader _reader = (struct dstr_reader){
        .allocator = allocator,
        .curr = dstr,
        .end = dstr + dstrlen(dstr),
        .data = dstr
    };
    memcpy(reader, &_reader, sizeof(struct dstr_reader));
    return reader;
}

void dstr_reader_destroy(dstr_reader_t reader)
{
    if (!reader) return;
    if(reader) reader->allocator.free(reader);
}

void dstr_reader_skip_spaces(dstr_reader_t reader)
{
    if (reader && reader->curr)
    {
        while (reader->curr < reader->end && isspace(reader->curr[0])) reader->curr++;
    }
}

bool dstr_reader_check_and_skip_char(dstr_reader_t reader, char c)
{
    if (reader && reader->curr && reader->curr < reader->end && reader->curr[0] == c) 
    { 
        reader->curr++;
        return true;
    }
    return false;
}

inline bool dstr_reader_check_and_skip_dstr(dstr_reader_t reader, const dstr_t dstr)
{
    if (reader && reader->curr && dstr)
    {
        u64 dstr_len = dstrlen(dstr);
        if (!dstr_len || reader->curr + dstr_len > reader->end) return false;
        
        for (u64 i = 0ULL; i < dstr_len; i++)
        {
            if (reader->curr[ i ] != dstr[ i ]) return false;
        }
        reader->curr += dstr_len;
        return true;
    } 
    return false;
}

inline bool dstr_reader_check_and_skip_cstr(dstr_reader_t reader, const char *cstr)
{
    if (reader && reader->curr && cstr)
    {
        u64 cstr_len = strlen(cstr);
        if (!cstr_len || reader->curr + cstr_len > reader->end) return false;
        
        for (u64 i = 0ULL; i < cstr_len; i++)
        {
            if (reader->curr[ i ] != cstr[ i ]) return false;
        }
        reader->curr += cstr_len;
        return true;
    } 
    return false;
}

bool dstr_reader_check_and_skip_chars(dstr_reader_t reader, const char *chars, u32 count)
{
    if (reader && reader->curr && chars && reader->curr + count <= reader->end)
    {
        char * ptr = chars;
        bool OK = true; u32 i = 0;
        char* p = reader->curr;
        while (p < reader->end && i++ < count)
        {
            if (*p++ != *ptr++)
            {
                OK = false;
                break;
            }
        }
        if (OK) reader->curr = p;
        return OK;
    }
    return false;
}

dstr_t dstr_reader_read_and_skip_chars(dstr_reader_t reader, u32 count)
{
    if (reader && reader->curr && reader->curr + count <= reader->end)
    {
        dstr_t dstr = dstr_create_from_chars(reader->curr, count, &reader->allocator);
        reader->curr += count;
        return dstr;
    }
    return NULL;
}

dstr_t dstr_reader_read_and_skip_chars_to_delim(dstr_reader_t reader, char delim)
{
    if (reader && reader->curr)
    {
        u32 i = 0;
        while (reader->curr + i < reader->end && *(reader->curr + i) != delim)
        {
            i++;
        }
        dstr_t dstr = dstr_create_from_chars(reader->curr, i, &reader->allocator);
        reader->curr += i;
        return dstr;
    }
    return NULL;
}

void dstr_reader_skip_to_next_line(dstr_reader_t reader)
{
    if (reader && reader->curr)
    {
        while (reader->curr < reader->end && *reader->curr++ != '\n');
    }
}

void dstr_reader_go_back(dstr_reader_t reader, u32 amount)
{
    if (reader && reader->curr)
    {
        reader->curr = (u32)(reader->curr - reader->data) >= amount ? reader->curr - amount : reader->data;
    }
}

bool dstr_reader_find_first_char(dstr_reader_t reader, char c, u32* out_len)
{
    if (reader && reader->curr)
    {
        u32 i = 0;
        while (reader->curr + i < reader->end)
        {
            if (*(reader->curr + i) == c)
            {
                if (out_len) *out_len = i;
                
                return true;
            }
            i++;
        }
    }
    return false;
}

bool dstr_reader_is_finish(dstr_reader_t reader)
{
    if (reader && reader->curr) return reader->curr >= reader->end;
    return true;
}

bool dstr_reader_read_and_skip_i32(dstr_reader_t reader, i32 *out)
{
    if (reader && reader->curr && reader->curr < reader->end)
    {
        char* end = reader->curr;
        errno = 0;
        long outcome = strtol(reader->curr, &end, 0);
        if (errno != 0 || end == reader->curr) return false;

        if (out) *out = (i32)outcome;
        reader->curr = end;
        return true;
    }
    return false;
}

bool dstr_reader_read_and_skip_i64(dstr_reader_t reader, i64 *out)
{
    if (reader && reader->curr && reader->curr < reader->end)
    {
        char* end = reader->curr;
        errno = 0;
        long long outcome = strtoll(reader->curr, &end, 0);
        if (errno != 0 || end == reader->curr) return false;

        if (out) *out = (i64)outcome;
        reader->curr = end;
        return true;
    }
    return false;
}

bool dstr_reader_read_and_skip_f32(dstr_reader_t reader, f32 *out)
{
    if (reader && reader->curr && reader->curr < reader->end)
    {
        char* end = reader->curr;
        errno = 0;
        f32 outcome = strtof(reader->curr, &end);
        if (errno != 0 || end == reader->curr) return false;

        if (out) *out = (f32)outcome;
        reader->curr = end;
        return true;
    }
    return false;
}

bool dstr_reader_read_and_skip_f64(dstr_reader_t reader, f64 *out)
{
    if (reader && reader->curr && reader->curr < reader->end)
    {
        char* end = reader->curr;
        errno = 0;
        f64 outcome = strtod(reader->curr, &end);
        if (errno != 0 || end == reader->curr) return false;

        if (out) *out = (f64)outcome;
        reader->curr = end;
        return true;
    }
    return false;
}

#define DSTR_INSENSITIVE_CHARCMP(c1, c2) ((c1) == (c2)) || (((c1) - (c2)) == 'A' - 'a') || (((c1) - (c2)) == 'a' - 'A')

bool dstr_reader_read_and_skip_bool(dstr_reader_t reader, bool *out)
{
    if (reader && reader->curr && reader->curr < reader->end)
    {
        char* p = reader->curr;
        while (p < reader->end && isspace(*p++));
        char* start = p;
        while (p < reader->end && !isspace(*p++));
        char* end = p;
        u32 len = (u32)(end - start);
        char word[ 5 ] = { 0 };
        if (len != 5 && len != 4) return false;
        else if (len == 4) memcpy(word, "true", 4); 
        else memcpy(word, "false", 4); 
        
        for (u32 i = 0; i < len; i++)
        {
            bool identical = DSTR_INSENSITIVE_CHARCMP(word[ i ], start[ i ]);
            if (!identical) return false;
        }

        if (out) *out = (len == 4);
        reader->curr += len;
        return true;
    }
    return false;
}

#endif // defined(DSTRING_IMPLEMENTATION)