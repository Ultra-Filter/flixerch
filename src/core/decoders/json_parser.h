#pragma once

#include "../utils/defines.h"
#include <stdio.h> // FILE*

/*

    TODO: Make the code prettier!! Right noow looks bad.
    TODO: json creator.
    TODO: json reader. (after parsing...)
    TODO: Add support for codepoints and wide chars.
    TODO: Use dstring ??

*/

typedef struct json_parser_mem_allocator {
    void* (*allocate)(u64);
    void* (*reallocate)(void*, u64);
    void  (*free)(void*);
} json_parser_mem_allocator;

typedef enum json_obj_type_e {
    JSON_OBJ_TYPE_ARRAY,
    JSON_OBJ_TYPE_MAP,
    JSON_OBJ_TYPE_STRING,
    JSON_OBJ_TYPE_INT,
    JSON_OBJ_TYPE_DOUBLE,
    JSON_OBJ_TYPE_BOOL,
    JSON_OBJ_TYPE_NULL,
    JSON_OBJ_TYPE_COUNT
} json_obj_type_e;

typedef struct json_obj_t {
    json_obj_type_e type;
    char* name; // cstring
    void* value;
    u32 value_size_in_bytes;
    struct json_obj_t * first_child, *right_sibling;
} json_obj_t;

typedef struct json_parser_t json_parser_t;

json_parser_t*  json_parser_create(const char* json_cstring, json_parser_mem_allocator* allocator);
json_obj_t      json_parser_parse_file(FILE* f, json_parser_mem_allocator* allocator); 
void            json_parser_destroy(json_parser_t* parser);
void            json_parser_fprint(FILE* stream, json_obj_t root, u8 indentation);
void            json_parser_destroy_obj(json_obj_t* obj, json_parser_mem_allocator* allocator);
bool            json_parser_parse(json_parser_t* parser, json_obj_t* out_parse_obj);

#if defined(JSON_PARSER_IMPLEMENTATION)

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#define DEFAULT_JSON_PARSER_MEM_ALLOCATOR (json_parser_mem_allocator){ .allocate = malloc, .reallocate = realloc, .free = free }
#define SKIP_SPACES(c) do { while (isspace(*(c)) && *(c)) { (c)++; } if ((curr)[0] != '\0' && (curr)[1] != '\0' && (curr)[0] == '/' && (curr)[1] == '/') JUMP_TO_NEXT_LINE(curr); while (isspace(*(c)) && *(c)) { (c)++; } } while(0)
#define SKIP_CHAR(curr, c) do { if (*(curr) != (c)) goto failure_ret; curr++; } while(0)
#define LEN_TO_CHAR(curr, name_len, c) do { while(*((curr) + (name_len)) != (c) && *((curr) + (name_len))) (name_len)++; } while(0)
#define JUMP_TO_NEXT_LINE(curr) do { if (*(curr) == '\n') { (curr)++; break; } if (*(curr) == '\0') break; (curr)++; } while(1)

typedef struct json_parser_t {
    json_parser_mem_allocator mem_alloc;
    char* json;
    u64 json_length;
} json_parser_t;

json_parser_t* json_parser_create(const char* json_string, json_parser_mem_allocator* allocator)
{
    json_parser_mem_allocator alloc = allocator != NULL ? *allocator : DEFAULT_JSON_PARSER_MEM_ALLOCATOR;
    json_parser_t * parser = (json_parser_t*)alloc.allocate(sizeof(json_parser_t));
    assert(parser != NULL);

    u64 json_len = strlen(json_string);
    parser->json = (char*)alloc.allocate(sizeof(char) * json_len);
    memcpy(parser->json, json_string, json_len * sizeof(char));
    parser->json_length = json_len;
    parser->mem_alloc = alloc;
 
    return parser;
}

void json_parser_destroy(json_parser_t* parser)
{
    if (parser != NULL)
    {
        if (parser->json != NULL)
        {
            parser->mem_alloc.free(parser->json);
        }
        parser->mem_alloc.free(parser);
    }
}


static bool _json_parse_(char** current, json_parser_mem_allocator allocator, json_obj_t* out)
{
    assert(out != NULL && current != NULL && *current != NULL);

    char* curr = *current;
    if (*curr == '\0') goto success_ret;

    SKIP_SPACES(curr);

    char start_val = *curr;
    if (start_val == '{')
    {
        out->type = JSON_OBJ_TYPE_MAP;
        out->value = (u32*)allocator.allocate(sizeof(u32));
        memset(out->value, 0, sizeof(u32));
        out->value_size_in_bytes = sizeof(u32);

        SKIP_CHAR(curr, '{'); 
        SKIP_SPACES(curr);
        
        if (*curr == '}') { curr++; goto success_ret; }
        
        SKIP_CHAR(curr, '\"');
        
        u32 name_len = 0;
        LEN_TO_CHAR(curr, name_len, '\"');
        
        char * name = (char*)allocator.allocate(sizeof(char) * (name_len + 1));
        memcpy(name, curr, name_len);
        name[name_len] = '\0'; 
        
        json_obj_t* child = (json_obj_t*)allocator.allocate(sizeof(json_obj_t));
        out->first_child = child;
        child->name = name;
        child->first_child = NULL;
        child->right_sibling = NULL;
        curr += ++name_len;
        
        SKIP_SPACES(curr);
        SKIP_CHAR(curr, ':');
        SKIP_SPACES(curr);

        bool success = _json_parse_(&curr, allocator, child);
        if (!success) goto failure_ret;
        (*((u32*)out->value))++; 

        while (true)
        {
            SKIP_SPACES(curr);
            if (!*curr) goto failure_ret;
            if (*curr == '}') { curr++; goto success_ret; }

            SKIP_CHAR(curr, ',');
            SKIP_SPACES(curr);

            SKIP_CHAR(curr, '\"');
            
            json_obj_t* nchild = (json_obj_t*)allocator.allocate(sizeof(json_obj_t));
            child->right_sibling = nchild;
            child = nchild;
            child->first_child = NULL;
            child->right_sibling = NULL;
            
            name_len = 0;
            LEN_TO_CHAR(curr, name_len, '\"');

            char * nname = (char*)allocator.allocate(sizeof(char) * (name_len + 1));
            memcpy(nname, curr, name_len);
            nname[name_len] = '\0'; // ignore '\"' and insert '\0'
            
            child->name = nname;
            curr += ++name_len;
            
            SKIP_SPACES(curr);
            SKIP_CHAR(curr, ':');
            SKIP_SPACES(curr);
            
            success = _json_parse_(&curr, allocator, child);
            if (!success) goto failure_ret;
            (*((u32*)out->value))++;
        }

        goto failure_ret; // never reached
    }
    else if (start_val == '\"')
    {
        out->type = JSON_OBJ_TYPE_STRING;

        SKIP_CHAR(curr, '\"');

        u32 len = 0;
        LEN_TO_CHAR(curr, len, '\"');

        char * str = (char*)allocator.allocate(sizeof(char) * (len + 1));
        memcpy(str, curr, len);
        str[len] = '\0'; // ignore '\"' and insert '\0'
        out->value = str;
        out->value_size_in_bytes = sizeof(char) * (++len);
        curr += len;
        goto success_ret;
    }
    else if (start_val == '[')
    {
        // array case
        out->type = JSON_OBJ_TYPE_ARRAY;
        SKIP_CHAR(curr, '[');
        out->value = (u32*)allocator.allocate(sizeof(u32));
        memset(out->value, 0, sizeof(u32));
        out->value_size_in_bytes = sizeof(u32);

        SKIP_SPACES(curr);
        if (*curr == ']') { curr++; goto success_ret; }
        
        json_obj_t * fchild = (json_obj_t*)allocator.allocate(sizeof(json_obj_t));
        fchild->first_child = NULL;
        fchild->right_sibling = NULL;
        fchild->name = NULL;
        bool succ = _json_parse_(&curr, allocator, fchild);
        if (!succ) goto failure_ret;
        (*((u32*)out->value))++;
        out->first_child = fchild;
        
        while (true)
        {
            SKIP_SPACES(curr);
            if (*curr == ']') { curr++; goto success_ret; }
            SKIP_CHAR(curr, ',');
            SKIP_SPACES(curr);
            json_obj_t * child = (json_obj_t*)allocator.allocate(sizeof(json_obj_t));
            fchild->right_sibling = child;
            fchild = child;
            fchild->name = NULL;
            fchild->first_child = NULL;
            fchild->right_sibling = NULL;
            bool succ = _json_parse_(&curr, allocator, fchild);
            if (!succ) goto failure_ret;
            (*((u32*)out->value))++;

        }
    }
    else if (isdigit(start_val) || start_val == '-')
    {
        char * ptr;
        int res = (int)strtol(curr, &ptr, 10);
        if (*ptr == '.')
        {
            out->type = JSON_OBJ_TYPE_DOUBLE;
            double d = strtod(curr, &ptr);
            out->value = allocator.allocate(sizeof(double));
            memcpy(out->value, &d, sizeof(double));
            curr = ptr;
            goto success_ret;
        }
        else
        {
            out->type = JSON_OBJ_TYPE_INT;
            out->value = allocator.allocate(sizeof(int));
            memcpy(out->value, &res, sizeof(int));
            curr = ptr;
            goto success_ret;
        }

    }
    else if (curr[0] != '\0' && curr[1] != '\0' && curr[2] != '\0' && curr[3] != '\0' 
        && curr[0] == 't' && curr[1] == 'r'
        && curr[2] == 'u' && curr[3] == 'e')
    {
        out->type = JSON_OBJ_TYPE_BOOL;

        out->value = allocator.allocate(sizeof(bool));
        bool _true_ = true;
        memcpy(out->value, &_true_, sizeof(bool));
        out->value_size_in_bytes = sizeof(bool);
        curr += 4;

        goto success_ret;
    }
    else if (curr[0] != '\0' && curr[1] != '\0' && curr[2] != '\0' && curr[3] != '\0' && curr[4] != '\0' && 
        curr[0] == 'f' && curr[1] == 'a'
        && curr[2] == 'l' && curr[3] == 's' && curr[4] == 'e')
    {
        out->type = JSON_OBJ_TYPE_BOOL;

        out->value = allocator.allocate(sizeof(bool));
        bool _false_ = false;
        memcpy(out->value, &_false_, sizeof(bool));
        out->value_size_in_bytes = sizeof(bool);
        curr += 5;

        goto success_ret;
    }
    else if (curr[0] != '\0' && curr[1] != '\0' && curr[2] != '\0' && curr[3] != '\0'
        && curr[0] == 'n' && curr[1] == 'u'
        && curr[2] == 'l' && curr[3] == 'l') {
        out->type = JSON_OBJ_TYPE_NULL;
        out->value = NULL;
        out->value_size_in_bytes = 0;
        curr += 4;
        goto success_ret; 
    } 
    else 
    {
        goto failure_ret;
    }

success_ret:
    *current = curr;
    return true;
failure_ret:
    *current = curr;
    return false;   
}

bool json_parser_parse(json_parser_t* parser, json_obj_t* out_parse_obj)
{
    assert(parser != NULL && out_parse_obj != NULL);

    out_parse_obj->name = (char*)parser->mem_alloc.allocate(sizeof(char) * (strlen("ROOT") + 1));
    memcpy(out_parse_obj->name, "ROOT", strlen("ROOT"));
    out_parse_obj->name[strlen("ROOT")] = '\0';
    out_parse_obj->type                 = JSON_OBJ_TYPE_MAP;
    out_parse_obj->first_child          = NULL;
    out_parse_obj->right_sibling        = NULL;
    char * ptr                          = parser->json;
    
    return _json_parse_(&ptr , parser->mem_alloc, out_parse_obj);
}

void json_parser_fprint(FILE* stream, json_obj_t root, u8 spaces)
{
    if (!stream) return;
    bool print_types = false;
    
    switch (root.type)
    {
        case JSON_OBJ_TYPE_MAP:
        {
            fprintf(stream, "%*s", spaces, ""); 
            if (root.name == NULL) { fprintf(stream, "{\n"); }
            else if (strcmp(root.name, "ROOT") == 0) fprintf(stream, "{\n"); else fprintf(stream, "\"%s\": {\n", root.name);
            json_obj_t * fc = root.first_child;
            while (fc != NULL)
            {
                json_parser_fprint(stream, *fc, spaces + 2);
                fc = fc->right_sibling;
                if (fc) fprintf(stream, ",\n");
            }
            fprintf(stream, "\n");
            fprintf(stream, "%*s", spaces, ""); 
            fprintf(stream, "}"); 
            if (print_types) fprintf(stream, " // Json type is MAP");
            
        } break;
        case JSON_OBJ_TYPE_STRING:
        {
            fprintf(stream, "%*s", spaces, ""); 
            if (root.name == NULL) { fprintf(stream, "\"%s\"", (char*)root.value); }
            else fprintf(stream, "\"%s\": \"%s\"", root.name, (char*)root.value);
            if (print_types) fprintf(stream, " // Json type is STRING");
        } break;
        case JSON_OBJ_TYPE_BOOL:
        {
            fprintf(stream, "%*s", spaces, ""); 
            if (root.name == NULL) fprintf(stream, "%s", (*(bool*)root.value) ? "true" : "false");
            else fprintf(stream, "\"%s\": %s", root.name, (*(bool*)root.value) ? "true" : "false");
            if (print_types) fprintf(stream, " // Json type is BOOL");
        } break;
        case JSON_OBJ_TYPE_ARRAY:
        {
            fprintf(stream, "%*s", spaces, ""); 
            if (root.name == NULL) fprintf(stream, "[\n");
            else fprintf(stream, "%s: [\n", root.name);
            json_obj_t* obj = root.first_child;
            while (obj)
            {
                json_parser_fprint(stream, *obj, spaces + 2);
                if (obj->right_sibling)
                {
                    fprintf(stream, ",\n");
                    obj = obj->right_sibling;
                } 
                else break;
            }
            fprintf(stream, "\n%*s", spaces, ""); 
            fprintf(stream, "]"); 
            if (print_types) fprintf(stream, " // Json type is ARRAY");
        } break;
        case JSON_OBJ_TYPE_DOUBLE:
        {
            fprintf(stream, "%*s", spaces, "");
            if (root.name == NULL) fprintf(stream, "%f", (*(double*)root.value)); 
            else fprintf(stream, "\"%s\": %f", root.name, (*(double*)root.value));
            if (print_types) fprintf(stream, " // Json type is DOUBLE");
        } break;
        case JSON_OBJ_TYPE_INT:
        {
            fprintf(stream, "%*s", spaces, ""); 
            if (root.name == NULL) fprintf(stream, "%d", (*(int*)root.value));
            else fprintf(stream, "\"%s\": %d", root.name, (*(int*)root.value));
            if (print_types) fprintf(stream, " // Json type is INT");
        } break;
        case JSON_OBJ_TYPE_NULL:
        {
            fprintf(stream, "%*s", spaces, ""); 
            if (root.name == NULL) fprintf(stream, "null");
            else fprintf(stream, "\"%s\": null", root.name);
            if (print_types) fprintf(stream, " // Json type is NULL");
        } break;
        default:
            assert("UNRECOGNIZED JSON TYPE!" && 0);
    }
}

void json_parser_destroy_obj(json_obj_t* obj, json_parser_mem_allocator* mem_allocator)
{
    if (obj == NULL) return;
    
    json_parser_mem_allocator allocator = mem_allocator == NULL ? DEFAULT_JSON_PARSER_MEM_ALLOCATOR : *mem_allocator;

    if (obj->first_child != NULL)
    {
        json_parser_destroy_obj(obj->first_child, mem_allocator);
    }
    if (obj->right_sibling != NULL)
    {
        json_parser_destroy_obj(obj->right_sibling, mem_allocator);
    } 

    allocator.free(obj);
}

json_obj_t json_parser_parse_file(FILE *f, json_parser_mem_allocator *_allocator) // caller must call json_parser_destroy_obj() ..
{
    json_obj_t res = { 0 };

    if (!f) return res;
    json_parser_mem_allocator allocator = _allocator == NULL ? DEFAULT_JSON_PARSER_MEM_ALLOCATOR : * _allocator;

    char * buff;
    long sizeof_buff;

    fseek(f, 0, SEEK_END);
    sizeof_buff = ftell(f) + 1;
    buff = allocator.allocate((u64)sizeof_buff);
    if (!buff) return res;

    fseek(f, 0, SEEK_SET);
    u64 amount = fread(buff, sizeof(char), sizeof_buff, f);
    buff[ amount ] = '\0';

    json_parser_t * parser = json_parser_create(buff, _allocator);
    if (!parser)
    {
        allocator.free(buff);
        return res;
    }

    json_parser_parse(parser, &res);
    json_parser_destroy(parser);

    allocator.free(buff);

    return res;
}

#endif
