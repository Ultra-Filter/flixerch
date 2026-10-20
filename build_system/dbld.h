#if !defined(DBUILD_STANDALONE)

#pragma once

#endif

/*
    ========== For anyone want to use this header File ===========

    NOTE: You can compile this header with the definition `DBUILD_STANDALONE` in order to treat it as a tranlation unit. (Remember to use '-x' flag when compiling)
    NOTE: Unfortunately we depend on few stuff so you need to have the files:
        1. "../data_structures/dstring.h"
        2. "../data_structures/dhash_table.h"
        3. "../parsers/json_parser.h"


    ================== Reminder to myself ========================

    TODO: Add threads support for now it doesnt use them.
    TODO: remove the include dependecies it kind of destroying the fun of single-header-lib.
    TODO: gourd the amount of proccess you create when compiling you can also reuse them as a pool.

    
    
    ===============================================================
*/


int dbuild(const int argc, char* argv[]);

#if defined(DBUILD_STANDALONE)

#define DSTRING_IMPLEMENTATION
#define JSON_PARSER_IMPLEMENTATION
#define DHASH_TABLE_IMPLEMENTATION
#define DBUILD_IMPLEMENTATION

int main(int argc, char* argv[])
{
    return dbuild(argc, argv);
}

#endif


#if defined(DBUILD_IMPLEMENTATION)
// ===========================================================
//                      Implementation
// ===========================================================

// ====================================
// Dependecies
// ====================================
#include "../src/core/utils/defines.h"
#include "../src/core/utils/containers/dstring.h"
#include "../src/core/utils/containers/dhash_table.h"
#include "../src/core/decoders/json_parser.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

// ====================================

// ====================================
// Definitions
// ====================================

#define KB(x) ((u64)(x) << 10ULL)
#define MB(x) ((KB(x))  << 10ULL)
#define GB(x) ((MB(x))  << 10ULL)

#if !defined(DBUILD_MEMORY_SIZE)
#   define DBUILD_MEMORY_SIZE MB(1) 
#endif

#if !defined(DBUILD_MAX_COMPILER_EXTRA_ARGS)
#   define DBUILD_MAX_COMPILER_EXTRA_ARGS 32
#endif

#if !defined(DBUILD_MAX_LINKER_EXTRA_ARGS)
#   define DBUILD_MAX_LINKER_EXTRA_ARGS 32
#endif

#if !defined(DBUILD_MAX_FILES_COUNT)
#   define DBUILD_MAX_FILES_COUNT KB(1)
#endif

#if !defined(DBUILD_TEMPLATE_DEFAULT)
#   define DBUILD_TEMPLATE_DEFAULT DBUILD_TEMPLATE_VSCODE 
#endif

#if !defined(DBUILD_BUILD_TARGET_DEFAULT)
#   define DBUILD_BUILD_TARGET_DEFAULT DBUILD_TARGET_DEBUG
#endif

#if !defined(DBUILD_BUILD_DIRECTORY_DEFAULT)
#   define DBUILD_BUILD_DIRECTORY_DEFAULT "build" 
#endif

#if !defined(DBUILD_SOURCE_DIRECTORY_DEFAULT)
#   define DBUILD_SOURCE_DIRECTORY_DEFAULT "src" 
#endif

#if !defined(DBUILD_DEBUG_COMPILATION_FLAG)
#   define DBUILD_DEBUG_COMPILATION_FLAG "-g" 
#endif

#if !defined(DBUILD_RELEASE_COMPILATION_FLAG)
#   define DBUILD_RELEASE_COMPILATION_FLAG "-Ofast" 
#endif

#if !defined(DBUILD_SHARED_COMPILATION_FLAG)
#   define DBUILD_SHARED_COMPILATION_FLAG "-shared" 
#endif

#if !defined(DBUILD_MAX_FORCED_FILES)
#define DBUILD_MAX_FORCED_FILES 32
#endif

#if !defined(DBUILD_MAX_INCLUDE_DIRECTORIES)
#define DBUILD_MAX_INCLUDE_DIRECTORIES 32
#endif

#if !defined(DBUILD_MAX_LIBS_INCLUDE_DIRECTORIES)
#define DBUILD_MAX_LIBS_INCLUDE_DIRECTORIES 32
#endif

#if !defined(DBUILD_MAX_LIBS)
#define DBUILD_MAX_LIBS 32
#endif

#if !defined(DBUILD_MAX_DEPENDENCIES_COUNT)
#define DBUILD_MAX_DEPENDENCIES_COUNT 64
#endif

#define ASSURE_NOT_LAST(i, argc, msg) if (i + 1 >= argc) { printf("ERROR: Expected an argument and got none.\n"); printf(msg); exit(EXIT_FAILURE); }

#define STREQ(s1, s2) (strcmp((s1), (s2)) == 0)

#define STR_START_WITH(s, c) ((s)[0] == c)

#define BYTE_STR "%c%c%c%c%c%c%c%c"

#define BYTE_FMT(b) \
    (((u8)(b)) & (1U << 7)) ? '1' : '0', \
    (((u8)(b)) & (1U << 6)) ? '1' : '0', \
    (((u8)(b)) & (1U << 5)) ? '1' : '0', \
    (((u8)(b)) & (1U << 4)) ? '1' : '0', \
    (((u8)(b)) & (1U << 3)) ? '1' : '0', \
    (((u8)(b)) & (1U << 2)) ? '1' : '0', \
    (((u8)(b)) & (1U << 1)) ? '1' : '0', \
    (((u8)(b)) & (1U << 0)) ? '1' : '0'

#define BOOL_PRINT_FMT(b) (b) ? "true" : "false"

// ====================================

// ====================================
// Declerations
// ====================================

typedef enum dbuild_target : u8    
{ 
    DBUILD_TARGET_DEBUG,
    DBUILD_TARGET_RELEASE, 
    DBUILD_TARGET_SHARED, 
    DBUILD_TARGET_COUNT 
} dbuild_target;
typedef enum dbuild_template : u8  
{ 
    DBUILD_TEMPLATE_VSCODE,
    DBUILD_TEMPLATE_CLANGD, 
    DBUILD_TEMPLATE_COUNT 
} dbuild_template;
typedef enum dbuild_file_type : u8 
{ 
    DBUILD_FILE_TYPE_C_SOURCE_FILE,
    DBUILD_FILE_TYPE_C_HEADER_FILE, 
    DBUILD_FILE_TYPE_UNKNOWN, 
    DBUILD_FILE_TYPE_COUNT 
} dbuild_file_type;

typedef struct file_data {
    dbuild_file_type type;
    dstr_t file_path;
    SYSTEMTIME last_write_time;
    dstr_t dependecies [ DBUILD_MAX_DEPENDENCIES_COUNT ];
    u32 dependencies_count;
    bool should_compile;
    bool is_changed;
} file_data;

static bool   str_eq_with_except( const char * s1, const char * s2, char e1, char e2);
static void   str_replace( char * str, char to_replace, char with );
static bool   str_is_suffix( const char * str, const char * suff );

static void   create_obj_file_name_from_path( char * path, char * out_file_name );
static void   create_flags_strings( void );
static void   set_exec_name_from_path( char * exec_path );
static dstr_t dstr_create_from_path( char * path);

static void*  allocate( u64 size );
static void*  reallocate( void * block, u64 size );
static void   deallocate( void * block );

static bool   load_conigurations( void );
static void   save_configurations( void );

static void   create_build_directory( void );
static void   create_template( void );

static void   scan_source_directory( const char * directory );
static void   filter_scanned_files_to_compile( void );

static bool   load_existings_object_files( void );
static void   save_new_object_files( int argc, char** argv );

static bool   compile_source_file( file_data * file, PROCESS_INFORMATION* pi);
static bool   link_all_object_files( void );

static bool   parse_args( const int argc , char * argv[] );
static void   print_help( void );
static bool   build( void );
static int    run_command( char * cmd );
static void   initialize_context( char** argv );
static void   clean_build_cache( void );

static bool   is_file_forced( dstr_t file_path );
static bool   CWD_is_empty( void );

// ====================================

// ====================================
// Globals
// ====================================

static const dstr_memory_allocator allocator = { 
    .allocate = allocate, 
    .reallocate = reallocate, 
    .free = deallocate 
};

static u8  DBUILDER_MEMORY [ DBUILD_MEMORY_SIZE ] = { 0 };
static u64 dbuilder_mem_off = 0;

static struct dbuilder_context 
{
    char build_folder_name   [ MAX_PATH ];
    char src_folder_name     [ MAX_PATH ];
    char output_debug_name   [ MAX_PATH ];
    char output_release_name [ MAX_PATH ];
    char output_shared_name  [ MAX_PATH ];

    bool should_clean;
    bool should_print_info;
    bool should_save_config;
    bool should_use_config;

    dstr_t compiler_extra_args [ DBUILD_MAX_COMPILER_EXTRA_ARGS ];
    dstr_t linker_extra_args   [ DBUILD_MAX_LINKER_EXTRA_ARGS ];
    u8 compiler_extra_args_count;
    u8 linker_extra_args_count;

    dstr_t include_dirs     [ DBUILD_MAX_INCLUDE_DIRECTORIES ];
    dstr_t include_libs_dir [ DBUILD_MAX_LIBS_INCLUDE_DIRECTORIES ];
    dstr_t include_libs     [ DBUILD_MAX_LIBS ];
    u8 include_dirs_count;
    u8 include_libs_count;
    u8 include_libs_dir_count;

    dbuild_target build_target;
    dbuild_template template_target;

    dstr_t pre_cmd;
    dstr_t post_cmd;
    bool has_pre_cmd;
    bool has_post_cmd;

    dhash_table_t* files_table;
    dhash_table_t* forced_files_table;

    dstr_t include_all_dirs_flags;
    dstr_t compiler_all_flags;
    dstr_t linker_all_flags;
    dstr_t include_libs_all_flags;
    dstr_t include_libs_dirs_all_flags;

    char executable_name [ MAX_PATH ];
    bool executable_created;
} CTX;

// ====================================

static bool build( void )
{
    PROCESS_INFORMATION pis [ DBUILD_MAX_FILES_COUNT ] = { 0 };
    HANDLE hdls             [ DBUILD_MAX_FILES_COUNT ] = { 0 };
    u32 idx = 0;
    bool success = true;

    load_existings_object_files( );
    create_flags_strings();

    dhash_table_iter_t* it = dhash_table_iter_create(CTX.files_table);

    file_data fd = { };

    while (dhash_table_iter_next(it, &fd))
    {
        if (fd.should_compile && fd.type == DBUILD_FILE_TYPE_C_SOURCE_FILE)
        {
            if (CTX.should_print_info) fprintf(stdout, "Compiling source file: %s\n", fd.file_path);
            if (!compile_source_file(&fd, &pis[ idx ])) 
            {
                success = false;
                break;
            }
            hdls[ idx ] = pis[ idx ].hProcess;
            idx++;
        }
    }
    
    if (success)
    {
        DWORD res = WaitForMultipleObjects(
            idx, hdls, TRUE, INFINITE 
        );

        if (res != WAIT_OBJECT_0)
        {
            if (CTX.should_print_info) fprintf(stderr, "Something went wrong compiling your sources. Aborting..");
            return false;
        }

        if (CTX.should_print_info) fprintf(stdout, "Comilation stage ended successfully.\n");

        if (!link_all_object_files())
        {
            CTX.executable_created = false;
            return false;
        }
        else
        {
            if (CTX.should_print_info) fprintf(stdout, "Linking stage ended successfully.\n");
            CTX.executable_created = true;
        }
    }
    else
    {
        fprintf(
            stderr,
            "Something went wrong compiling your source files.\n"
        );
        return false;
    }

    
    return true;
}

int dbuild( const int argc, char *argv[] )
{
    if (argc > 1 && STREQ(argv[ 1 ], "###object-files###")) // Used internal like forking without copy of PCB - Do Not Pass this flag manually it can result in seg fault
    {
        save_new_object_files(argc, argv);
        return EXIT_SUCCESS;
    }

    initialize_context( argv );

    if (argc <= 1) {

        if (CWD_is_empty())
        {
            create_template(); 
            return EXIT_SUCCESS; 
        }

        CTX.should_use_config = true;

        if (!load_conigurations())
        {
            fprintf(stderr, "You asked me to load & use config that you saved but you have not saved any configs for the debug target before.");
            return EXIT_FAILURE;
        }

        if (CTX.should_print_info) fprintf(stdout, "Loaded configurations successfully.\n"); 
        
        create_build_directory();

        scan_source_directory(CTX.src_folder_name);
        
        if (CTX.has_pre_cmd) run_command(CTX.pre_cmd);

        if (!build()) return EXIT_FAILURE;

        if (CTX.has_post_cmd) run_command(CTX.post_cmd);

        return EXIT_SUCCESS;
    }
    
    if (!parse_args(argc, argv)) exit(EXIT_FAILURE);
    
    if (CTX.should_save_config)
    {
        create_build_directory();
        save_configurations();
    }
    else if (CTX.should_use_config)
    {
        if (!load_conigurations())
        {
            fprintf(stderr, "You asked me to load & use config that you saved but you have not saved any configs for the debug target before.");
            return EXIT_FAILURE;
        }
        if (CTX.should_print_info) fprintf(stdout, "Loaded configurations successfully.\n"); 
        create_build_directory();
    }
    

    scan_source_directory(CTX.src_folder_name);

    if (CTX.has_pre_cmd) run_command(CTX.pre_cmd);

    if (!build()) return EXIT_FAILURE;

    if (CTX.has_post_cmd) run_command(CTX.post_cmd);

    dstr_t cmd = dstr_create_from_cstr_format(
        &allocator,
        "%s ###object-files### %s %s/%s/object_files.json %s",
        argv[ 0 ],
        CTX.src_folder_name,
        CTX.build_folder_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? "debug" :
        CTX.build_target == DBUILD_TARGET_RELEASE ? "release" : "shared",
        CTX.include_all_dirs_flags ? CTX.include_all_dirs_flags : ""
    );

    run_command(cmd);

    return EXIT_SUCCESS;
}

static inline void initialize_context( char** argv )
{
    set_exec_name_from_path(argv[ 0 ]);

    CTX.build_target    = DBUILD_BUILD_TARGET_DEFAULT;
    CTX.template_target = DBUILD_TEMPLATE_DEFAULT;

    memset(CTX.build_folder_name  , 0, MAX_PATH);
    memset(CTX.src_folder_name    , 0, MAX_PATH);
    memset(CTX.output_debug_name  , 0, MAX_PATH);
    memset(CTX.output_release_name, 0, MAX_PATH);
    memset(CTX.output_shared_name , 0, MAX_PATH);
    memcpy(CTX.build_folder_name  , DBUILD_BUILD_DIRECTORY_DEFAULT , strlen(DBUILD_BUILD_DIRECTORY_DEFAULT ));
    memcpy(CTX.src_folder_name    , DBUILD_SOURCE_DIRECTORY_DEFAULT, strlen(DBUILD_SOURCE_DIRECTORY_DEFAULT));
    memcpy(CTX.output_debug_name  , "main.exe",   strlen("main.exe"));
    memcpy(CTX.output_release_name, "main.exe",   strlen("main.exe"));
    memcpy(CTX.output_shared_name , "main.dll",   strlen("main.dll"));

    CTX.files_table = dhash_table_create(
        DBUILD_MAX_FILES_COUNT, 
        sizeof(file_data), 
        HASH_TYPE_FNV1A,
        DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST,
        (dhash_table_mem_allocator*)&allocator
    );

    CTX.forced_files_table = dhash_table_create(
        DBUILD_MAX_FORCED_FILES, 
        sizeof(file_data), 
        HASH_TYPE_FNV1A,
        DHASH_COLLISION_RESOLUTION_TYPE_LINKED_LIST,
        (dhash_table_mem_allocator*)&allocator
    );
}

static inline void clean_build_cache( void )
{
    char path [ MAX_PATH * 2 ] = { 0 };
    
    sprintf(
        path,
        "%s/%s/object_files",
        CTX.build_folder_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? "debug" :
        CTX.build_target == DBUILD_TARGET_RELEASE ? "release" : "shared"
    );

    SHFILEOPSTRUCTA file_op = {
        NULL,
        FO_DELETE,
        path,
        "",
        FOF_NOCONFIRMATION |
        FOF_NOERRORUI |
        FOF_SILENT,
        false,
        0,
        "" 
    };

    SHFileOperationA(&file_op); 
    
    memset(path, 0, MAX_PATH + 20);
    
    sprintf(
        path,
        "%s/%s/files.json",
        CTX.build_folder_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? "debug" :
        CTX.build_target == DBUILD_TARGET_RELEASE ? "release" : "shared"
    );
    
    file_op = (SHFILEOPSTRUCTA){
        NULL,
        FO_DELETE,
        path,
        "",
        FOF_NOCONFIRMATION |
        FOF_NOERRORUI |
        FOF_SILENT,
        false,
        0,
        "" 
    };

    SHFileOperationA(&file_op); 
}

static bool load_conigurations()
{
    FILE * config_file = NULL;

    char config_path [ MAX_PATH * 2 ] = { 0 };

    sprintf(
        config_path,
        "%s%s",
        CTX.build_folder_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? "/debug/config.json" :
        CTX.build_target == DBUILD_TARGET_RELEASE ? "/release/config.json" : "/shared/config.json"
    );

    config_file = fopen(config_path, "r");
    
    if (!config_file) return false;

    json_obj_t root = json_parser_parse_file(config_file, (json_parser_mem_allocator*)&allocator);

    if (root.type != JSON_OBJ_TYPE_MAP || !root.name)
    {
        fprintf(stderr, "Error parsing config file at: %s\n", config_path);
        return false;
    }

    json_obj_t* curr = root.first_child;

    while (curr && curr->name)
    {
        if (STREQ(curr->name, "build_folder_name"))
        {
            memcpy(CTX.build_folder_name, curr->value, curr->value_size_in_bytes);
            CTX.build_folder_name[ curr->value_size_in_bytes ] = '\0';
        }
        else if (STREQ(curr->name, "src_folder_name"))
        {
            memcpy(CTX.src_folder_name, curr->value, curr->value_size_in_bytes);
            CTX.src_folder_name[ curr->value_size_in_bytes ] = '\0';
        }
        else if (STREQ(curr->name, "output_debug_name"))
        {
            memcpy(CTX.output_debug_name, curr->value, curr->value_size_in_bytes);
            CTX.output_debug_name[ curr->value_size_in_bytes ] = '\0';
        }
        else if (STREQ(curr->name, "output_release_name"))
        {
            memcpy(CTX.output_release_name, curr->value, curr->value_size_in_bytes);
            CTX.output_release_name[ curr->value_size_in_bytes ] = '\0';
        }
        else if (STREQ(curr->name, "output_shared_name"))
        {
            memcpy(CTX.output_shared_name, curr->value, curr->value_size_in_bytes);
            CTX.output_shared_name[ curr->value_size_in_bytes ] = '\0';
        }
        else if (STREQ(curr->name, "build_target"))
        {
            char* val = (char*)curr->value;
            CTX.build_target = STREQ(val, "DEBUG") ? DBUILD_TARGET_DEBUG : 
                STREQ(val, "RELEASE") ? DBUILD_TARGET_RELEASE  : DBUILD_TARGET_SHARED;
        }
        else if (STREQ(curr->name, "template_type"))
        {
            char* val = (char*)curr->value;
            CTX.template_target = STREQ(val, "vscode") ? DBUILD_TEMPLATE_VSCODE : 
                STREQ(val, "clangd") ? DBUILD_TEMPLATE_CLANGD  : DBUILD_TEMPLATE_DEFAULT;
        }
        else if (STREQ(curr->name, "tasks"))
        {
            json_obj_t * c = curr->first_child;
            while (c)
            {
                if (STREQ(c->name, "pre"))
                {
                    json_obj_t * c1 = c->first_child;
                    while (c1)
                    {
                        if (STREQ(c1->name, "exist"))
                            CTX.has_pre_cmd = *(bool*)c1->value; 
                        c1 = c1->right_sibling;
                    }
                    if (CTX.has_pre_cmd)
                    {
                        json_obj_t * c2 = c->first_child;
                        while (c2)
                        {
                            if (STREQ(c2->name, "cmd"))
                                CTX.pre_cmd = dstr_create_from_chars((const char*)c2->value, c2->value_size_in_bytes, &allocator); 
                            c2 = c2->right_sibling;
                        }
                    }
                }
                else if (STREQ(c->name, "post"))
                {
                    json_obj_t * c1 = c->first_child;
                    while (c1)
                    {
                        if (STREQ(c1->name, "exist"))
                            CTX.has_post_cmd = *(bool*)c1->value; 
                        c1 = c1->right_sibling;
                    }
                    if (CTX.has_post_cmd)
                    {
                        json_obj_t * c2 = c->first_child;
                        while (c2)
                        {
                            if (STREQ(c2->name, "cmd"))
                                CTX.post_cmd = dstr_create_from_chars((const char*)c2->value, c2->value_size_in_bytes, &allocator); 
                            c2 = c2->right_sibling;
                        }
                    }
                }
                c = c->right_sibling;
            }
        }
        else if (STREQ(curr->name, "compiler_extra_args"))
        {
            CTX.compiler_extra_args_count = 0;

            json_obj_t * c = curr->first_child;
            while (c)
            {
                CTX.compiler_extra_args[ CTX.compiler_extra_args_count++ ] = dstr_create_from_chars((const char*)c->value, c->value_size_in_bytes, &allocator);
                if (CTX.compiler_extra_args_count >= DBUILD_MAX_COMPILER_EXTRA_ARGS)
                {
                    break;
                }
                c = c->right_sibling;
            } 
        }
        else if (STREQ(curr->name, "linker_extra_args"))
        {
            CTX.linker_extra_args_count = 0;

            json_obj_t * c = curr->first_child;
            while (c)
            {
                CTX.linker_extra_args[ CTX.linker_extra_args_count++ ] = dstr_create_from_chars((const char*)c->value, c->value_size_in_bytes, &allocator);
                if (CTX.linker_extra_args_count >= DBUILD_MAX_LINKER_EXTRA_ARGS)
                {
                    break;
                }
                c = c->right_sibling;
            } 
        }
        else if (STREQ(curr->name, "include_dirs"))
        {
            CTX.include_dirs_count = 0;

            json_obj_t * c = curr->first_child;
            while (c)
            {
                CTX.include_dirs[ CTX.include_dirs_count++ ] = dstr_create_from_chars((const char*)c->value, c->value_size_in_bytes, &allocator);
                if (CTX.include_dirs_count >= DBUILD_MAX_INCLUDE_DIRECTORIES)
                {
                    break;
                }
                c = c->right_sibling;
            } 
        }
        else if (STREQ(curr->name, "include_libs"))
        {
            CTX.include_libs_count = 0;

            json_obj_t * c = curr->first_child;
            while (c)
            {
                CTX.include_libs[ CTX.include_libs_count++ ] = dstr_create_from_chars((const char*)c->value, c->value_size_in_bytes, &allocator);
                if (CTX.include_libs_count >= DBUILD_MAX_LIBS)
                {
                    break;
                }
                c = c->right_sibling;
            } 
        }
        else if (STREQ(curr->name, "include_libs_dir"))
        {
            CTX.include_libs_dir_count = 0;

            json_obj_t * c = curr->first_child;
            while (c)
            {
                CTX.include_libs_dir[ CTX.include_libs_dir_count++ ] = dstr_create_from_chars((const char*)c->value, c->value_size_in_bytes, &allocator);
                if (CTX.include_libs_dir_count >= DBUILD_MAX_LIBS_INCLUDE_DIRECTORIES)
                {
                    break;
                }
                c = c->right_sibling;
            } 
        }

        curr = curr->right_sibling;
    } 

    return true;
}

static void save_configurations( void )
{
    FILE * config_file = NULL;

    char config_path [ MAX_PATH * 2 ] = { 0 };

    sprintf(
        config_path,
        "%s%s",
        CTX.build_folder_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? "/debug/config.json" :
        CTX.build_target == DBUILD_TARGET_RELEASE ? "/release/config.json" : "/shared/config.json"
    );

    config_file = fopen(config_path, "w");

    fprintf(
        config_file,
        "// %s config file. DO NOT EDIT THIS FILE DIRECTLY.\n"
        "{\n"
        "   \"build_folder_name\": \"%s\",\n"
        "   \"src_folder_name\": \"%s\",\n"
        "   \"output_debug_name\": \"%s\",\n"
        "   \"output_release_name\": \"%s\",\n"
        "   \"output_shared_name\": \"%s\",\n"        
        "   \"tasks\": {\n"        
        "   \t\"pre\": { \"exist\": %s, \"cmd\": \"%s\" },\n"        
        "   \t\"post\": { \"exist\": %s, \"cmd\": \"%s\" }\n"        
        "   },\n"        
        "   \"build_target\": \"%s\",\n"        
        "   \"template_type\": \"%s\",\n",        
        CTX.build_target == DBUILD_TARGET_DEBUG ? "Debug" : CTX.build_target == DBUILD_TARGET_RELEASE ? "Release" : "Shared", 
        CTX.build_folder_name,
        CTX.src_folder_name,
        CTX.output_debug_name,
        CTX.output_release_name,
        CTX.output_shared_name,
        BOOL_PRINT_FMT(CTX.has_pre_cmd), CTX.has_pre_cmd ? CTX.pre_cmd : "",
        BOOL_PRINT_FMT(CTX.has_post_cmd), CTX.has_post_cmd ? CTX.post_cmd : "",
        CTX.build_target == DBUILD_TARGET_DEBUG ? "DEBUG" : CTX.build_target == DBUILD_TARGET_RELEASE ? "RELEASE" : "SHARED", 
        CTX.template_target == DBUILD_TEMPLATE_VSCODE ? "vscode" : CTX.template_target == DBUILD_TEMPLATE_CLANGD ? "clangd" : "none" 
    );

    fprintf(
        config_file,
        "   \"compiler_extra_args_count\": %u,\n"
        "   \"compiler_extra_args\": [\n",
        CTX.compiler_extra_args_count
    );

    for (u8 i = 0; i < CTX.compiler_extra_args_count; i++)
    {
        fprintf(
            config_file,
            "\t\t\"%s\"%s\n",
            CTX.compiler_extra_args[ i ], i + 1 < CTX.compiler_extra_args_count ? "," : ""
        );
    }

    fprintf(
        config_file,
        "   ],\n"
    );
    
    fprintf(
        config_file,
        "   \"linker_extra_args_count\": %u,\n"
        "   \"linker_extra_args\": [\n",
        CTX.linker_extra_args_count
    );
    
    for (u8 i = 0; i < CTX.linker_extra_args_count; i++)
    {
        fprintf(
            config_file,
            "\t\t\"%s\"%s\n",
            CTX.linker_extra_args[ i ], i + 1 < CTX.linker_extra_args_count ? "," : ""
        );
    }
    
    fprintf(
        config_file,
        "   ],\n"
    );

    fprintf(
        config_file,
        "   \"include_dirs_count\": %u,\n"
        "   \"include_dirs\": [\n",
        CTX.include_dirs_count
    );
    
    for (u8 i = 0; i < CTX.include_dirs_count; i++)
    {
        fprintf(
            config_file,
            "\t\t\"%s\"%s\n",
            CTX.include_dirs[ i ], i + 1 < CTX.include_dirs_count ? "," : ""
        );
    }
    
    fprintf(
        config_file,
        "   ],\n"
    );

    fprintf(
        config_file,
        "   \"include_libs_count\": %u,\n"
        "   \"include_libs\": [\n",
        CTX.include_libs_count
    );
    
    for (u8 i = 0; i < CTX.include_libs_count; i++)
    {
        fprintf(
            config_file,
            "\t\t\"%s\"%s\n",
            CTX.include_libs[ i ], i + 1 < CTX.include_libs_count ? "," : ""
        );
    }
    
    fprintf(
        config_file,
        "   ],\n"
    );

    fprintf(
        config_file,
        "   \"include_libs_dir_count\": %u,\n"
        "   \"include_libs_dir\": [\n",
        CTX.include_libs_dir_count
    );
    
    for (u8 i = 0; i < CTX.include_libs_dir_count; i++)
    {
        fprintf(
            config_file,
            "\t\t\"%s\"%s\n",
            CTX.include_libs_dir[ i ], i + 1 < CTX.include_libs_dir_count ? "," : ""
        );
    }
    
    fprintf(
        config_file,
        "   ]\n"
    );

    fprintf(config_file, "}\n");

    fclose(config_file);
}

static bool load_existings_object_files( void )
{
    char obj_file_path [ MAX_PATH + 20 ] = { 0 };

    sprintf(
        obj_file_path,
        "%s/%s/files.json",
        CTX.build_folder_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? "debug" : 
        CTX.build_target == DBUILD_TARGET_RELEASE ? "release" : "shared"
    );
    FILE * f = fopen(obj_file_path, "r");

    if (!f) return false;

    json_obj_t root = json_parser_parse_file(f, (json_parser_mem_allocator*)&allocator);

    if (!root.name || root.type != JSON_OBJ_TYPE_MAP || !root.first_child) return false;
    
    json_obj_t* p = root.first_child;

    if (!p || !STREQ(p->name, "files")) return false;

    p = p->first_child;

    json_obj_t * q = p;

    while (q)
    {
        p = q->first_child;

        file_data fd = (file_data){ };

        while (p)
        {
            if (STREQ(p->name, "type"))
            {
                fd.type = STREQ((const char *)p->value, "C-Header-File") ? DBUILD_FILE_TYPE_C_HEADER_FILE :
                          STREQ((const char *)p->value, "C-Source-File") ? DBUILD_FILE_TYPE_C_SOURCE_FILE : DBUILD_FILE_TYPE_UNKNOWN;
            }
            else if (STREQ(p->name, "path"))
            {
                fd.file_path = dstr_create_from_chars((const char *)p->value, p->value_size_in_bytes, &allocator);
            }
            else if (STREQ(p->name, "last_write_time"))
            {
                json_obj_t * p1 = p->first_child;

                SYSTEMTIME st = { 0 };
                
                while (p1)
                {
                    if (STREQ(p1->name, "year"))
                    {
                        st.wYear = *(DWORD*)p1->value;
                    }    
                    else if (STREQ(p1->name, "month"))
                    {
                        st.wMonth = *(DWORD*)p1->value;
                    }    
                    else if (STREQ(p1->name, "day"))
                    {
                        st.wDay = *(DWORD*)p1->value;
                    }    
                    else if (STREQ(p1->name, "hour"))
                    {
                        st.wHour = *(DWORD*)p1->value;
                    }    
                    else if (STREQ(p1->name, "minute"))
                    {
                        st.wMinute = *(DWORD*)p1->value;
                    }    
                    else if (STREQ(p1->name, "second"))
                    {
                        st.wSecond = *(DWORD*)p1->value;
                    }    
                    else if (STREQ(p1->name, "millisecond"))
                    {
                        st.wMilliseconds = *(DWORD*)p1->value;
                    }    
                    else if (STREQ(p1->name, "day_of_week"))
                    {
                        st.wDayOfWeek = *(DWORD*)p1->value;
                    }  
                    p1 = p1->right_sibling;  
                }
                
                fd.last_write_time = st;
            }
            else if (STREQ(p->name, "dependencies"))
            {
                json_obj_t * p1 = p->first_child;
                
                while (p1)
                {
                    fd.dependecies[ fd.dependencies_count++ ] = dstr_create_from_chars((const char *)p1->value, p1->value_size_in_bytes, &allocator);
                    if (fd.dependencies_count >= DBUILD_MAX_DEPENDENCIES_COUNT) break;
                    p1 = p1->right_sibling;
                }
            }
            p = p->right_sibling;
        } 

        file_data fdi = (file_data){ };
        
        if (!dhash_table_get(CTX.files_table, fd.file_path, &fdi)) continue;

        if (
            fdi.last_write_time.wMilliseconds != fd.last_write_time.wMilliseconds  ||
            fdi.last_write_time.wSecond       != fd.last_write_time.wSecond        ||
            fdi.last_write_time.wMinute       != fd.last_write_time.wMinute        ||
            fdi.last_write_time.wHour         != fd.last_write_time.wHour          ||
            fdi.last_write_time.wDay          != fd.last_write_time.wDay           ||
            fdi.last_write_time.wMonth        != fd.last_write_time.wMonth         ||
            fdi.last_write_time.wYear         != fd.last_write_time.wYear          ||
            fdi.last_write_time.wDayOfWeek    != fd.last_write_time.wDayOfWeek
        )
        {
            fdi = fd;
            fdi.is_changed = true;
            dhash_table_set(CTX.files_table, fdi.file_path, &fdi);
        }

        q = q->right_sibling;
    }

    filter_scanned_files_to_compile();

    return true;
}

static bool link_all_object_files( void )
{
    dstr_t cmd = dstr_create_from_cstr_format(
        &allocator,
        "gcc -o %s/%s/%s %s %s/%s/object_files/* %s %s %s",
        CTX.build_folder_name, 
        CTX.build_target == DBUILD_TARGET_DEBUG ? "debug" : 
        CTX.build_target == DBUILD_TARGET_RELEASE ? "release" : "shared",
        CTX.build_target == DBUILD_TARGET_DEBUG ? CTX.output_debug_name : 
        CTX.build_target == DBUILD_TARGET_RELEASE ? CTX.output_release_name : CTX.output_shared_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? DBUILD_DEBUG_COMPILATION_FLAG : 
        CTX.build_target == DBUILD_TARGET_RELEASE ? DBUILD_RELEASE_COMPILATION_FLAG : DBUILD_SHARED_COMPILATION_FLAG,
        CTX.build_folder_name, 
        CTX.build_target == DBUILD_TARGET_DEBUG ? "debug" : 
        CTX.build_target == DBUILD_TARGET_RELEASE ? "release" : "shared",
        CTX.linker_all_flags ? CTX.linker_all_flags : "", 
        CTX.include_libs_dirs_all_flags ? CTX.include_libs_dirs_all_flags : "",
        CTX.include_libs_all_flags ? CTX.include_libs_all_flags : ""
    );
    
    PROCESS_INFORMATION pi = { 0 };
    ZeroMemory(&pi, sizeof(pi));
    STARTUPINFOA si = { 0 };
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    
    WINBOOL res = CreateProcessA(
        NULL,
        cmd, 
        NULL, 
        NULL, 
        FALSE, 
        0, 
        NULL, 
        NULL, 
        &si, 
        &pi
    );

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exit_code;
    WINBOOL has_exit = GetExitCodeProcess(pi.hProcess, &exit_code);

    return has_exit && res == TRUE && exit_code == EXIT_SUCCESS;
}

static void ___scan_sources_and_update___(FILE* f, char* dir, int argc, char** argv)
{
    static int number_of_times = 0;

    
    HANDLE h = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA fd = { 0 };

    char dir_path [ MAX_PATH ]= { 0 };
    sprintf(
        dir_path,
        "%s/*", dir
    );
    
    h = FindFirstFileA(dir_path, &fd);
    
    if (!h || h == INVALID_HANDLE_VALUE) return;
    
    do {
        if (STREQ(fd.cFileName, ".") || STREQ(fd.cFileName, "..")) continue;
        
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            char new_dir_path [ MAX_PATH * 2 ] = { 0 };
            
            sprintf(
                new_dir_path,
                "%s/%s",
                dir, fd.cFileName
            );
            ___scan_sources_and_update___(f, new_dir_path, argc, argv);
        }
        else
        {
            if (number_of_times++)
            {
                fprintf(f, ",\n");
            }
            SYSTEMTIME st = { 0 };
            FileTimeToSystemTime(&fd.ftLastWriteTime, &st);

            const char* type = str_is_suffix(fd.cFileName, ".c") ? "C-Source-File" :
                               str_is_suffix(fd.cFileName, ".h") ? "C-Header-File" : "Unknown";

            char file_path[ MAX_PATH * 2 ] = { 0 };

            sprintf(file_path, "%s/%s", dir, fd.cFileName);

            fprintf(
                f, 
                "\t\t{\n"
                "\t\t\t\"type\": \"%s\",\n"
                "\t\t\t\"path\": \"%s\",\n"
                "\t\t\t\"last_write_time\": {\n"
                "\t\t\t\t\"year\": %d,\n"
                "\t\t\t\t\"month\": %d,\n"
                "\t\t\t\t\"day\": %d,\n"
                "\t\t\t\t\"hour\": %d,\n"
                "\t\t\t\t\"minute\": %d,\n"
                "\t\t\t\t\"second\": %d,\n"
                "\t\t\t\t\"millisecond\": %d,\n"
                "\t\t\t\t\"day_of_week\": %d\n"
                "\t\t\t},\n"
                "\t\t\t\"dependencies\": [\n"
                ,
                type,
                file_path,
                st.wYear,
                st.wMonth,
                st.wDay,
                st.wHour,
                st.wMinute,
                st.wSecond,
                st.wMilliseconds,
                st.wDayOfWeek
            );

            dstr_builder_t cmd_bld = dstr_builder_create(KB(1), &allocator);

            dstr_builder_append_cstr_format(
                cmd_bld,
                "gcc -MM %s ",
                file_path
            );

            for (int i = 4; i < argc; i++)
            {
                dstr_builder_append_cstr_format(
                    cmd_bld,
                    "%s ", argv[ i ]
                );
            }
            
            dstr_t cmd = dstr_builder_generate_dstr(cmd_bld, &allocator);

            dstr_builder_t builder = dstr_builder_create(KB(1), &allocator);
            
            FILE * cmd_stream = popen(cmd, "r");

            char buff [ 256 ] = { 0 };
            
            while (cmd_stream && !feof(cmd_stream))
            {
                if (fgets(buff, KB(1) - 1, cmd_stream) != NULL)
                {
                    dstr_builder_append_cstr(builder, buff);
                }
            }
     
            pclose(cmd_stream);

            dstr_builder_replace_all_char(builder, '\\', '/');
            dstr_t output = dstr_builder_generate_dstr(builder, &allocator);

            {            
                int j = 0;
                while (output && j < (int)dstrlen(output))
                {
                    int i = j;
                    while (i < (int)dstrlen(output) && !isspace(*(output + i))) i++;
                    
                    if (i > 2 && (*(output + i - 1) == 'c' || *(output + i - 1) == 'h'))
                    {
                        dstr_t dependacy = dstr_create_from_chars(output + j, i - j, &allocator);

                        while (i < (int)dstrlen(output) && !isspace(*(output + i))) i++;
                        
                        fprintf(
                            f,
                            "\t\t\t\t\"%s\"%s\n",
                            dependacy,
                            i + 2 < (int)dstrlen(output) ? "," : ""
                        );
                    }
                    j = ++i;

                }
            }

            fprintf(
                f, "\t\t\t]\n\t\t}"
            );
        }
    } while(FindNextFileA(h, &fd));
    CloseHandle(h);
}

static void save_new_object_files( int argc, char** argv )
{
    assert(argc > 3);
    char* name = argv[ 3 ];
    FILE * f = fopen(name, "w");
    if (!f) return;

    fprintf(f, "// Files Data: DO NOT EDIT THIS FILE MANUALLY!!\n{\n\t\"files\": [\n");

    ___scan_sources_and_update___(f, argv[ 2 ], argc, argv);
    
    fprintf(f, "\n\t]\n}\n");
    
    fclose(f);
}

static bool compile_source_file( file_data* file, PROCESS_INFORMATION* pi)
{
    if (!pi || !file) return false;

    char file_name [ MAX_PATH ] = { 0 };
    
    create_obj_file_name_from_path(file->file_path, file_name);

    dstr_t command = dstr_create_from_cstr_format(
        &allocator,
        "gcc -o %s/%s/object_files/%s %s %s -c %s %s",
        CTX.build_folder_name, 
        CTX.build_target == DBUILD_TARGET_DEBUG ? "debug" : 
        CTX.build_target == DBUILD_TARGET_RELEASE ? "release" : "shared",
        file_name,
        CTX.build_target == DBUILD_TARGET_DEBUG ? DBUILD_DEBUG_COMPILATION_FLAG : 
        CTX.build_target == DBUILD_TARGET_RELEASE ? DBUILD_RELEASE_COMPILATION_FLAG : DBUILD_SHARED_COMPILATION_FLAG,
        file->file_path,
        CTX.compiler_all_flags ? CTX.compiler_all_flags : "",
        CTX.include_all_dirs_flags ? CTX.include_all_dirs_flags : ""
    );

    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));
    STARTUPINFOA si = { 0 };
    ZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(si);

    WINBOOL res = CreateProcessA(
        NULL,
        command, 
        NULL, 
        NULL, 
        FALSE, 
        0, 
        NULL, 
        NULL, 
        &si, 
        pi
    );
    
    return res == TRUE;
}

static inline bool CWD_is_empty( void )
{
    HANDLE h = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA fd = { 0 };

    char path [ MAX_PATH ];
    DWORD res = GetCurrentDirectoryA(MAX_PATH, path);

    if (res > 0 && res < MAX_PATH)
    {
        u32 path_len = strlen(path);
        path[ path_len ] = '/';
        path[ path_len + 1 ] = '*';
        path[ path_len + 2 ] = '\0';
        h = FindFirstFileA(path, &fd);

        if (!h || h == INVALID_HANDLE_VALUE) return false; 

        do {
            if (!STREQ(fd.cFileName, ".") && !STREQ(fd.cFileName, "..") && !STREQ(fd.cFileName, CTX.executable_name)) 
            {
                return false;
            }
        } while (FindNextFileA(h, &fd));

        CloseHandle(h);
        return true;
    }

    return false;
}

static inline void set_exec_name_from_path( char *exec_path )
{
    if (!exec_path) return;

    u32 exec_len = strlen(exec_path);
    i32 i = (i32)exec_len - 1;
    
    while (i >= 0 && exec_path[ i ] != '/' && exec_path[ i ] != '\\') i--;

    char* exec_start = exec_path + i + 1; // Ignore the delim '/' or '\\'
    
    memset(CTX.executable_name, 0, MAX_PATH);
    memcpy(CTX.executable_name, exec_start, exec_len - i - 1);   
}

static inline int run_command( char *cmd )
{
    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFOA        si = { 0 };

    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    
    si.cb = sizeof(si);

    WINBOOL res = CreateProcessA(
        NULL,
        cmd, 
        NULL, 
        NULL, 
        FALSE, 
        0, 
        NULL, 
        NULL, 
        &si, 
        &pi
    );

    return (int)res;
}

static inline bool is_file_forced( dstr_t file_path )
{
    return dhash_table_get(CTX.forced_files_table, file_path, NULL);
}

static void create_build_directory( void )
{
    if (CTX.should_clean)
    {
        clean_build_cache();
    }

    if (!CreateDirectoryA(CTX.build_folder_name, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            printf("Something went wrong checking if the '%s' directory exists.\n", CTX.build_folder_name);
            exit(EXIT_FAILURE);
        }
    }
    
    char sub_dirs [ 2 * MAX_PATH ] = { 0 };
    u32 len = strlen(CTX.build_folder_name);
    memcpy(sub_dirs, CTX.build_folder_name, len);
    const char * suffix = "/debug";
    memcpy(sub_dirs + len, suffix, strlen(suffix));
    
    if (!CreateDirectoryA(sub_dirs, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            printf("Something went wrong checking if the '%s' directory exists.\n", sub_dirs);
            exit(EXIT_FAILURE);
        }
    }

    memset(sub_dirs + len, 0, 2 * MAX_PATH - len);
    suffix = "/release\0";
    memcpy(sub_dirs + len, suffix, strlen(suffix));
    
    if (!CreateDirectoryA(sub_dirs, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            printf("Something went wrong checking if the '%s' directory exists.\n", sub_dirs);
            exit(EXIT_FAILURE);
        }
    }

    memset(sub_dirs + len, 0, 2 * MAX_PATH - len);
    suffix = "/shared\0";
    memcpy(sub_dirs + len, suffix, strlen(suffix));
    
    if (!CreateDirectoryA(sub_dirs, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            printf("Something went wrong checking if the '%s' directory exists.\n", sub_dirs);
            exit(EXIT_FAILURE);
        }
    }

    memset(sub_dirs + len, 0, 2 * MAX_PATH - len);
    suffix = "/debug/object_files\0";
    memcpy(sub_dirs + len, suffix, strlen(suffix));
    
    if (!CreateDirectoryA(sub_dirs, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            printf("Something went wrong checking if the '%s' directory exists.\n", sub_dirs);
            exit(EXIT_FAILURE);
        }
    }

    memset(sub_dirs + len, 0, 2 * MAX_PATH - len);
    suffix = "/release/object_files\0";
    memcpy(sub_dirs + len, suffix, strlen(suffix));
    
    if (!CreateDirectoryA(sub_dirs, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            printf("Something went wrong checking if the '%s' directory exists.\n", sub_dirs);
            exit(EXIT_FAILURE);
        }
    }

    memset(sub_dirs + len, 0, 2 * MAX_PATH - len);
    suffix = "/shared/object_files\0";
    memcpy(sub_dirs + len, suffix, strlen(suffix));
    
    if (!CreateDirectoryA(sub_dirs, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            printf("Something went wrong checking if the '%s' directory exists.\n", sub_dirs);
            exit(EXIT_FAILURE);
        }
    }
}

static inline void create_flags_strings(void)
{
    dstr_builder_t bd = dstr_builder_create(KB(1), &allocator);

    for (u32 i = 0; i < CTX.include_dirs_count; i++)
    {
        dstr_builder_append_cstr_format(
            bd,
            "-I%s ",
            CTX.include_dirs[ i ]
        );
    }

    dstr_builder_append_cstr_format(
        bd,
        "-I%s ",
        CTX.src_folder_name
    );

    CTX.include_all_dirs_flags = dstr_builder_generate_dstr(bd, &allocator);

    dstr_builder_clear(bd);

    for (u32 i = 0; i < CTX.compiler_extra_args_count; i++)
    {
        dstr_builder_append_cstr_format(
            bd,
            "%s ",
            CTX.compiler_extra_args[ i ]
        );
    }

    CTX.compiler_all_flags = dstr_builder_generate_dstr(bd, &allocator);
    
    dstr_builder_clear(bd);

    for (u32 i = 0; i < CTX.linker_extra_args_count; i++)
    {
        dstr_builder_append_cstr_format(
            bd,
            "%s ",
            CTX.linker_extra_args[ i ]
        );
    }

    CTX.linker_all_flags = dstr_builder_generate_dstr(bd, &allocator);
    
    dstr_builder_clear(bd);

    for (u32 i = 0; i < CTX.include_libs_count; i++)
    {
        dstr_builder_append_cstr_format(
            bd,
            "-l%s ",
            CTX.include_libs[ i ]
        );
    }

    CTX.include_libs_all_flags = dstr_builder_generate_dstr(bd, &allocator);
    
    dstr_builder_clear(bd);

    for (u32 i = 0; i < CTX.include_libs_dir_count; i++)
    {
        dstr_builder_append_cstr_format(
            bd,
            "-L%s ",
            CTX.include_libs_dir[ i ]
        );
    }

    CTX.include_libs_dirs_all_flags = dstr_builder_generate_dstr(bd, &allocator);
}

static void filter_scanned_files_to_compile(void)
{  
    dhash_table_iter_t * it = dhash_table_iter_create(CTX.files_table);

    file_data fd = (file_data){ };

    while (dhash_table_iter_next(it, &fd))
    {
        if (fd.is_changed || fd.type != DBUILD_FILE_TYPE_C_SOURCE_FILE) continue;

        if (dhash_table_get(CTX.forced_files_table, fd.file_path, NULL))
        {
            fd.should_compile = true;
            dhash_table_set(CTX.files_table, fd.file_path, &fd);
            continue;
        }

        for (u32 i = 0; i < fd.dependencies_count; i++)
        {
            file_data dfd = (file_data){ };
            if (dhash_table_get(CTX.files_table, fd.dependecies[ i ], &dfd))
            {
                if (dfd.is_changed)
                {
                    fd.should_compile = true;
                    dhash_table_set(CTX.files_table, fd.file_path, &fd);
                    break;
                }
            }
        }
    }
}

static void scan_source_directory( const char *directory )
{
    HANDLE h            = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA fd = { 0 };

    u32 dirlen = strlen(directory);
    char dir [ MAX_PATH ];

    memcpy(dir, directory, dirlen);
    dir[ dirlen ] = '/';
    dir[ dirlen + 1 ] = '*';
    dir[ dirlen + 2 ] = '\0';

    h = FindFirstFileA(dir, &fd);

    if (CTX.should_print_info) printf("Scanning Directory: '%s'.\n", dir);

    do
    {
        if (STREQ(fd.cFileName, ".") || STREQ(fd.cFileName, "..")) continue;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            char next_dir [ MAX_PATH * 2 ] = { 0 };

            sprintf(
                next_dir,
                "%s/%s",
                directory,
                fd.cFileName
            );

            scan_source_directory(next_dir);
        }
        else
        {
            char file_path [ MAX_PATH * 2 ] = { 0 };

            sprintf(
                file_path,
                "%s/%s",
                directory,
                fd.cFileName
            );

            str_replace(file_path, '\\', '/');
    
            SYSTEMTIME st;
            FileTimeToSystemTime(&fd.ftLastWriteTime, &st);

            file_data filedata = (file_data){ };

            filedata.file_path          = dstr_create_from_path(file_path);
            filedata.last_write_time    = st;
            filedata.dependencies_count = 0;
            filedata.is_changed         = false;
            filedata.should_compile     = true;
            filedata.type               = str_is_suffix(file_path, ".c") ? DBUILD_FILE_TYPE_C_SOURCE_FILE :
                                           str_is_suffix(file_path, ".h") ? DBUILD_FILE_TYPE_C_HEADER_FILE : 
                                                                            DBUILD_FILE_TYPE_UNKNOWN;

            dhash_table_add(CTX.files_table, filedata.file_path, &filedata);
        }
    } while (FindNextFileA(h, &fd));
    CloseHandle(h);
}

static void create_template()
{
    if (CTX.template_target == DBUILD_TEMPLATE_VSCODE)
    {
        char vscode_dir [ MAX_PATH ] = { 0 }; 
        const char* vscode_pref = ".vscode\0";
        u32 pref_len = strlen(vscode_pref);

        memcpy(vscode_dir, vscode_pref, pref_len);

        if (!CreateDirectoryA(vscode_dir, NULL))
        {
            DWORD err = GetLastError();
            if (err != ERROR_ALREADY_EXISTS)
            {
                fprintf(stderr, "something went wrong creating .vscode directory while trying to create template.");
                exit(EXIT_FAILURE);
            }
        }

        char* suff;
        u32 suff_len;
        char _debugger_path [ MAX_PATH ] = { 0 };
        char _compiler_path [ MAX_PATH ] = { 0 };

        FILE* cmd_p = popen("where gdb", "r");
        
        if (cmd_p) fgets(_debugger_path, MAX_PATH, cmd_p);

        pclose(cmd_p);
        cmd_p = popen("where gcc", "r");
        
        if (cmd_p) fgets(_compiler_path, MAX_PATH, cmd_p);

        pclose(cmd_p);

        dstr_t debugger_path = dstr_create_from_path(_debugger_path);
        dstr_t compiler_path = dstr_create_from_path(_compiler_path);

        suff = "/launch.json\0";
        suff_len = strlen(suff);
        memcpy(vscode_dir + pref_len, suff, suff_len);

        FILE* f = fopen(vscode_dir, "w");
        if (!f) exit(EXIT_FAILURE);
        fprintf(
            f,
            "{\n"
            "   \"version\": \"0.2.0\",\n"
            "   \"configurations\": [\n"
            "       {\n"
            "          \"name\": \"Debug\",\n"
            "          \"type\": \"cppdbg\",\n"
            "          \"program\": \"${workspaceFolder}/%s/%s\",\n"
            "          \"args\": [ ],\n"
            "          \"stopAtEntry\": false,\n"
            "          \"cwd\": \"${workspaceFolder}\",\n"
            "          \"environment\": [ ],\n"
            "          \"externalConsole\": true,\n"
            "          \"miDebuggerPath\": \"%s\",\n"
            "          \"preLaunchTask\": \"Build (Debug)\",\n"
            "          \"request\": \"launch\"\n"
            "       },\n"
            "       {\n"
            "          \"name\": \"Clean-Cache & Debug\",\n"
            "          \"type\": \"cppdbg\",\n"
            "          \"program\": \"${workspaceFolder}/%s/%s\",\n"
            "          \"args\": [ ],\n"
            "          \"stopAtEntry\": false,\n"
            "          \"cwd\": \"${workspaceFolder}\",\n"
            "          \"environment\": [ ],\n"
            "          \"externalConsole\": true,\n"
            "          \"miDebuggerPath\": \"%s\",\n"
            "          \"preLaunchTask\": \"Build & Clean (Debug)\",\n"
            "          \"request\": \"launch\"\n"
            "       }\n"
            "   ]\n"
            "}\n",
            CTX.build_folder_name, CTX.output_debug_name,
            debugger_path,
            CTX.build_folder_name, CTX.output_debug_name,
            debugger_path
        );

        fclose(f);
        memset(vscode_dir + pref_len, 0, MAX_PATH - pref_len);

        suff = "/extensions.json\0";
        suff_len = strlen(suff);
        memcpy(vscode_dir + pref_len, suff, suff_len);
        f = fopen(vscode_dir, "w");
        if (!f) exit(EXIT_FAILURE);
        fprintf(
            f,
            "{\n"
            "   \"recommendations\": [\n"
            "       \"ms-vscode.cpptools\",\n"
            "       \"ms-vscode.cpp-devtools\",\n"
            "       \"ms-vscode.cpptools-extension-pack\",\n"
            "       \"formulahendry.code-runner\",\n"
            "       \"usernamehw.errorlens\",\n"
            "       \"chouzz.vscode-better-align\",\n"
            "       \"eamodio.gitlens\",\n"
            "       \"gruntfuggly.triggertaskonsave\"\n"
            "   ]\n"
            "}\n"
        );

        fclose(f);
        memset(vscode_dir + pref_len, 0, MAX_PATH - pref_len);

        suff = "/settings.json\0";
        suff_len = strlen(suff);
        memcpy(vscode_dir + pref_len, suff, suff_len);
        f = fopen(vscode_dir, "w");
        if (!f) exit(EXIT_FAILURE);
        fprintf(
            f,
            "{""\n"
            "    \"code-runner.executorMap\": {""\n"
            "        \"c\": \".\\\\%s\\\\debug\\\\%s\"""\n"
            "    },""\n"
            "    \"code-runner.showExecutionMessage\": false,""\n"
            "    \"code-runner.clearPreviousOutput\": true,""\n"
            "    \"code-runner.runInTerminal\": true,""\n"
            "    \"code-runner.saveAllFilesBeforeRun\": true,""\n"
            "    \"files.associations\" : {""\n"
            "        \"*.h\" : \"c\",""\n"
            "        \"*.c\" : \"c\",""\n"
            "        \"settings.json\": \"jsonc\"""\n"
            "    },""\n"
            "    \"triggerTaskOnSave.tasks\": {""\n"
            "          \"Build (Debug)\": [""\n"
            "             \"**/*.c\",""\n"
            "             \"**/*.h\"""\n"
            "          ]""\n"
            "    },""\n"
            "    {""\n"
            "        \"editor.quickSuggestions\": {""\n"
            "            \"other\": true""\n"
            "        },""\n"
            "        \"editor.suggestOnTriggerCharacters\": true,""\n"
            "        \"editor.acceptSuggestionOnEnter\": \"on\",""\n"
            "        \"editor.tabCompletion\": \"on\"""\n"
            "    }""\n"
            "}""\n"
            ,
            CTX.build_folder_name, CTX.output_debug_name
        );

        fclose(f);
        memset(vscode_dir + pref_len, 0, MAX_PATH - pref_len);
        suff = "/c_cpp_properties.json\0";
        suff_len = strlen(suff);
        memcpy(vscode_dir + pref_len, suff, suff_len);
        f = fopen(vscode_dir, "w");
        if (!f) exit(EXIT_FAILURE);
        fprintf(
            f,
            "{""\n"
            "    \"configurations\": [""\n"
            "        {""\n"
            "            \"name\": \"Win32 C configurations\",""\n"
            "            \"includePath\": [""\n"
            "                \"${workspaceFolder}\",""\n"
            "                \"${workspaceFolder}/%s/**\",""\n"
            "            ],""\n"
            "            \"defines\": [""\n"
            "                \"_DEBUG\",""\n"
            "                \"UNICODE\",""\n"
            "                \"_UNICODE\",""\n"
            "            ],""\n"
            "            \"windowsSdkVersion\": \"10.0.26100.0\",""\n"
            "            \"compilerPath\": \"%s\",""\n"
            "            \"intelliSenseMode\": \"windows-gcc-x64\",""\n"
            "            \"cStandard\": \"c17\",""\n"
            "            \"cppStandard\": \"c++17\"""\n"
            "        }""\n"
            "    ],""\n"
            "    \"version\": 4""\n"
            "}""\n"
            ,
            CTX.src_folder_name, compiler_path
        );

        fclose(f);
        memset(vscode_dir + pref_len, 0, MAX_PATH - pref_len);
        suff = "/tasks.json\0";
        suff_len = strlen(suff);
        memcpy(vscode_dir + pref_len, suff, suff_len);
        f = fopen(vscode_dir, "w");
        if (!f) exit(EXIT_FAILURE);
        fprintf(
            f,
            "{""\n"
            "    \"tasks\" : ""\n"
            "    [""\n"
            "        {""\n"
            "            \"type\": \"shell\",""\n"
            "            \"label\": \"Build & Run (Release)\",""\n"
            "            \"command\": \"${workspaceFolder}\\\\%s\",""\n"
            "            \"args\": [ \"--build-release\", \"--post-build\", \".\\\\%s\\\\release\\\\%s\" ],""\n"
            "            \"problemMatcher\": [\"$gcc\"],""\n"
            "            \"group\": \"build\"""\n"
            "        },""\n"
            "        {""\n"
            "            \"type\": \"shell\",""\n"
            "            \"label\": \"Build (Debug)\",""\n"
            "            \"command\": \"${workspaceFolder}\\\\%s\",""\n"
            "            \"args\": [ \"--build-debug\" ],""\n"
            "            \"problemMatcher\": [\"$gcc\"],""\n"
            "            \"group\": \"build\"""\n"
            "        },""\n"
            "        {""\n"
            "            \"type\": \"shell\",""\n"
            "            \"label\": \"Build & Clean (Debug)\",""\n"
            "            \"command\": \"${workspaceFolder}\\\\%s\",""\n"
            "            \"args\": [ \"--build-debug\", \"--clean\"],""\n"
            "            \"problemMatcher\": [\"$gcc\"],""\n"
            "            \"group\": \"build\"""\n"
            "        }""\n"
            // TODO: Add more....
            "    ]""\n"
            "}""\n"
            ,
            CTX.executable_name,
            CTX.build_folder_name,
            CTX.output_release_name,
            CTX.executable_name,
            CTX.executable_name
        );

        fclose(f);
        
        char src_dir [ MAX_PATH ] = { 0 };
        pref_len = strlen(CTX.src_folder_name);
        memcpy(src_dir, CTX.src_folder_name, pref_len);

        if (!CreateDirectoryA(src_dir, NULL))
        {
            DWORD err = GetLastError();
            if (err != ERROR_ALREADY_EXISTS)
            {
                fprintf(stderr, "something went wrong creating src directory while trying to create template.");
                exit(EXIT_FAILURE);
            }
        }

        
        suff = "/main.c\0";
        suff_len = strlen(suff);
        memcpy(src_dir + pref_len, suff, suff_len);

        f = fopen(src_dir, "w");
        if (!f) exit(EXIT_FAILURE);

        fprintf(
            f,
            "// This file generated by dbuild ystem. Enjoy :)""\n"
            "// To build press ctrl+shift+b and choose Build (Debug)""\n"
            "// To run press ctrl+alt+n""\n"
            """\n"
            "# include <stdio.h>""\n"
            """\n"
            "int main( void )""\n"
            "{""\n"
            "   printf(\"Hello World!\");""\n"
            "   return 0;""\n"
            "}""\n"
        );

        fclose(f);

    }
    else if (CTX.template_target == DBUILD_TEMPLATE_CLANGD)
    {
        FILE*f = fopen(".clangd", "w");
        fprintf(
            f,
            "CompileFlags:\n\tADD:\n\t- -I%s\n",
            CTX.src_folder_name
        );
    }
}

static inline void print_help( void )
{
    printf(
        "                                                                                                        ""\n"
        "     =====================================================================================              ""\n"
        "     #####                          dbuild.h - Help message                          #####              ""\n"
        "     =====================================================================================              ""\n"
        "                                                                                                        ""\n"
    );
    printf(
        "Usage .\\dbld.exe [Options]""\n"
        "    NOTE: .\\dbld.exe without args does the following:""\n"
        "        if current directory is empty then it generates a new default template.""\n"
        "        if current directory is not empty and you save the configurations at least once then it runs the saved configurations.""\n"
        "        otherwise it prints this message.""\n"
    );
    printf(
        "Options:""\n"
        "  -bd,        --build-debug            add define build target as debug.""\n"
        "  -br,        --build-release          add define build target as release.""\n"
        "  -bs,        --build-shared           add define build target as shared library.""\n"
        "  -od,        --output-debug           set output file name of target debug.""\n"
        "  -or,        --output-release         set output file name of target release.""\n"
        "  -os,        --output-shared          set output file name of target shared library.""\n"
        "  -c,         --clean                  tells the builder to clean all targets defined.""\n"
        "  -s,         --save-config            tells the builder to save current configurations.""\n"
        "  -uc,        --use-config             tells the builder to load configurations and use it in current build.""\n"
        "  -h,         --help                   prints this help message & exits.""\n"
        "  -cf,        --compiler-flag          adds compilation flag to the compiler.""\n"
        "                                          NOTE: should be followed immidiatly with a flag.""\n"
        "  -lf,        --linker-flag            adds linking flag to the linker.""\n"
        "                                          NOTE: should be followed immidiatly with a flag.""\n"
        "  -i,         --info                   adds information printing while building.""\n"
        "  -bt-vscode, --build-template-vscode  building a template (Quick start) for vs code.""\n"
        "  -bt-clangd, --build-template-clangd  building a template (Quick start) for clangd.""\n"
        "  -src,       --source-directory       set the source directory (default: 'src').""\n"
        "  -bld,       --build-directory        set the build directory (default: 'build').""\n"
        "  -ff,        --force-file             forcing file to be compiled even if it has no changes.""\n"
        "                                          NOTE: should be followed immidiatly with a file name.""\n"
        "  -I,         --include-directory      add an include directory to the compilation step.""\n"
        "                                          NOTE: should be followed immidiatly with a dir path.""\n"
        "  -L,         --link-directory         add a link directory to the linker step.""\n"
        "                                          NOTE: should be followed immidiatly with a dir path.""\n"
        "  -l,         --link-library           add a link library to the linker step.""\n"
        "                                          NOTE: should be followed immidiatly with the lib name.""\n"
        "  -pr,        --pre-build              add a pre build task to run before linking (after compiling).""\n"
        "                                          NOTE: should be followed immidiatly with the cmd string.""\n"
        "  -po,        --post-build             add a post build task to run after linking (after compiling).""\n"
        "                                          NOTE: should be followed immidiatly with the cmd string.""\n"
        "                                          NOTE: this task will not run if linking failed.""\n"
        """\n"
    );
    printf(
        "Examples:""\n"
        ".\\dbld.exe -br                               this command will run your current saved configuration file on the release target.""\n"
        ".\\dbld.exe -br -cf -Wall -i                  this command will run your current saved configuration file on the release target.""\n"
        "                                                      & add to the compilation flag the '-Wall' flag & print info while doing it.""\n"
        ".\\dbld.exe -bd -c -i                         this command will run your current saved configuration file on the""\n"
        "                                                      debug target & clean entire build/debug dir & print info while doing it.""\n"
        ".\\dbld.exe -bs -I C:/libs/my_lib/include     this command will run your current saved configuration file on the""\n"
        "                                                      shared target & add the C:/libs/my_lib/include as include path at compilation.""\n"
        ".\\dbld.exe -s -bs -L C:/libs/my_lib/include  this command will run your current saved configuration file on the""\n"
        "                                                      shared target & add the C:/libs/my_lib/include as include path at linking &.""\n"
        "                                                      override your current config file with those settings.""\n"
    );
    printf(
        "Compiler-Flags:""\n"
        "  When you compile the builder you need to create a source file 'dbuild.c' and include the dbuild.h file.""\n"
        "  You must also define DBUILD_IMPLEMENTATION before including the header file.""\n"
        "  You can also modified the following values by defining the values before compiling the builder:""\n"
        "  1.  DBUILD_MEMORY_SIZE:                   default is 1 MB,    this is used for all allocations in the program." "\n" 
        "                                                                   NOTE: if the builder prints error message 'Out-Of-Memory' you should increase this value.""\n"
        "  2.  DBUILD_MAX_COMPILER_EXTRA_ARGS:  default is 32,      this is used when you add a compiler extra arg using '-cf' flag.""\n"
        "  3.  DBUILD_MAX_FILES_COUNT:          default is 1024,    this is the max total source files you have on the source directory.""\n"
        "  4.  DBUILD_TEMPLATE_DEFAULT:         default is vscode,  this is the template the builder will use when running the builder on empty folder with no args.""\n"
        "  5.  DBUILD_BUILD_TARGET_DEFAULT:     default is debug,   this is the build target the builder will use when running the builder on non empty folder with no args.""\n"
        "  6.  DBUILD_BUILD_DIRECTORY_DEFAULT:  default is 'build', this is the default build directory if it does not exists the builder will create it.""\n"
        "  7.  DBUILD_SOURCE_DIRECTORY_DEFAULT: default is 'src',   this is the default source directory if it does not exists the builder will throw an error.""\n"
        "  8.  DBUILD_MAX_FORCED_FILES:         default is 32,      this is the total count of forced file compilation requested using '-ff' flag.""\n"
        "  9.  DBUILD_MAX_INCLUDE_DIRECTORIES:  default is 32,      this is the total count of include directories you are using, (requested by '-I' flag).""\n"
        "  10. DBUILD_MAX_DEPENDENCIES_COUNT:    default is 64,      this is the total count of include you can use in any source file (include recursive).""\n"
        "\n"
    );
}

static bool parse_args( const int argc, char* argv[] )
{
    for (int i = 1; i < argc; i++)
    {
        const char* arg = argv[ i ];
        u32 len = strlen(arg); 
        
        if (len > 1 && STR_START_WITH(arg, '-'))
        {
                 if (STREQ(arg, "-bd")        || STREQ(arg, "--build-debug")          ) { CTX.build_target = DBUILD_TARGET_DEBUG;        }
            else if (STREQ(arg, "-br")        || STREQ(arg, "--build-release")        ) { CTX.build_target = DBUILD_TARGET_RELEASE;      }
            else if (STREQ(arg, "-bs")        || STREQ(arg, "--build-shared")         ) { CTX.build_target = DBUILD_TARGET_SHARED;       }
            else if (STREQ(arg, "-c")         || STREQ(arg, "--clean")                ) { CTX.should_clean = true;                      }
            else if (STREQ(arg, "-s")         || STREQ(arg, "--save-config")          ) { CTX.should_save_config = true;                }
            else if (STREQ(arg, "-uc")        || STREQ(arg, "--use-config")           ) { CTX.should_use_config  = true;                }
            else if (STREQ(arg, "-h")         || STREQ(arg, "--help")                 ) { print_help(); exit(EXIT_SUCCESS);             }
            else if (STREQ(arg, "-i")         || STREQ(arg, "--info")                 ) { CTX.should_print_info = true;                 }
            else if (STREQ(arg, "-bt-vscode") || STREQ(arg, "--build-template-vscode")) { CTX.template_target = DBUILD_TEMPLATE_VSCODE; }
            else if (STREQ(arg, "-bt-clangd") || STREQ(arg, "--build-template-clangd")) { CTX.template_target = DBUILD_TEMPLATE_CLANGD; }
            else if (STREQ(arg, "-od")        || STREQ(arg, "--output-debug")         )
            { 
                ASSURE_NOT_LAST(i, argc, "-od | --output-debug needs additional arg telling what the output file name should be.\n"); 
                u32 name_len = strlen(argv[ ++i ]); 
                if (!str_is_suffix( argv[ i ], ".exe"))
                {
                    fprintf(
                        stderr,
                        "You entered this name for the executable of debug target '%s' which does not ends with '.exe'.\n"
                        "You must use a valid name for the execuatble for example main_dbg.exe.\n",
                        argv[ i ]
                    );
                    exit(EXIT_FAILURE);
                }
                memcpy(CTX.output_debug_name, argv[ i ], name_len); 
                CTX.output_debug_name[ name_len ] = '\0';
            }
            else if (STREQ(arg, "-or")        || STREQ(arg, "--output-release")       )
            { 
                ASSURE_NOT_LAST(i, argc, "-or | --output-release needs additional arg telling what the output file name should be.\n"); 
                u32 name_len = strlen(argv[ ++i ]); 
                if (!str_is_suffix( argv[ i ], ".exe"))
                {
                    fprintf(
                        stderr,
                        "You entered this name for the executable of release target '%s' which does not ends with '.exe'.\n"
                        "You must use a valid name for the execuatble for example main_rls.exe.\n",
                        argv[ i ]
                    );
                    exit(EXIT_FAILURE);
                }
                memcpy(CTX.output_release_name, argv[ i ], name_len); 
                CTX.output_release_name[ name_len ] = '\0'; 
            }
            else if (STREQ(arg, "-os")        || STREQ(arg, "--output-shared")        )
            { 
                ASSURE_NOT_LAST(i, argc, "-os | --output-shared needs additional arg telling what the output file name should be.\n"); 
                u32 name_len = strlen(argv[ ++i ]); 
                if (!str_is_suffix( argv[ i ], ".dll"))
                {
                    fprintf(
                        stderr,
                        "You entered this name for the executable of shared target '%s' which does not ends with '.dll'.\n"
                        "You must use a valid name for the execuatble for example main_srd.dll.\n",
                        argv[ i ]
                    );
                    exit(EXIT_FAILURE);
                }
                memcpy(CTX.output_shared_name, argv[ i ], name_len); 
                CTX.output_shared_name[ name_len ] = '\0'; 
            }
            else if (STREQ(arg, "-cf")        || STREQ(arg, "--compiler-flag")        )
            {
                ASSURE_NOT_LAST(i, argc, "-cf | --compiler-flag needs additional arg with the actual flag to pass.\n");
                if (CTX.compiler_extra_args_count >= DBUILD_MAX_COMPILER_EXTRA_ARGS) 
                {
                    printf("ERROR: You compile the builder with a limit of %d extra argss to the compiler/linker, and You passed too much.\n", DBUILD_MAX_COMPILER_EXTRA_ARGS);
                    printf("\tNOTE: in order to chnge the max compiler args You can compile the builder with -DDBUILD_MAX_COMPILER_EXTRA_ARGS=amount.\n");
                    exit(EXIT_FAILURE);
                }
                CTX.compiler_extra_args[ CTX.compiler_extra_args_count++ ] = dstr_create_from_cstr(argv[ ++i ], &allocator);
            }
            else if (STREQ(arg, "-lf")        || STREQ(arg, "--linker-flag")          )
            {
                ASSURE_NOT_LAST(i, argc, "-lf | --linker-flag needs additional arg with the actual flag to pass.\n");
                if (CTX.linker_extra_args_count >= DBUILD_MAX_LINKER_EXTRA_ARGS) 
                {
                    printf("ERROR: You compile the builder with a limit of %d extra argss to the compiler/linker, and You passed too much.\n", DBUILD_MAX_LINKER_EXTRA_ARGS);
                    printf("\tNOTE: in order to chnge the max compiler/linker args You can compile the builder with -DDBUILD_MAX_LINKER_EXTRA_ARGS=amount.\n");
                    exit(EXIT_FAILURE);
                }
                CTX.linker_extra_args[ CTX.linker_extra_args_count++ ] = dstr_create_from_cstr(argv[ ++i ], &allocator);
            }
            else if (STREQ(arg, "-src")       || STREQ(arg, "--source-directory")     )
            {
                ASSURE_NOT_LAST(i, argc, "-src | --source-directory needs additional arg with the name of the source directory.\n");
                u32 name_len = strlen(argv[ ++i ]);
                if (name_len >= MAX_PATH)
                {
                    printf("ERROR: the name of the required source directory is greater then %d which is not allowed.\n", MAX_PATH);
                    exit(EXIT_FAILURE);
                }
                memcpy(CTX.src_folder_name, argv[ i ], name_len);
                CTX.src_folder_name[ name_len ]= '\0';
            }
            else if (STREQ(arg, "-bld")       || STREQ(arg, "--build-directory")      )
            {
                ASSURE_NOT_LAST(i, argc, "-bld | --build-directory needs additional arg with the name of the build directory.\n");
                u32 name_len = strlen(argv[ ++i ]);
                if (name_len >= MAX_PATH)
                {
                    printf("ERROR: the name of the required build directory is greater then %d which is not allowed.\n", MAX_PATH);
                    exit(EXIT_FAILURE);
                }
                memcpy(CTX.build_folder_name, argv[ i ], name_len);
                CTX.build_folder_name[ name_len ]= '\0';
            }
            else if (STREQ(arg, "-ff")        || STREQ(arg, "--force-file")           )
            {
                ASSURE_NOT_LAST(i, argc, "-ff | --force-file needs additional arg with the actual file to force compilation on.\n");
                if (dhash_table_count(CTX.forced_files_table) >= DBUILD_MAX_FORCED_FILES) 
                {
                    printf("WARNING: You compile the builder with a limit of %d forced files,"
                            " and You passed too much. the builder will not crush but will clean the build directory.\n", DBUILD_MAX_FORCED_FILES);
                }
                dstr_t file_path = dstr_create_from_cstr(argv[ ++i ], &allocator);
                dhash_table_add(CTX.forced_files_table, file_path, file_path);
            }
            else if (STREQ(arg, "-I")         || STREQ(arg, "--include-directory")   )
            {
                ASSURE_NOT_LAST(i, argc, "-I | --include-directory needs additional arg with the actual path to the include directory.\n");
                if (CTX.include_dirs_count >= DBUILD_MAX_INCLUDE_DIRECTORIES) 
                {
                    printf("ERROR: You compile the builder with a limit of %d include directories, and You passed too much.\n", DBUILD_MAX_INCLUDE_DIRECTORIES);
                    exit(EXIT_FAILURE);
                }
                CTX.include_dirs[ CTX.include_dirs_count++ ] = dstr_create_from_cstr(argv[ ++i ], &allocator);
            }
            else if (STREQ(arg, "-L")         || STREQ(arg, "--link-directory")      )
            {
                ASSURE_NOT_LAST(i, argc, "-L | --link-directrory needs additional arg with the actual path to the link directory.\n");
                if (CTX.include_libs_dir_count >= DBUILD_MAX_LIBS_INCLUDE_DIRECTORIES) 
                {
                    printf("ERROR: You compile the builder with a limit of %d include link directories, and You passed too much.\n", DBUILD_MAX_LIBS_INCLUDE_DIRECTORIES);
                    exit(EXIT_FAILURE);
                }
                CTX.include_libs_dir[ CTX.include_libs_dir_count++ ] = dstr_create_from_cstr(argv[ ++i ], &allocator);
            }
            else if (STREQ(arg, "-l")         || STREQ(arg, "--link-library")         )
            {
                ASSURE_NOT_LAST(i, argc, "-l | --link-library needs additional arg with the actual name of the library.\n");
                if (CTX.include_libs_count >= DBUILD_MAX_LIBS) 
                {
                    printf("ERROR: You compile the builder with a limit of %d include link libraries, and You passed too much.\n", DBUILD_MAX_LIBS);
                    exit(EXIT_FAILURE);
                }
                CTX.include_libs[ CTX.include_libs_count++ ] = dstr_create_from_cstr(argv[ ++i ], &allocator);
            }
            else if (STREQ(arg, "-pr")        || STREQ(arg, "--pre-build")            )
            {
                ASSURE_NOT_LAST(i, argc, "-pr | --pre-build should be followed with command string, but you passed None.\n");
                if (CTX.has_pre_cmd)
                {
                    printf("WARNING: You passed two commands as the pre build command, the first is ignored.\n");
                }
                CTX.has_pre_cmd = true;
                CTX.pre_cmd = dstr_create_from_cstr(argv[ ++i ], &allocator);
            }
            else if (STREQ(arg, "-po")        || STREQ(arg, "--post-build")           )
            {
                ASSURE_NOT_LAST(i, argc, "-po | --post-build should be followed with command string, but you passed None.\n");
                if (CTX.has_post_cmd)
                {
                    printf("WARNING: You passed two commands as the post build command, the first is ignored.\n");
                }
                CTX.has_post_cmd = true;
                CTX.post_cmd = dstr_create_from_cstr(argv[ ++i ], &allocator);
            }
        }
        else
        {
            // Warning only - No crash on unrecognized args.
            static int NOT = 0;
            if (i < argc) printf("WARNING: (parsing user arguments), something went wrong parsing your args, at: '%s'\n\n", argv[ i ]);
            if (!NOT) print_help();
            NOT++;
        }
    }
    return true;
}

static inline void* allocate ( u64 size )
{
    if (dbuilder_mem_off + size >= DBUILD_MEMORY_SIZE)
    {
        fprintf(
            stderr,
            "ERROR: (Out-Of-Memory) builder needs to allocate more then %llu.\n"
            "You can compile the builder again with -DDBUILD_MEMORY_SIZE=...\n",
            DBUILD_MEMORY_SIZE
        );
        exit(EXIT_FAILURE);
    }

    void* b = (u8*)DBUILDER_MEMORY + dbuilder_mem_off;
    dbuilder_mem_off += size;

    return b;
}

static inline void* reallocate( void* block, u64 size )
{
    if(dbuilder_mem_off + size >= DBUILD_MEMORY_SIZE)
    {
        fprintf(
            stderr,
            "ERROR: Out-Of-Memory builder needs to allocate more then %llu.\n"
            "You can compile the builder again with -DDBUILD_MEMORY_SIZE=...\n",
            DBUILD_MEMORY_SIZE
        );
        exit(EXIT_FAILURE);
    }

    if (!block) return NULL;
    
    void* b = (u8*)DBUILDER_MEMORY + dbuilder_mem_off;
    dbuilder_mem_off += size;
    
    memcpy(b, block, size);
    
    return b;
}

static inline void deallocate( void* block ) 
{ 
    (void)block; // No deallocation needed!
}

static inline dstr_t dstr_create_from_path( char *p )
{
    if (!p) return NULL;

    u64 len = strlen( p );

    if (len >= MAX_PATH) return NULL;

    char res [ MAX_PATH ] = { 0 };

    u32 idx = 0;
    for (u64 i = 0; i < len; i++)
    {
        if (p[ i ] == '\\') res[ idx++ ] = '/';
        else if (i < len - 1 && p[ i ] == '.' && p[ i + 1 ] == '.')
        {
            i += 1;
            idx -= 2;
            while (idx > 0 && res[ idx ] != '/') idx--;
        }
        else
        {
            res[ idx++ ] = p[ i ];
        }
    }
    
    res[ idx ] = '\0';

    return dstr_create_from_cstr(res, &allocator);
}

static inline void create_obj_file_name_from_path( char *path, char *out_file_name )
{
    if (!path || !out_file_name) return;

    u32 idx = 0;

    for (char* p = path; *p; p++) 
        out_file_name[ idx++ ] = *p == '/' ? '_' : *p;

    out_file_name[ idx - 2 ] ='.';
    out_file_name[ idx - 1 ] ='o';
    out_file_name[ idx ] ='\0';
}

static inline bool str_is_suffix( const char* str, const char* suff ) 
{
    if (!str || !suff) return false;

    u32 str_len = strlen(str);
    u32 suff_len = strlen(suff);
    
    if (str_len < suff_len) return false;

    for (u32 i = 1; i <= suff_len; i++)
        if (str[ str_len - i ] != suff[ suff_len - i ]) return false;

    return true;
}

static inline bool str_eq_with_except( const char *s1, const char *s2, char e1, char e2 )
{
    if (!s1 || !s2) return false;

    u32 l1 = strlen(s1);
    u32 l2 = strlen(s2);

    if (l1 != l2) return false;

    for (u32 i = 0; i < l1; i++)
    {
        if (s1[i] != s2[i] && ((s1[i] != e1 && s1[i] != e2) || (s2[i] != e1 && s2[i] != e2))) return false;
    }

    return true;
}

static inline void str_replace( char *str, char to_replace, char with )
{
    if (!str) return;
    for (char *p = str; *p; p++) if (*p == to_replace) *p = with;
}

#endif
