#pragma once

#include "../defines.h"

#define DSTRING_NULL_TERMINATOR '\0'

typedef char * dstr_t;
typedef struct _dstr_builder _dstr_builder;
typedef _dstr_builder* dstr_builder_t;

typedef struct dstr_memory_allocator
{
    void* (*allocate)(u64 size);
    void* (*reallocate)(void* block, u64 new_size);
    void  (*free)(void* block);
} dstr_memory_allocator;

/*
    TODO: Add definitions (macros) for using those functions without providing an allocator for simplicity
    
    Implementation should look like...

    #define DSTRING_EXPAND(x) x
    #define DSTRING_SELECT_MACRO(args, macro2, macro1, ...) macro1

    ==============
        Example 
    ==============
    dstr_t _dstring___create__from_cstring(const char* cstr, dstr_memory_allocator* allocator); // Allocates dstr_t with size `strlen(cstr)` - from null-terminated cstr
    #define _1_dstring___create__from_cstring(cstr) _dstring___create__from_cstring(cstr, NULL)
    #define _2_dstring___create__from_cstring(cstr, allocator) _dstring___create__from_cstring(cstr, allocator)
    #define dstring___create__from_cstring(...)   \
        EXPAND(                                   \
            DSTRING_SELECT_MACRO(                 \
                __VA_ARGS__,                      \
                _2_dstring___create__from_cstring,\
                _1_dstring___create__from_cstring \
                )                                 \
            (__VA_ARGS__)
    ===============================================
*/


dstr_t dstr_create_from_cstr(const char* cstr, dstr_memory_allocator* allocator); // Allocates dstr_t with size `strlen(cstr)` - from null-terminated cstr
dstr_t dstr_create_from_chars(const char chars[], u64 count, dstr_memory_allocator* allocator); // Allocates dstr_t with size `count` - from char[] chars
dstr_t dstr_create_from_dstr(const dstr_t dstr, dstr_memory_allocator* allocator); // Allocates dstr_t with size `dstring___get__length(dstr)` - from dstr_t dstr
dstr_t dstr_create_from_cstr_format(dstr_memory_allocator* allocator, const char* format, ...); // Allocates dstr_t with size `snprintf(format, ...)` 
dstr_t dstr_create_from_dstrs_concat(const dstr_t first, const dstr_t second, dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with second
dstr_t dstr_create_from_cstrs_concat(const char *first, const char *second, dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with second, NOTE: both are null-terminated strings
dstr_t dstr_create_from_chars_concat(const char first[], const char second[], u64 first_count, u64 second_count, dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with second
dstr_t dstr_create_from_dstrs_concat_with_delim(const dstr_t first, const dstr_t second, const char join_delim, dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with join delim which then concat with second
dstr_t dstr_create_from_cstrs_concat_with_delim(const char* first, const char* second, const char join_delim, dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with join delim which then concat with second
dstr_t dstr_create_from_chars_concat_with_delim(const char first[], const char second[], u64 first_count, u64 second_count, const char join_delim, dstr_memory_allocator* allocator); // Allocates dstr_t equals to copy of first concat with join delim which then concat with second
dstr_t dstr_create_substring_by_delim(const dstr_t dstr, const char delim, bool from_start, dstr_memory_allocator* allocator); // return prefix of `dstr` to `delim` or suffix from `delim` based on from_start, NOTE return value does not contain the delim
dstr_t dstr_create_substring_by_delims(const dstr_t dstr, const char delims[], u64 delims_count, bool from_start, dstr_memory_allocator* allocator); // return prefix of `dstr` to `delims` or suffix from `delims` based on from_start, NOTE return value does not contain the delim
bool   dstr_is_contain_char(const dstr_t dstr, const char c, u64 * index); // Returns true if there exists index where `dstring[index] == c`, And if `index != NULL` then set index = first index where `dstring[index] == c`
bool   dstr_is_contain_dstr(const dstr_t text, const dstr_t pattern, u64* index); // Returns true if there exists a substring of text wich matches pattern, And if `index != NULL` then set index = first index of first match
bool   dstr_is_contain_cstr(const dstr_t text, const char* pattern, u64* index); // Returns true if there exists a substring of text wich matches pattern, And if `index != NULL` then set index = first index of first match, NOTE: cstring must be null-terminated
bool   dstr_is_prefix_dstr(const dstr_t text, const dstr_t pattern); // Returns true if text[0...pattern_length] == pattern
bool   dstr_is_prefix_cstr(const dstr_t text, const char* pattern); // Returns true if text[0...pattern_length] == pattern, NOTE: cstring must be null-terminated
bool   dstr_is_suffix_dstr(const dstr_t text, const dstr_t pattern); // Returns true if text[text_len - pattern_length - 1 ... text_length - 1] == pattern
bool   dstr_is_suffix_cstr(const dstr_t text, const char* pattern); // Returns true if text[text_len - pattern_length - 1 ... text_length - 1] == pattern, NOTE: cstring must be null-terminated
bool   dstr_is_equal_dstrs(const dstr_t dstr1, const dstr_t dstr2); // return true <=> dstr1 == dstr2
bool   dstr_is_equal_cstrs(const dstr_t dstr, const char* cstr); // return true <=> dstr1 == dstr2
void   dstr_destroy(dstr_t dstr, dstr_memory_allocator* allocator); // Free the allocation of `dstr`
void   dstr_to_lower(dstr_t dstr); // Convert dstr uppercase to lowercase and all other characters dont change
void   dstr_to_upper(dstr_t dstr); // Convert dstr lowercase to uppercase and all other characters dont change
void   dstr_replace_char(dstr_t dstr, char to_replace, char replacement); // replace the first `char == to_replace` in dstr to `replacement`
void   dstr_replace_chars(dstr_t dstr, char to_replace[], char replacement[], u64 length); // replace the first substring of dstr which equles to `to_replace` with `replacement`
void   dstr_break_by_delim(dstr_t dstr, char delim, dstr_t* out_array, u64* out_array_length, dstr_memory_allocator* allocator); // breaks dstr to out_array = { d1, d2, d3, .., dk } where dstr = d1 + delim + d2 + delim + ... + delim + dk. out_array can be NULL in order to check what is out_array_length, NOTE: caller must allocate and free out_array  
void   dstr_break_by_delim_start_end(dstr_t dstr, char delim_start, char delim_end, char** out_array, dstr_memory_allocator* allocator); // breaks dstr to out_array = { d2, d2, d3} where dstr = d1 + start_delim + d2 + end_delim + d3, NOTE: (out_array is dstr_t [3]), if di is empty then out_array[i] = NULL, if start_delim exists more then once we choose the first, and if end_delim exists more then once we choose the first that comes after delim_start.

dstr_builder_t  dstr_builder_create(u64 initial_capacity, dstr_memory_allocator* allocator);
dstr_t          dstr_builder_generate_dstr(dstr_builder_t builder, dstr_memory_allocator* allocator);
dstr_t          dstr_builder_chop_prefix_by_delim(dstr_builder_t builder, const char delim, dstr_memory_allocator* allocator);
dstr_t          dstr_builder_chop_suffix_by_delim(dstr_builder_t builder, const char delim, dstr_memory_allocator* allocator);
void            dstr_builder_destroy(dstr_builder_t builder);
void            dstr_builder_append_dstr(dstr_builder_t builder, dstr_t dstr);
void            dstr_builder_append_cstr(dstr_builder_t builder, const char* cstring);
void            dstr_builder_append_cstr_format(dstr_builder_t builder, const char* format, ...);
void            dstr_builder_append_chars(dstr_builder_t builder, const char chars[], u64 count);
void            dstr_builder_append_char(dstr_builder_t builder, const char c);
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
char*           dstr_builder_get_chars(dstr_builder_t builder);
u64             dstr_builder_get_capacity(dstr_builder_t builder);
u64             dstr_builder_get_chars_count(dstr_builder_t builder);

#define dstrlen(dstr) (((u64*)dstr) - 1)[ 0 ]
       
#if defined(DSTRING_IMPLEMENTATION)

#define char_to_lower(c) (char)('A' <= (c) && (c) <= 'Z') ? ((c) + 'a' - 'A') : (c) 
#define char_to_upper(c) (char)('a' <= (c) && (c) <= 'z') ? ((c) - 'a' + 'A') : (c) 
#define DSTRING_DEFAULT_ALLOCATOR (dstr_memory_allocator){ .allocate = malloc, .reallocate = realloc, .free = free }

typedef struct _dstr_builder {
    char * chars;
    u64 capacity, count;
    dstr_memory_allocator allocator;
} _dstr_builder; 

#define DSTRING_ALLOCATE(size, allocator) ((allocator) == NULL) ? malloc((size)) : (allocator)->allocate((size))
#define DSTRING_REALLOCATE(block, size, allocator) ((allocator) == NULL) ? realloc((block), (size)) : (allocator)->reallocate((block), (size))
#define DSTRING_FREE(block, allocator) ((allocator) == NULL) ? free((block)) : (allocator)->free((block))

dstr_t dstr_create_from_cstr(const char* cstr, dstr_memory_allocator* allocator)
{
    u64 cstr_len = strlen(cstr);
    
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + cstr_len + sizeof(char), allocator);
    ptr[ 0 ] = cstr_len;
    
    dstr_t dstr = (dstr_t)(ptr + 1);
    memcpy(dstr, cstr, cstr_len * sizeof(char));
    dstr[cstr_len] = DSTRING_NULL_TERMINATOR;

    return dstr;
}

dstr_t dstr_create_from_chars(const char chars[], u64 count, dstr_memory_allocator* allocator)
{
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + count + sizeof(char), allocator);
    ptr[ 0 ] = count;

    dstr_t dstr = (dstr_t)(ptr + 1);
    memcpy(dstr, chars, count * sizeof(char));
    dstr[count] = DSTRING_NULL_TERMINATOR;

    return dstr;
}

dstr_t dstr_create_from_dstr(const dstr_t dstr, dstr_memory_allocator* allocator)
{
    u64* old_ptr = ((u64*)dstr) - 1;
    u64* new_ptr = DSTRING_ALLOCATE(sizeof(u64) + old_ptr[0] + sizeof(char), allocator);
    dstr_t new_dstr = (dstr_t)(new_ptr + 1);

    memcpy(new_ptr, old_ptr, sizeof(u64) + old_ptr[0] + sizeof(char));

    return new_dstr;
}

dstr_t dstr_create_from_cstr_format(dstr_memory_allocator* allocator, const char *format, ...)
{
    va_list args;
    va_list args_copy;
    va_start(args, format);
    va_start(args_copy, format);

    u64 str_len = vsnprintf(NULL, 0, format, args_copy) + sizeof(char);

    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + str_len + sizeof(char), allocator);
    dstr_t dstr = (dstr_t)(ptr + 1);
    
    ptr[ 0 ] = str_len;
    
    vsnprintf(dstr, str_len, format, args);
    dstr[str_len] = DSTRING_NULL_TERMINATOR;

    va_end(args);
    va_end(args_copy);

    return dstr;
}

dstr_t dstr_create_from_dstrs_concat(const dstr_t first, const dstr_t second, dstr_memory_allocator* allocator)
{
    u64 first_len  = dstrlen(first);
    u64 second_len = dstrlen(second);
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + second_len + sizeof(char), allocator);
    ptr[ 0 ] = first_len + second_len;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_len * sizeof(char));
    memcpy(dstr + first_len, second, second_len * sizeof(char));

    dstr[first_len + second_len] = DSTRING_NULL_TERMINATOR;

    return dstr;
}

dstr_t dstr_create_from_cstrs_concat(const char *first, const char *second, dstr_memory_allocator* allocator)
{
    u64 first_len  = strlen(first);
    u64 second_len = strlen(second);
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + second_len + sizeof(char), allocator);
    ptr[ 0 ] = first_len + second_len;
    
    dstr_t dstr = (dstr_t)(ptr + 1);
    
    memcpy(dstr, first, first_len * sizeof(char));
    memcpy(dstr + first_len, second, second_len * sizeof(char));

    dstr[first_len + second_len] = DSTRING_NULL_TERMINATOR;
    
    return dstr;
}

dstr_t dstr_create_from_chars_concat(const char first[], const char second[], u64 first_count, u64 second_count, dstr_memory_allocator* allocator)
{
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_count + second_count + sizeof(char), allocator);
    ptr[ 0 ] = first_count + second_count;
    
    dstr_t dstr = (dstr_t)(ptr + 1);
    
    memcpy(dstr, first, first_count * sizeof(char));
    memcpy(dstr + first_count, second, second_count * sizeof(char));

    dstr[first_count + second_count] = DSTRING_NULL_TERMINATOR;
    
    return dstr;
}

dstr_t dstr_create_from_dstrs_concat_with_delim(dstr_t first, dstr_t second, char join_delim, dstr_memory_allocator* allocator)
{
    u64 first_len  = dstrlen(first);
    u64 second_len = dstrlen(second);
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + sizeof(char) + second_len + sizeof(char), allocator);
    ptr[ 0 ] = first_len + second_len + 1;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_len * sizeof(char));
    dstr[first_len] = join_delim;
    memcpy(dstr + first_len + 1, second, second_len * sizeof(char));

    dstr[first_len + second_len + 1] = DSTRING_NULL_TERMINATOR;

    return dstr;    
}

dstr_t dstr_create_from_cstrs_concat_with_delim(const char *first, const char *second, const char join_delim, dstr_memory_allocator* allocator)
{
    u64 first_len  = strlen(first);
    u64 second_len = strlen(second);
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_len + sizeof(char) + second_len + sizeof(char), allocator);
    ptr[ 0 ] = first_len + second_len + 1;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_len * sizeof(char));
    dstr[first_len] = join_delim;
    memcpy(dstr + first_len + 1, second, second_len * sizeof(char));

    dstr[first_len + second_len + 1] = DSTRING_NULL_TERMINATOR;

    return dstr; 
}

dstr_t dstr_create_from_chars_concat_with_delim(const char first[], const char second[], u64 first_count, u64 second_count, const char join_delim, dstr_memory_allocator* allocator)
{
    u64* ptr = DSTRING_ALLOCATE(sizeof(u64) + first_count + sizeof(char) + second_count + sizeof(char), allocator);
    ptr[ 0 ] = first_count + second_count + 1;

    dstr_t dstr = (dstr_t)(ptr + 1);

    memcpy(dstr, first, first_count * sizeof(char));
    dstr[first_count] = join_delim;
    memcpy(dstr + first_count + 1, second, second_count * sizeof(char));

    dstr[first_count + second_count + 1] = DSTRING_NULL_TERMINATOR;

    return dstr; 
}

dstr_t dstr_create_substring_by_delim(const dstr_t dstr, const char delim, bool from_start, dstr_memory_allocator* allocator)
{
    u64 dstr_len = dstrlen(dstr);
    const char * ptr = dstr;
    if (from_start)
    {
        u64 index = 0;
        while (*ptr++ != delim && index++ < dstr_len);
        
        return dstr_create_from_chars(dstr, index, allocator);
    }
    else
    {
        u64 index = dstr_len - 1;
        while (*ptr++ != delim && index-- != 0);

        return dstr_create_from_chars(dstr, index, allocator);
    }
}

dstr_t dstr_create_substring_by_delims(const dstr_t dstr, const char delims[], u64 delims_count, bool from_start, dstr_memory_allocator* allocator)
{
    u64 dstr_len = dstrlen(dstr);
    u64 start = from_start ? 0 : dstr_len - delims_count;
    u64 didx  = from_start ? 1 : -1;

    if (delims_count <= dstr_len)
    {

        for (u64 i = start; i != 0 && i <= dstr_len - delims_count; i += didx)
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
    }
    return dstr_create_from_cstr("", allocator); // empty string when error ????
}

bool dstr_is_contain_char(const dstr_t dstr, const char c, u64 *index)
{
    u64 dstr_len = (((u64*)dstr) - 1)[ 0 ]; // could use dstring___get__length(dstr);

    u64 idx = 0;
    for (char * at = dstr; at < dstr + dstr_len; at++, idx++)
    {
        if (at[0] == c)
        {
            if (index != NULL)
            {
                *index = idx;
                return true;
            }
        }
    }
    
    return false;
}

bool dstr_is_contain_dstr(const dstr_t text, const dstr_t pattern, u64 *index)
{
    u64 text_len = (((u64*)text) - 1)[ 0 ];
    u64 pattern_len = (((u64*)pattern) - 1)[ 0 ];

    if (pattern_len > text_len) return false;

    for (u64 i = 0; i < text_len - pattern_len; i++)
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
    u64 text_len = (((u64*)text) - 1)[ 0 ];
    u64 pattern_len = strlen(pattern);

    if (pattern_len > text_len) return false;

    for (u64 i = 0; i < text_len - pattern_len; i++)
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
    u64 text_len = (((u64*)text) - 1)[ 0 ];
    u64 pattern_len = (((u64*)pattern) - 1)[ 0 ];

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
    u64 text_len = (((u64*)text) - 1)[ 0 ];
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
    u64 text_len = (((u64*)text) - 1)[0];
    u64 pattern_len = (((u64*)pattern) - 1)[0];
    
    if (pattern_len >= text_len) return false;
    
    for (char* t = (text + text_len - pattern_len), *p = pattern; p < pattern + pattern_len; p++, t++)
    {
        if (*p != *t) return false;
    }
    
    return true;
}

bool dstr_is_suffix_cstr(const dstr_t text, const char *pattern)
{
    u64 text_len = (((u64*)text) - 1)[0];
    u64 pattern_len = strlen(pattern);
    
    if (pattern_len >= text_len) return false;
    const char* p = pattern;
    for (char* t = (text + text_len - pattern_len); p < pattern + pattern_len; p++, t++)
    {
        if (*p != *t) return false;
    }
    
    return true;
}

bool dstr_is_equal_dstrs(const dstr_t dstr1, const dstr_t dstr2)
{
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

void dstr_destroy(dstr_t dstr, dstr_memory_allocator* allocator)
{
    DSTRING_FREE((((u64*)dstr) - 1), allocator);
}

void dstr_to_lower(dstr_t dstr)
{
    u64 dstr_len = dstrlen(dstr);
    
    for (char * d = dstr; d < dstr + dstr_len; d++)
    {
        *d = char_to_lower((*d));
    }
}

void dstr_to_upper(dstr_t dstr)
{
    u64 dstr_len = dstrlen(dstr);
    
    for (char * d = dstr; d < dstr + dstr_len; d++)
    {
        *d = char_to_upper((*d));
    }
}

void dstr_replace_char(dstr_t dstr, char to_replace, char replacement)
{
    u64 dstr_len = dstrlen(dstr);
    
    for (char * d = dstr; d < dstr + dstr_len; d++)
    {
        if (*d == to_replace)
        {
            *d = replacement;
        } 
    }
}

void dstr_replace_chars(dstr_t dstr, char to_replace[], char replacement[], u64 length)
{
    u64 dstr_len = dstrlen(dstr);
    
    if (length >= dstr_len) return;

    for (u64 i = 0; i < dstr_len - length; i++)
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
        }
    }
}

void dstr_break_by_delim(dstr_t dstr, char delim, dstr_t *out_array, u64* out_array_length, dstr_memory_allocator* allocator)
{
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
                last = p + 1; // ignores delim
                count++;
            }
        }
    }
    if (out_array_length != NULL) *out_array_length = count;
}

void dstr_break_by_delim_start_end(dstr_t dstr, char delim_start, char delim_end, char **out_array, dstr_memory_allocator *allocator)
{
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

    if (out_array != NULL)
    {
        u64 start_len = (u64)(start - dstr);
        if (start_len > 0)
        {
            out_array[ 0 ] = dstr_create_from_chars(dstr, start_len, allocator);
        }
        else 
        {
            out_array[ 0 ] = NULL;
        }
        u64 mid_len = (u64)(end - start);
        if (start_len > 0)
        {
            out_array[ 1 ] = dstr_create_from_chars(start, mid_len, allocator);
        }
        else 
        {
            out_array[ 1 ] = NULL;
        }
        u64 end_len = (u64)(dstr + dstr_len - end);
        if (start_len > 0)
        {
            out_array[ 2 ] = dstr_create_from_chars(end, end_len, allocator);
        }
        else 
        {
            out_array[ 2 ] = NULL;
        }
    }
}

dstr_builder_t dstr_builder_create(u64 initial_capacity, dstr_memory_allocator* allocator)
{
    dstr_memory_allocator _allocator = allocator == NULL ? DSTRING_DEFAULT_ALLOCATOR : *allocator; 

    dstr_builder_t builder = _allocator.allocate(sizeof(_dstr_builder));

    builder->capacity = initial_capacity;
    builder->chars    = DSTRING_ALLOCATE(sizeof(char) * builder->capacity, allocator);
    builder->count    = 0;
    builder->allocator = _allocator;

    return builder;
}

dstr_t dstr_builder_generate_dstr(dstr_builder_t builder, dstr_memory_allocator* allocator)
{
    return dstr_create_from_chars(builder->chars, builder->count, allocator);
}

dstr_t dstr_builder_chop_prefix_by_delim(dstr_builder_t builder, const char delim, dstr_memory_allocator* allocator)
{
    u64 delim_index = 0;
    for (; delim_index < builder->count && builder->chars[delim_index] != delim; delim_index++);

    dstr_t prefix = dstr_create_from_chars(builder->chars, delim_index, allocator);

    dstr_builder_remove_prefix(builder, delim_index);

    return prefix;
}

dstr_t dstr_builder_chop_suffix_by_delim(dstr_builder_t builder, const char delim, dstr_memory_allocator* allocator)
{
    u64 delim_index = builder->count - 1;
    for (; delim_index != 0 && builder->chars[delim_index] != delim; delim_index--);

    dstr_t dstr = dstr_create_from_chars(builder->chars + delim_index + 1, builder->count - delim_index, allocator);

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
    u64 dstr_len = dstrlen(dstr);
    if (builder->capacity <= builder->count + dstr_len)
    {
        builder->capacity += builder->count + dstr_len;
        builder->chars = builder->allocator.reallocate(builder->chars, builder->capacity); 
    }

    memcpy(builder->chars + builder->count, dstr, dstr_len);
    builder->count += dstr_len;
}

void dstr_builder_append_cstr(dstr_builder_t builder, const char *cstring)
{
    u64 dstr_len = strlen(cstring);
    if (builder->capacity <= builder->count + dstr_len)
    {
        builder->capacity += builder->count + dstr_len;
        builder->chars = builder->allocator.reallocate(builder->chars, builder->capacity); 
    }
    
    memcpy(builder->chars + builder->count, cstring, dstr_len);
    builder->count += dstr_len;
}

void dstr_builder_append_cstr_format(dstr_builder_t builder, const char *format, ...)
{
    va_list args;
    va_list args_copy;
    va_start(args, format);
    va_start(args_copy, format);

    u64 str_len = vsnprintf(NULL, 0, format, args_copy);

    if (builder->capacity <= builder->count + str_len)
    {
        while (builder->capacity <= builder->count + str_len) builder->capacity *= 2;
        void* block = builder->allocator.reallocate(builder->chars, builder->capacity);
        assert(block);
        builder->chars = block;

    }
    
    vsnprintf(builder->chars + builder->count, str_len + 1, format, args);
    builder->count += str_len;

    va_end(args);
    va_end(args_copy);

}

void dstr_builder_append_chars(dstr_builder_t builder, const char chars[], u64 count)
{
    if (builder->capacity <= builder->count + count)
    {
        builder->capacity += builder->count + count;
        builder->chars = builder->allocator.reallocate(builder->chars, builder->capacity); 
    }
    
    memcpy(builder->chars + builder->count, chars, count);
    builder->count += count;
}

void dstr_builder_append_char(dstr_builder_t builder, const char c)
{
    if (builder->capacity <= builder->count + 1)
    {
        builder->capacity *= 2;
        builder->chars = builder->allocator.reallocate(builder->chars, builder->capacity); 
    }
    
    builder->chars[builder->count++] = c;
}

void dstr_builder_replace_at_dstr(dstr_builder_t builder, dstr_t replacement, u64 start_index)
{
    if (dstrlen(replacement) + start_index >= builder->count) return; // Silence error

    u64 replacement_len = dstrlen(replacement);
    memcpy(builder->chars + start_index, replacement, replacement_len);
    builder->count += replacement_len;
}

void dstr_builder_replace_at_cstr(dstr_builder_t builder, const char *replacement, u64 start_index)
{
    if (strlen(replacement) + start_index >= builder->count) return; // Silence error
    u64 replacement_len = strlen(replacement);
    memcpy(builder->chars + start_index, replacement, replacement_len);
    builder->count += replacement_len;
}

void dstr_builder_replace_at_chars(dstr_builder_t builder, const char replacement[], u64 count, u64 start_index)
{
    if (count + start_index >= builder->count) return; // Silence error
    memcpy(builder->chars + start_index, replacement, count);
    builder->count += count;
}

u64 dstr_builder_get_capacity(dstr_builder_t builder)
{
    return builder->capacity;
}

u64 dstr_builder_get_chars_count(dstr_builder_t builder)
{
    return builder->count;
}

void dstr_builder_reserve(dstr_builder_t builder, u64 amount)
{
    builder->capacity += amount;
    builder->chars = builder->allocator.reallocate(builder->chars, builder->capacity);
}

void dstr_builder_remove_at_char(dstr_builder_t builder, u64 index)
{
    if (index >= builder->count) return; // Silence error
    memmove(builder->chars + index, builder->chars + index + 1, builder->count-- - index);
}

void dstr_builder_remove_at_chars(dstr_builder_t builder, u64 index, u64 count)
{
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
    char * ptr = builder->chars;
    u64 iteration = 0;
    u64 deleted_count = 0;
    
    while(iteration < builder->count)
    {
        if(*ptr == c)
        {
            memmove(ptr, ptr + 1, builder->count - deleted_count - (int)(ptr - builder->chars));
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
    for (char * p = builder->chars; p < builder->chars + builder->count; p++)
    {
        *p = *p == to_replace ? replacement : *p;
    }
}

void dstr_builder_remove_prefix(dstr_builder_t builder, u64 prefix_len)
{
    if (prefix_len >= builder->count)
    {
        builder->count = 0;
        return;
    }
    memmove(builder->chars, builder->chars + prefix_len + 1, builder->count - prefix_len);
}

void dstr_builder_remove_suffix(dstr_builder_t builder, u64 suffix_len)
{
    if (suffix_len >= builder->count)
    {
        builder->count = 0;
        return;
    }
    builder->count -= suffix_len;
}

char *dstr_builder_get_chars(dstr_builder_t builder)
{
    return builder->chars;
}

#endif // defined(DSTRING_IMPLEMENTATION)
