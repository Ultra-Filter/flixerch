#include <windows.h>
#include <tchar.h> 
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <shlwapi.h>

/*
    Manual: 
        This source file is a stand alone build system when you compile it you can use the executable as follows.
        Suppose you work in directory DIR with subdirectories 'src', 'build' and you have this file executable as 'builder.exe'.
        Then you can use the following args to activate the wanted configuration.
        
        .\builder.exe -arg will use the arg as input to the compiler when compiling source files to objects file. TODO:
        .\builder.exe +bd will build your source files to the folder 'build/debug' and crate an executable with debug config. 
        .\builder.exe +br will build your source files to the folder 'build/release' and crate an executable with release config. 
        .\builder.exe +i will print info as it run.
        .\builder.exe +c will clean the current build type folder (a.k.a release\debug)
        .\builedr.exe +r will run the executable in the build\build_type folder when he finish compiling.
        .\builder.exe +sfile.c will override the last compilation of file.o and will replace it with the new one.

        When using those args you can shuffle them as you wish (order does NOT matter).

        If you wish to use this file as your builder then you need to do at least the following requirements:
            1) Edit all the defines below so the builder will know the compiler path and libs you use.
            2) Make sure to change also the SOURCE_FILE_DIRECTORY macro to point to the right src folder.
            3) If you have a LOT of files then you probobly need to adjust the amount of memory chuncks i use. (Don't over-do it)
            4) If any assert happand when runing then probebly some folder or path does not exists or you are out of memory.

        NOTE: This file does NOT allocating memory on the heap at all - this is performance critical.


        SUGGESTIONS:
            1) If you work on an IDE which allow you to set tasks (Most of them do) 
                then you dan set multiple tasks with different arguments to the builder. (Even better activate the builder on each save and habe the executable ready for you).
            2) You can allways add to this code some way to search for the newest compiler + add some threads for MAX performance. (I was lasy).
            3) You can compiler this builder with: Path/To/Compiler/gcc.exe -o builder build.c -Wall -Wextra -Werror -o3
            
        TODO:
            We have a very cool bug here. 
            Suppose we have the following case:
                main.c uses a function called func
                and for that main.c includes header.h which declares func(); and implment it in header.c ...
                Now suppose the user changes the name of the function to func1... 
                because main.c uses the function func then if ew compile it we get 'implicit decleration' (compilation error)
                BUT what happened if the file main.c did not changed?? the linker whould not notice the bug and since the file
                already compiled the compiler wont tell you that you tried to implicit declare 'func'...
            Solution:
                The solution for now is to clean and rebuild the whole thing...
                Later we can think about dependecy graph and how to check who needs re-compilation...
*/


#define __maybe_unused __attribute__((unused))

#ifndef COMPILER_PATH
    #define COMPILER_PATH               L"gcc "
#endif
#ifndef LINKER_FLAGS
    #define LINKER_FLAGS                L"-lglfw3 -lgdi32 -lopengl32 -lcurl -liphlpapi -lssl -lcrypto -lWs2_32 -lnghttp2 -lnghttp3 -lpsl -lzstd -lssh2 -lz -lngtcp2 -lbrotlicommon -lbrotlidec -lsecur32 -lwldap32 -lbcrypt -lcrypt32 -lngtcp2_crypto_libressl -lbrotlidec -lbrotlicommon"
#endif
#ifndef COMPILER_FLAGS 
    #define COMPILER_FLAGS              L"-Wall -Wextra -Werror -Wno-int-to-pointer-cast -Wno-unused-function"
#endif
#ifndef LIBS_FLAGS_PATH 
    #define LIBS_FLAGS_PATH             L"-Ldeps/libs"
#endif
#ifndef LIBS_INCLUDE_PATH 
    #define LIBS_INCLUDE_PATH           L"-Ideps/include -Isrc "
#endif
#ifndef DEBUG_DEFINITIONS 
    #define DEBUG_DEFINITIONS           L"-D_DEBUG -DUNICODE -D_UNICODE "
#endif
#ifndef RELEASE_DEFINITIONS 
    #define RELEASE_DEFINITIONS         L"-DUNICODE -D_UNICODE"
#endif
    

#ifndef SOURCE_FILES_DIRECTORY
    #define SOURCE_FILES_DIRECTORY                 L"src"
#endif
#define SOURCE_FILES_DIRECTORY_ALL             SOURCE_FILES_DIRECTORY L"\\*"
#ifndef BUILD_DIRECTORY
    #define BUILD_DIRECTORY                        L"build"
#endif
#define BUILD_DIRECTORY_ALL                    BUILD_DIRECTORY L"\\*"
#define BUILD_DEBUG_DIRECTORY                  BUILD_DIRECTORY L"\\debug"
#define BUILD_DEBUG_DIRECTORY_ALL              BUILD_DEBUG_DIRECTORY L"\\*"
#define BUILD_RELEASE_DIRECTORY                BUILD_DIRECTORY L"\\release"
#define BUILD_RELEASE_DIRECTORY_ALL            BUILD_RELEASE_DIRECTORY L"\\*"
#define BUILD_DEBUG_EXISTING_OBJECTS_DIR       BUILD_DEBUG_DIRECTORY L"\\object_files"
#define BUILD_DEBUG_EXISTING_OBJECTS_DIR_ALL   BUILD_DEBUG_EXISTING_OBJECTS_DIR L"\\*"
#define BUILD_RELEASE_EXISTING_OBJECTS_DIR     BUILD_RELEASE_DIRECTORY L"\\object_files"
#define BUILD_RELEASE_EXISTING_OBJECTS_DIR_ALL BUILD_RELEASE_EXISTING_OBJECTS_DIR L"\\*"
#define BUILD_DEBUG_EXISTING_OBJECTS_FILE      BUILD_DEBUG_DIRECTORY L"\\objects.txt"
#define BUILD_RELEASE_EXISTING_OBJECTS_FILE    BUILD_RELEASE_DIRECTORY L"\\objects.txt"

#define MEMORY_CHUNK_SMALL_SIZE  256
#define MEMORY_CHUNK_SMALL_COUNT 2048

#define MEMORY_CHUNK_NORMAL_SIZE  2048
#define MEMORY_CHUNK_NORMAL_COUNT 128

#define MEMORY_CHUNK_HUGE_SIZE  8192
#define MEMORY_CHUNK_HUGE_COUNT 4

typedef enum memory_type_e
{
    MEMORY_TYPE_SMALL = 0,
    MEMORY_TYPE_NORMAL = 1,
    MEMORY_TYPE_HUGE = 2,
    MEMORY_TYPE_COUNT = 3
} memory_type_e;

typedef struct wstring_t
{
    wchar_t * wchars;
    uint32_t length, capacity;
} wstring_t;

typedef struct files_data_s
{
    uint32_t count, capacity;
    struct file {
        wstring_t file_name;
        wstring_t file_path;
        SYSTEMTIME file_last_write_data;
    } * files;
} files_data_s;

wstring_t wstring__create(memory_type_e type);
void      wstring__append_wchar(wstring_t * str, const wchar_t c);
void      wstring__append_wchars(wstring_t * str, const wchar_t * cs, const uint32_t count);
void      wstring__append_char(wstring_t * str, const char c);
void      wstring__append_chars(wstring_t * str, const char * cs, const uint32_t count);
void      wstring__append_wstring(wstring_t * first, const wstring_t second);
void      wstring__get_word_by_delim(const wstring_t str, const wchar_t delim, wstring_t * word);
uint32_t  wstring__get_word_by_delim_pattern(const wstring_t str, const wchar_t* delim_pattern, const uint32_t count, wstring_t * word);
uint32_t  wstring__get_word_by_delim_pattern_index(
    const wstring_t str, 
    const uint32_t start_index, 
    const wchar_t* delim_pattern, 
    const uint32_t count, 
    wstring_t * word
);
uint32_t  wstring__get_word_by_delim2_pattern_index(
    const wstring_t str, 
    const uint32_t start_index, 
    const wchar_t* delim_pattern1, 
    const uint32_t count1, 
    const wchar_t* delim_pattern2, 
    const uint32_t count2, 
    wstring_t * word
);
bool wstring__compare(const wstring_t str1, const wstring_t str2);

files_data_s files__create(memory_type_e type);
void         files___existing_files__read(files_data_s * existing_files);
void         files___existing_files__update(files_data_s updated_files);
void         files___append_file(files_data_s * files, wstring_t file_name, wstring_t file_path, SYSTEMTIME file_last_wrirte_time);

void *        memory___alloc(memory_type_e type);
void *        memory___realloc(void* block, memory_type_e type);
void          memory___free(void * block);
uint32_t      memory___type_to_size(memory_type_e type);
memory_type_e memory___size_to_type(uint32_t size);

void builder___parse_args(int argc, char** argv);

void directory____files_and_dirs__generate(const wchar_t * path, wstring_t * out_dirs, files_data_s * out_files);

void cmd___object_file__compile(wstring_t source_file_name, wstring_t source_file_path);
void cmd___object_file__link(void);
void cmd___executable__run(void);

bool SYSTEMTIME__compare(SYSTEMTIME t1, SYSTEMTIME t2);
int build(int argc, char ** argv);

// #ifdef BUILD_IMPLEMENTATION 
#if 1
#undef BUILD_IMPLEMENTATION

static struct MEMORY_CHUNK_SMALL
{
    memory_type_e type;
    char memory [ MEMORY_CHUNK_SMALL_SIZE ];
    bool occupied;
} small_memories [ MEMORY_CHUNK_SMALL_COUNT ];

static struct MEMORY_CHUNK_NORMAL
{
    memory_type_e type;
    char memory [ MEMORY_CHUNK_NORMAL_SIZE ];
    bool occupied;
} normal_memories [ MEMORY_CHUNK_NORMAL_COUNT ];

static struct MEMORY_CHUNK_HUGE
{
    memory_type_e type;
    char memory [ MEMORY_CHUNK_HUGE_SIZE ];
    bool occupied;
} huge_memories [ MEMORY_CHUNK_HUGE_COUNT ];

static struct {
    bool clean_build;
    bool should_run; 
    bool should_print;
    enum { 
        BUILD_TYPE_DEBUG, 
        BUILD_TYPE_RELEASE 
    } build_type;
    wchar_t* existing_object_files_dir_path;
    wchar_t* existing_object_files_file_path;
    wstring_t requested_compiler_args;
    wstring_t override_objects;
    wstring_t include_dirs;
    files_data_s files_data;
    files_data_s existing_files_data;
    files_data_s updated_files_data;
    wstring_t object_compilation_command;
    bool executable_created_successfully;
} build_info_s;

int build(int argc, char** argv)
{    
    build_info_s.override_objects        = wstring__create(MEMORY_TYPE_SMALL);
    build_info_s.requested_compiler_args = wstring__create(MEMORY_TYPE_SMALL);
    build_info_s.include_dirs            = wstring__create(MEMORY_TYPE_NORMAL);
    build_info_s.files_data              = files__create(MEMORY_TYPE_NORMAL);
    build_info_s.existing_files_data     = files__create(MEMORY_TYPE_NORMAL);
    build_info_s.updated_files_data      = files__create(MEMORY_TYPE_NORMAL);

    builder___parse_args(argc, argv);
    
    if (build_info_s.should_print)
    {
        printf("Scaning directory: %ls\n", SOURCE_FILES_DIRECTORY_ALL);
    }

    directory____files_and_dirs__generate(
            SOURCE_FILES_DIRECTORY_ALL, 
            &build_info_s.include_dirs, 
            &build_info_s.files_data
        );

    if (build_info_s.clean_build)
    {
        if (build_info_s.should_print)
        {
            printf("Cleaning build\\%s folder!\n", build_info_s.build_type == BUILD_TYPE_DEBUG ? "debug" : "release");
        }

        for (uint32_t fi = 0; fi < build_info_s.files_data.count; fi++)
        {
            cmd___object_file__compile(
                build_info_s.files_data.files[fi].file_name, 
                build_info_s.files_data.files[fi].file_path
            );

            files___append_file(
                &build_info_s.updated_files_data,
                build_info_s.files_data.files[fi].file_name,
                (wstring_t){ 0 },
                build_info_s.files_data.files[fi].file_last_write_data
            );
        }
    }
    else
    {
        if (build_info_s.should_print)
        {
            printf("Checking files for compilation\n");
        }

        files___existing_files__read(&build_info_s.existing_files_data);
    
        for (uint32_t fi = 0; fi < build_info_s.files_data.count; fi++)
        {
            bool need_compilation = true;
    
            for (uint32_t efi = 0; efi < build_info_s.existing_files_data.count; efi++)
            {
                bool is_file_exist = wstring__compare(
                    build_info_s.files_data.files[fi].file_name, 
                    build_info_s.existing_files_data.files[efi].file_name
                );
    
                if (is_file_exist)
                {
                    bool is_file_up_to_date = SYSTEMTIME__compare(
                        build_info_s.files_data.files[fi].file_last_write_data,
                        build_info_s.existing_files_data.files[efi].file_last_write_data
                    );
    
                    if (is_file_up_to_date)
                    {
                        need_compilation = false;
                    }
                } 
            }
            
            if (need_compilation)
            {
                if (build_info_s.should_print)
                {
                    printf("\tCompiling file: %ls to object file.\n", build_info_s.files_data.files[fi].file_name.wchars);
                }

                cmd___object_file__compile(
                    build_info_s.files_data.files[fi].file_name, 
                    build_info_s.files_data.files[fi].file_path
                );
            }
            else
            {
                if (build_info_s.should_print)
                {
                    printf("\tSkipping file: %ls, this file has no changes.\n", build_info_s.files_data.files[fi].file_name.wchars);
                }
            }
    
            files___append_file(
                &build_info_s.updated_files_data,
                build_info_s.files_data.files[fi].file_name,
                (wstring_t){ 0 },
                build_info_s.files_data.files[fi].file_last_write_data
            );
        }
    }

    cmd___object_file__link();
    
    if (build_info_s.should_run)
    {
        cmd___executable__run();
    }
    
    files___existing_files__update(build_info_s.updated_files_data);

    return 0;
}

void builder___parse_args(int argc, char** argv)
{
    for (char ** ptr = argv; ptr < argv + argc; ptr++)
    {
        if (*ptr[0] == '-')
        {
            // TODO: use those in compilation.
            wstring__append_chars(
                &build_info_s.requested_compiler_args, 
                *ptr, 
                strlen(*ptr)
            );
        }
        else if (*ptr[0] == '+')
        {
            if (strlen(*ptr) <= 1)
            {
                continue;
            }

            if ((*ptr)[1] == 'c')
            {
                build_info_s.clean_build = true;
            } 
            else if ((*ptr)[1] == 'r')
            {
                build_info_s.should_run = true;
            } 
            else if ((*ptr)[1] =='s')
            {
                wstring__append_chars(
                    &build_info_s.override_objects, 
                    " \0", 
                    1
                );
                wstring__append_chars(
                    &build_info_s.override_objects, 
                    (*ptr) + 2, 
                    strlen((*ptr) + 2)
                );
            }
            else if (strlen(*ptr) >= 3)
            {
                if ((*ptr)[1] == 'b' && (*ptr)[2] == 'r')
                {
                    build_info_s.build_type = BUILD_TYPE_RELEASE;
                } 
                else if ((*ptr)[1] == 'b' && (*ptr)[2] == 'd')
                {
                    build_info_s.build_type = BUILD_TYPE_DEBUG;
                }
            }
            else if ((*ptr)[1] == 'i')
            {
                build_info_s.should_print = true;
            } 
        }
    }

    if (build_info_s.build_type == BUILD_TYPE_RELEASE)
    {
        build_info_s.existing_object_files_dir_path = BUILD_RELEASE_EXISTING_OBJECTS_DIR;
        build_info_s.existing_object_files_file_path = BUILD_RELEASE_EXISTING_OBJECTS_FILE;
    }
    else
    {
        build_info_s.existing_object_files_dir_path = BUILD_DEBUG_EXISTING_OBJECTS_DIR;
        build_info_s.existing_object_files_file_path = BUILD_DEBUG_EXISTING_OBJECTS_FILE;
    }
}

void directory____files_and_dirs__generate(const wchar_t *path, wstring_t *out_dirs, files_data_s *out_files)
{
    WIN32_FIND_DATAW data;
    HANDLE handle = FindFirstFileW(path, &data);

    if (handle != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (data.cFileName[0] == (wchar_t)'.') continue; // Ignore '..' and '.'

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                wchar_t dir_name [ 260 ];
                uint32_t dir_name_length = 0;
                for (uint32_t i = 0; i < 260; i++)
                {
                    dir_name[dir_name_length++] = path[i];
                    if (path[i] == (wchar_t)'\0') break;
                }
                dir_name_length -= 2; // remove the '*\0' 
                for (uint32_t i = 0; i < 260; i++)
                {
                    if (data.cFileName[i] == (wchar_t)'\0') break;
                    dir_name[dir_name_length++] = data.cFileName[i];
                }
                
                if (build_info_s.should_print)
                {
                    printf("\tScanning Directory '%ls'\n", dir_name);
                }

                // wstring__append_wchars(out_dirs, L" -I", 3);
                // wstring__append_wchars(out_dirs, dir_name, dir_name_length);
                
                dir_name[dir_name_length++] = (wchar_t)'\\';
                dir_name[dir_name_length++] = (wchar_t)'*';
                dir_name[dir_name_length++] = (wchar_t)'\0';
                directory____files_and_dirs__generate(dir_name, out_dirs, out_files);
            }
            else
            {
                bool ok = false;

                for (size_t i = 2; i < 260; i++)
                {
                    wchar_t a = data.cFileName[i-2];
                    wchar_t b = data.cFileName[i-1];
                    wchar_t c = data.cFileName[i];


                    if (c == (wchar_t)'\0' && b == (wchar_t)'c' && a == (wchar_t)'.')
                    {
                        ok = true;
                        break;
                    }
                }
                if (!ok) continue;

                wchar_t dir_name [ 260 ];
                uint32_t dir_name_length = 0;
                for (uint32_t i = 0; i < 260; i++)
                {
                    dir_name[dir_name_length++] = path[i];
                    if (path[i] == (wchar_t)'\0') break;
                }
                dir_name_length -= 2;
                
                wstring_t file_name = wstring__create(MEMORY_TYPE_SMALL);
                wstring_t file_path = wstring__create(MEMORY_TYPE_SMALL);

                wstring__append_wchars(&file_path, dir_name, dir_name_length);

                for (uint32_t i = 0; i < 260; i++)
                {
                    wstring__append_wchar(&file_name, data.cFileName[i]);
                    wstring__append_wchar(&file_path, data.cFileName[i]);
                    if (data.cFileName[i] == (wchar_t)'\0') break;
                }

                SYSTEMTIME st;
                FileTimeToSystemTime(&data.ftLastWriteTime, &st);

                if (build_info_s.should_print)
                {
                    printf("\tFile found: '%ls' at '%ls'\n", file_name.wchars, path);
                }

                files___append_file(out_files, file_name, file_path, st);
            }
        } while (FindNextFileW(handle, &data));
        CloseHandle(handle);
    }
}


void cmd___object_file__compile(wstring_t source_file_name, wstring_t source_file_path)
{     
    wstring_t command = wstring__create(MEMORY_TYPE_NORMAL);

    wstring__append_wchars(&command, L"call ", 5);
    wstring__append_wchars(
        &command, 
        COMPILER_PATH, 
        sizeof(COMPILER_PATH) / sizeof(wchar_t) - 1
    );
    wstring__append_wchars(&command, L" -o ", 4);
    if (build_info_s.build_type == BUILD_TYPE_DEBUG)
    {
        wstring__append_wchars(
            &command, 
            BUILD_DEBUG_EXISTING_OBJECTS_DIR, 
            sizeof(BUILD_DEBUG_EXISTING_OBJECTS_DIR) / sizeof(wchar_t) - 1
        );
        CreateDirectoryW(BUILD_DEBUG_DIRECTORY, NULL);
        CreateDirectoryW(BUILD_DEBUG_EXISTING_OBJECTS_DIR, NULL);
    } 
    else
    {
        wstring__append_wchars(
            &command, 
            BUILD_RELEASE_EXISTING_OBJECTS_DIR, 
            sizeof(BUILD_RELEASE_EXISTING_OBJECTS_DIR) / sizeof(wchar_t) -1
        );
        CreateDirectoryW(BUILD_RELEASE_DIRECTORY, NULL);
        CreateDirectoryW(BUILD_RELEASE_EXISTING_OBJECTS_DIR, NULL);
    }
    wstring__append_wchars(&command, L"\\", 1);
    wstring__append_wchars(&command, source_file_name.wchars, source_file_name.length - 3);
    wstring__append_wchars(&command, L".o ", 3);
    
    wstring__append_wchars(&command, source_file_path.wchars, source_file_path.length - 1);
    wstring__append_char(&command, ' ');

    if (build_info_s.build_type == BUILD_TYPE_DEBUG)
    {
        wstring__append_wchars(&command, L"-g ", 3);
    }
    else
    {
        //wstring__append_wchars(&command, L"-o3 ", 4);
    }
    wstring__append_wchars(&command, L"-c ", 3);
    if (build_info_s.build_type == BUILD_TYPE_DEBUG)
    {
        wstring__append_wchars(&command, COMPILER_FLAGS, sizeof(COMPILER_FLAGS) / sizeof(wchar_t) - 1);
        wstring__append_char(&command, ' ');
    }
    else 
    {
        wstring_t no_warning_flags = wstring__create(MEMORY_TYPE_SMALL);
        const wchar_t * warn = L"-Wno-int-to-pointer-cast  ";
        wstring__append_wchars(&no_warning_flags, warn, wcslen(warn));
        wstring__append_wchars(&command, no_warning_flags.wchars, no_warning_flags.length - 1);
    }
    wstring__append_wchars(&command, LIBS_INCLUDE_PATH, sizeof(LIBS_INCLUDE_PATH) / sizeof(wchar_t) - 1);
    // wstring__append_char(&command, ' ');
    // wstring__append_wstring(&command, build_info_s.include_dirs);
    // wstring__append_char(&command, ' ');


    if (build_info_s.build_type == BUILD_TYPE_DEBUG)
    {
        wstring__append_wchars(&command, DEBUG_DEFINITIONS, sizeof(DEBUG_DEFINITIONS) / sizeof(wchar_t) - 1);
    }
    else
    {
        wstring__append_wchars(&command, RELEASE_DEFINITIONS, sizeof(RELEASE_DEFINITIONS) / sizeof(wchar_t) - 1);
        wstring__append_wchars(&command, L" -mwindows", sizeof(L" -mwindows") / sizeof(wchar_t) - 1);
    }

    _wsystem(command.wchars);

    if (build_info_s.should_print)
    {
        printf("Command [Compilation of '%ls']\n\t%ls\n", source_file_name.wchars, command.wchars);
    }

    memory___free(command.wchars);
}

void cmd___object_file__link(void)
{
    wstring_t command = wstring__create(MEMORY_TYPE_NORMAL);

    wstring__append_wchars(&command, L"call ", 5);
    wstring__append_wchars(
        &command, 
        COMPILER_PATH, 
        sizeof(COMPILER_PATH) / sizeof(wchar_t) - 1
    );
    wstring__append_wchars(&command, L" -o ", 4);
    if (build_info_s.build_type == BUILD_TYPE_DEBUG)
    {
        wstring__append_wchars(
            &command, 
            BUILD_DEBUG_DIRECTORY_ALL, 
            sizeof(BUILD_DEBUG_DIRECTORY) / sizeof(wchar_t) - 1
        );
    }
    else
    {
        wstring__append_wchars(
            &command, 
            BUILD_RELEASE_DIRECTORY_ALL, 
            sizeof(BUILD_RELEASE_DIRECTORY) / sizeof(wchar_t) - 1
        );
    }
    wstring__append_wchars(&command, L"\\main.exe ", 10);
    
    
    
    WIN32_FIND_DATAW data;
    HANDLE handle = FindFirstFileW(
        (build_info_s.build_type == BUILD_TYPE_DEBUG) ? 
        BUILD_DEBUG_EXISTING_OBJECTS_DIR_ALL : BUILD_RELEASE_EXISTING_OBJECTS_DIR_ALL,
        &data
    );
    
    do
    {
        wchar_t f_name [ 260 ];
        uint32_t f_name_len = 0;
        if ((build_info_s.build_type == BUILD_TYPE_DEBUG))
        {
            for (uint32_t i = 0; i < sizeof(BUILD_DEBUG_EXISTING_OBJECTS_DIR) / sizeof(wchar_t) - 1; i++)
            {
                f_name[f_name_len++] = (BUILD_DEBUG_EXISTING_OBJECTS_DIR)[i];
            }
        }
        else
        {
            for (uint32_t i = 0; i < sizeof(BUILD_RELEASE_EXISTING_OBJECTS_DIR) / sizeof(wchar_t) - 1; i++)
            {
                f_name[f_name_len++] = (BUILD_RELEASE_EXISTING_OBJECTS_DIR)[i];
            }
        }
            
        f_name[f_name_len++] = (wchar_t)'\\';
        f_name[f_name_len++] = data.cFileName[0];
        f_name[f_name_len++] = data.cFileName[1];
        for (uint32_t i = 2; i < 260; i++)
        {
            wchar_t a,b,c;
            a = data.cFileName[i - 2];
            b = data.cFileName[i - 1];
            c = data.cFileName[i];
            f_name[f_name_len++] = data.cFileName[i]; 
            
            if (a == (wchar_t)'.' && b == (wchar_t)'o' && c == (wchar_t)'\0')
            {
                wstring__append_wchars(&command, f_name, f_name_len - 1);
                wstring__append_char(&command, ' ');
                break;
            }
            else if(c == (wchar_t)'\0')
            {
                break;
            }
        }
    } while (FindNextFileW(handle, &data));
    CloseHandle(handle);
    wstring__append_char(&command, ' ');
    wstring__append_wchars(&command, LIBS_FLAGS_PATH, sizeof(LIBS_FLAGS_PATH) / sizeof(wchar_t) - 1);
    wstring__append_char(&command, ' ');
    wstring__append_wchars(&command, LINKER_FLAGS, sizeof(LINKER_FLAGS) / sizeof(wchar_t) - 1);
    if (build_info_s.build_type == BUILD_TYPE_RELEASE)
    {
        wstring__append_wchars(&command, L" -mwindows", sizeof(L" -mwindows") / sizeof(wchar_t) - 1);
    }

    int result = _wsystem(command.wchars);
    if (result == 0) build_info_s.executable_created_successfully = true;

    if (build_info_s.should_print)
    {
        printf("Command (linking)\n\t%ls\n", command.wchars);
        printf("Compilation was %s, executable %s.\n", 
            result == 0 ? "successfull" : "not successfull", result == 0 ? "created" : "did not created");
    }

    memory___free(command.wchars);
}

void cmd___executable__run(void)
{
    if (build_info_s.executable_created_successfully)
    {
        if (build_info_s.build_type == BUILD_TYPE_DEBUG)
        {
            _wsystem(L".\\" BUILD_DEBUG_DIRECTORY L"\\main.exe");
        }
        else
        {
            _wsystem(L".\\" BUILD_RELEASE_DIRECTORY L"\\main.exe");
        }
    }
}

bool SYSTEMTIME__compare(SYSTEMTIME t1, SYSTEMTIME t2)
{
    bool same_Year = t1.wYear == t2.wYear;
    bool same_Month = t1.wMonth == t2.wMonth;
    bool same_Day = t1.wDay == t2.wDay;
    bool same_Hour = t1.wHour == t2.wHour;
    bool same_Minute = t1.wMinute == t2.wMinute;
    bool same_Second = t1.wSecond == t2.wSecond;
    bool same_Milliseconds = t1.wMilliseconds == t2.wMilliseconds;

    return same_Year && same_Month && same_Day && same_Hour && same_Minute && same_Second && same_Milliseconds;
}

void * memory___alloc(memory_type_e type)
{
    switch (type)
    {
        case MEMORY_TYPE_SMALL:
        {
            static uint32_t small_memory_index = 0;
            if (!small_memories[small_memory_index].occupied)
            {
                small_memories[small_memory_index].type = MEMORY_TYPE_SMALL;
                small_memories[small_memory_index].occupied = true;
                void * block = (void * )small_memories[small_memory_index].memory;
                small_memory_index++;
                return block;
            }
            else
            {
                for (uint32_t i = 0; i < MEMORY_CHUNK_SMALL_COUNT; i++)
                {
                    if (!small_memories[i].occupied)
                    {
                        small_memories[i].type = MEMORY_TYPE_SMALL;
                        small_memories[i].occupied = true;
                        void * block = (void * )small_memories[i].memory;
                        return block;
                    }
                }
        
                assert(0 && "You are out of memories! consider updating the MEMORY_CHUNK_SMALL_COUNT value.");
            }
            break;
        }
        case MEMORY_TYPE_NORMAL:
        {
            static uint32_t normal_memory_index = 0;
            if (!normal_memories[normal_memory_index].occupied)
            {
                normal_memories[normal_memory_index].type = MEMORY_TYPE_NORMAL;
                normal_memories[normal_memory_index].occupied = true;
                void * block = (void * )normal_memories[normal_memory_index].memory;
                normal_memory_index++;
                return block;
            }
            else
            {
                for (uint32_t i = 0; i < MEMORY_CHUNK_NORMAL_COUNT; i++)
                {
                    if (!normal_memories[i].occupied)
                    {
                        normal_memories[i].type = MEMORY_TYPE_NORMAL;
                        normal_memories[i].occupied = true;
                        void * block = (void * )normal_memories[i].memory;
                        return block;
                    }
                }
        
                assert(0 && "You are out of memories! consider updating the MEMORY_CHUNK_NORMAL_COUNT value.");
            }
            break;
        }
        case MEMORY_TYPE_HUGE:
        {
            static uint32_t huge_memory_index = 0;
            if (!huge_memories[huge_memory_index].occupied)
            {
                huge_memories[huge_memory_index].type = MEMORY_TYPE_HUGE;
                huge_memories[huge_memory_index].occupied = true;
                void * block = (void * )huge_memories[huge_memory_index].memory;
                huge_memory_index++;
                return block;
            }
            else
            {
                for (uint32_t i = 0; i < MEMORY_CHUNK_HUGE_COUNT; i++)
                {
                    if (!huge_memories[i].occupied)
                    {
                        huge_memories[i].type = MEMORY_TYPE_HUGE;
                        huge_memories[i].occupied = true;
                        void * block = (void * )huge_memories[i].memory;
                        return block;
                    }
                }
        
                assert(0 && "You are out of memories! consider updating the MEMORY_CHUNK_SMALL_COUNT value.");
            }
            break;
        }
        default: break;
    }

    return NULL;
}

void *memory___realloc(void *block, memory_type_e type)
{
    memory_type_e prev_type = *(((char* )block) - sizeof(memory_type_e));
    uint32_t prev_size = memory___type_to_size(prev_type);
    uint32_t new_size= memory___type_to_size(type);
    assert(new_size!= 0);

    char * new_block = (char* )memory___alloc(type);
    for (
        char * ptr_in = new_block, *ptr_out = (char* )block; 
        ptr_in < new_block + new_size && ptr_out < (char* )block + prev_size; 
        *ptr_in++ = *ptr_out++
    );

    memory___free(block);

    return new_block;
}

void memory___free(void * block)
{
    memory_type_e type = *(((char* )block) - sizeof(memory_type_e));

    uint32_t size = memory___type_to_size(type);

    assert(size != 0);
    
    *((bool* )(((char* )block) + size)) = false; // set occupied to false
}

uint32_t memory___type_to_size(memory_type_e type)
{
    uint32_t size = (type == MEMORY_TYPE_SMALL) ? MEMORY_CHUNK_SMALL_SIZE   :
                    (type == MEMORY_TYPE_NORMAL) ? MEMORY_CHUNK_NORMAL_SIZE :
                    (type == MEMORY_TYPE_HUGE) ? MEMORY_CHUNK_HUGE_SIZE     : 0;
    return size;
}

memory_type_e memory___size_to_type(uint32_t size)
{
    if (size <= MEMORY_CHUNK_SMALL_SIZE)
    {
        return MEMORY_TYPE_SMALL;
    }
    else if (size <= MEMORY_CHUNK_NORMAL_SIZE)
    {
        return MEMORY_TYPE_NORMAL;
    }
    else if (size <= MEMORY_CHUNK_HUGE_SIZE)
    {
        return MEMORY_TYPE_HUGE;
    }
    return MEMORY_TYPE_COUNT;
}

wstring_t wstring__create(memory_type_e type)
{
    wstring_t result = { 0 };
    
    result.capacity = memory___type_to_size(type);
    result.length = 0;
    result.wchars = (wchar_t* )memory___alloc(type);

    return result;
}

void wstring__append_wchar(wstring_t *str, const wchar_t c)
{
    if (str->length * sizeof(wchar_t) >= str->capacity)
    {
        memory_type_e mem_type = memory___size_to_type(str->capacity);
        assert(mem_type != MEMORY_TYPE_COUNT && mem_type != MEMORY_TYPE_HUGE);

        memory_type_e next_mem_type = (mem_type == MEMORY_TYPE_SMALL) ? MEMORY_TYPE_NORMAL : MEMORY_TYPE_HUGE;   

        str->capacity = memory___type_to_size(next_mem_type);
        str->wchars = memory___realloc(str->wchars, next_mem_type);
    }

    str->wchars[str->length++] = c;
}

void wstring__append_wchars(wstring_t *str, const wchar_t *cs, const uint32_t count)
{
    if ((str->length + count) * sizeof(wchar_t) >= str->capacity)
    {
        memory_type_e mem_type = memory___size_to_type(str->capacity);
        assert(mem_type != MEMORY_TYPE_COUNT && mem_type != MEMORY_TYPE_HUGE);

        memory_type_e next_mem_type = (mem_type == MEMORY_TYPE_SMALL) ? MEMORY_TYPE_NORMAL : MEMORY_TYPE_HUGE;   
        
        str->capacity = memory___type_to_size(next_mem_type);
        str->wchars = memory___realloc(str->wchars, next_mem_type);
    }

    uint32_t amount_left = count; const wchar_t * cs_ptr = cs;
    while (amount_left--) str->wchars[str->length++] = *cs_ptr++;
}

void wstring__append_char(wstring_t *str, const char c)
{
    wstring__append_wchar(str, (wchar_t)c);
}

void wstring__append_chars(wstring_t *str, const char *cs, const uint32_t count)
{
    if ((str->length + count) * sizeof(wchar_t) >= str->capacity)
    {
        memory_type_e mem_type = memory___size_to_type(str->capacity);
        assert(mem_type != MEMORY_TYPE_COUNT && mem_type != MEMORY_TYPE_HUGE);

        memory_type_e next_mem_type = (mem_type == MEMORY_TYPE_SMALL) ? MEMORY_TYPE_NORMAL : MEMORY_TYPE_HUGE;   
        
        str->capacity = memory___type_to_size(next_mem_type);
        str->wchars = memory___realloc(str->wchars, next_mem_type);
    }

    uint32_t amount_left = count; const char * cs_ptr = cs;
    while (amount_left--) str->wchars[str->length++] = (wchar_t)(*cs_ptr++);
}

void wstring__append_wstring(wstring_t *first, const wstring_t second)
{
    if ((first->length + second.length) * sizeof(wchar_t) >= first->capacity)
    {
        memory_type_e mem_type = memory___size_to_type(first->capacity);
        assert(mem_type != MEMORY_TYPE_COUNT && mem_type != MEMORY_TYPE_HUGE);

        memory_type_e next_mem_type = (mem_type == MEMORY_TYPE_SMALL) ? MEMORY_TYPE_NORMAL : MEMORY_TYPE_HUGE;   
        
        first->capacity = memory___type_to_size(next_mem_type);
        first->wchars = memory___realloc(first->wchars, next_mem_type);
    }

    uint32_t amount_left = second.length;
    for (uint32_t i = 0; i < amount_left; i++)
    {
        first->wchars[first->length++] = second.wchars[i];
    }
}

void wstring__get_word_by_delim(const wstring_t str, const wchar_t delim, wstring_t *word)
{
    for (uint32_t index = 0; index < str.length; index++)
    {
        wstring__append_wchar(word, str.wchars[index]);
        if (str.wchars[index] == delim) break;
    }
}

uint32_t wstring__get_word_by_delim_pattern(const wstring_t str, const wchar_t *delim_pattern, const uint32_t count, wstring_t *word)
{
    uint32_t read_amount = 0;
    for (uint32_t index = 0; index < str.length; index++)
    {
        wstring__append_wchar(word, str.wchars[index]);
        read_amount++;
        if (index + count > str.length) continue;
        
        bool reached_pattern = true;
        for (uint32_t p_index = 0; p_index < count; p_index++)
        {
            if (str.wchars[index + p_index] != delim_pattern[p_index])
            {
                reached_pattern = false;
                break;
            }
        }
        
        if (reached_pattern)
        {
            for (uint32_t p_index = 0; p_index < count; p_index++)
            {
                wstring__append_wchar(word, str.wchars[index + p_index]); 
                read_amount++;  
            }
            break;
        }
    }
    return read_amount;
}

uint32_t wstring__get_word_by_delim_pattern_index(
    const wstring_t str, 
    const uint32_t start_index, 
    const wchar_t *delim_pattern, 
    const uint32_t count, 
    wstring_t *word
)
{
    uint32_t read_amount = 0;
    for (uint32_t index = start_index; index < str.length; index++)
    {
        wstring__append_wchar(word, str.wchars[index]);
        read_amount++;
        if (index + count > str.length) continue;
        
        bool reached_pattern = true;
        for (uint32_t p_index = 0; p_index < count; p_index++)
        {
            if (str.wchars[index + p_index] != delim_pattern[p_index])
            {
                reached_pattern = false;
                break;
            }
        }
        
        if (reached_pattern)
        {
            for (uint32_t p_index = 1; p_index < count; p_index++)
            {
                wstring__append_wchar(word, str.wchars[index + p_index]); 
                read_amount++;  
            }
            break;
        }
    }
    return read_amount;
}

uint32_t wstring__get_word_by_delim2_pattern_index(
    const wstring_t str, 
    const uint32_t start_index, 
    const wchar_t *delim_pattern1, 
    const uint32_t count1, 
    const wchar_t *delim_pattern2, 
    const uint32_t count2, 
    wstring_t *word
)
{
    uint32_t read_amount = 0;
    for (uint32_t index = start_index; index < str.length; index++)
    {
        wstring__append_wchar(word, str.wchars[index]);
        read_amount++;
        if (index + count1 > str.length && index + count2 > str.length) continue;
        
        bool reached_pattern1 = true;
        for (uint32_t p_index = 0; p_index < count1; p_index++)
        {
            if (str.wchars[index + p_index] != delim_pattern1[p_index])
            {
                reached_pattern1 = false;
                break;
            }
        }
        
        if (reached_pattern1)
        {
            for (uint32_t p_index = 1; p_index < count1; p_index++)
            {
                wstring__append_wchar(word, str.wchars[index + p_index]); 
                read_amount++;  
            }
            break;
        }

        bool reached_pattern2 = true;
        for (uint32_t p_index = 0; p_index < count2; p_index++)
        {
            if (str.wchars[index + p_index] != delim_pattern2[p_index])
            {
                reached_pattern2 = false;
                break;
            }
        }
        
        if (reached_pattern2)
        {
            for (uint32_t p_index = 1; p_index < count2; p_index++)
            {
                wstring__append_wchar(word, str.wchars[index + p_index]); 
                read_amount++;  
            }
            break;
        }
    }
    return read_amount;
}

bool wstring__compare(const wstring_t str1, const wstring_t str2)
{
    bool same_length = str1.length == str2.length;

    if (same_length)
    {
        for (uint32_t i = 0; i < str1.length; i++)
        {
            if (str1.wchars[i] != str2.wchars[i])
            {
                return false;
            }
        }
    }

    return same_length;
}

files_data_s files__create(memory_type_e type)
{
    files_data_s files_data = { 0 };

    files_data.capacity = memory___type_to_size(type);
    files_data.files    = memory___alloc(type);

    return files_data;
}

void files___existing_files__read(files_data_s *existing_files)
{ 
    HANDLE objects_file_handle = CreateFileW(
        (const wchar_t*)build_info_s.existing_object_files_file_path,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    assert(objects_file_handle != INVALID_HANDLE_VALUE);
    
    char file_buffer [ MEMORY_CHUNK_HUGE_SIZE ];
    unsigned long read_amount;

    assert(
        ReadFile(
            objects_file_handle, 
            file_buffer, 
            MEMORY_CHUNK_HUGE_SIZE, 
            &read_amount, 
            NULL
        )
    );
    assert (read_amount < MEMORY_CHUNK_HUGE_SIZE);

    file_buffer[read_amount] = '\0';

    memory_type_e mem_type = memory___size_to_type(read_amount + 1);
    wstring_t wstring_file = wstring__create(mem_type);
    wstring__append_chars(&wstring_file, file_buffer, read_amount + 1);
    
    for (uint32_t index = 0; index < read_amount; index++)
    {
        wstring_t file_name = wstring__create(MEMORY_TYPE_SMALL);
        uint32_t file_name_length = wstring__get_word_by_delim_pattern_index(
            wstring_file, 
            index,
            L".c", 
            2, 
            &file_name
        );
        wstring__append_char(&file_name, '\0');
        index += file_name_length;
        
        SYSTEMTIME st = { 0 };

        assert(index + 4 < read_amount);
        st.wYear = atoi(
            (char [5]) {
                file_buffer[index++], 
                file_buffer[index++], 
                file_buffer[index++], 
                file_buffer[index++],
                '\0'
            }
        );
        
        assert(index + 2 < read_amount);
        st.wMonth = atoi(
            (char [3]) {
                file_buffer[index++], 
                file_buffer[index++],
                '\0'
            }
        );

        assert(index + 2 < read_amount);
        st.wDay = atoi(
            (char [3]) {
                file_buffer[index++], 
                file_buffer[index++],
                '\0'
            }
        );
        
        assert(index + 2 < read_amount);
        st.wHour = atoi(
            (char [3]) {
                file_buffer[index++], 
                file_buffer[index++],
                '\0'
            }
        );
        
        assert(index + 2 < read_amount);
        st.wMinute = atoi(
            (char [3]) {
                file_buffer[index++], 
                file_buffer[index++],
                '\0'
            }
        );
        
        assert(index + 2 < read_amount);
        st.wSecond = atoi(
            (char [3]) {
                file_buffer[index++], 
                file_buffer[index++],
                '\0'
            }
        );
        
        assert(index + 4 <= read_amount);
        st.wMilliseconds = atoi(
            (char [5]) {
                file_buffer[index++], 
                file_buffer[index++],
                file_buffer[index++],
                file_buffer[index++],
                '\0'
            }
        );

        files___append_file(existing_files, file_name, (wstring_t){ 0 }, st);
        // for '\n'
    }

    CloseHandle(objects_file_handle);
}

void files___existing_files__update(files_data_s updated_files)
{
    HANDLE objects_file_handle = CreateFileW(
        (const wchar_t*)build_info_s.existing_object_files_file_path,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    assert(objects_file_handle != INVALID_HANDLE_VALUE);

    char write_buffer [ MEMORY_CHUNK_HUGE_SIZE ];
    uint32_t write_buffer_len = 0;

    for (uint32_t i = 0; i < updated_files.count; i++)
    {
        char name [ 128 ];
        char temp [ 256 ];
        uint32_t j = 0;
        for (; j < updated_files.files[i].file_name.length; j++)
        {
            name[j] = (char)updated_files.files[i].file_name.wchars[j];
        }
        name[j] = '\0';

        sprintf(
            temp,
            "%s%04d%02d%02d%02d%02d%02d%04d\n",
            name, 
            updated_files.files[i].file_last_write_data.wYear,
            updated_files.files[i].file_last_write_data.wMonth,
            updated_files.files[i].file_last_write_data.wDay,
            updated_files.files[i].file_last_write_data.wHour,
            updated_files.files[i].file_last_write_data.wMinute,
            updated_files.files[i].file_last_write_data.wSecond,
            updated_files.files[i].file_last_write_data.wMilliseconds
        );

        memcpy(write_buffer + write_buffer_len, temp, strlen(temp));
        write_buffer_len += strlen(temp);
    }

    unsigned long amount_wrriten;
    assert(
        WriteFile(
            objects_file_handle,
            write_buffer,
            write_buffer_len,
            &amount_wrriten,
            NULL
        )
    );

    CloseHandle(objects_file_handle);
}

void files___append_file(files_data_s *files, wstring_t file_name, wstring_t file_path, SYSTEMTIME file_last_wrirte_time)
{
    if (files->count * sizeof(files->files[0]) >= files->capacity)
    {
        memory_type_e mem_type = memory___size_to_type(files->capacity);
        assert(mem_type != MEMORY_TYPE_COUNT && mem_type != MEMORY_TYPE_HUGE);

        memory_type_e next_mem_type = (mem_type == MEMORY_TYPE_SMALL) ? MEMORY_TYPE_NORMAL : MEMORY_TYPE_HUGE;   

        files->capacity = memory___type_to_size(next_mem_type);
        files->files = memory___realloc(files->files, next_mem_type);
    }

    files->files[files->count].file_name = file_name; 
    files->files[files->count].file_path = file_path; 
    files->files[files->count++].file_last_write_data = file_last_wrirte_time; 
}

#endif