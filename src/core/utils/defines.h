#pragma once

// Aliases for better readability

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef signed char         s8;
typedef signed short        s16;
typedef signed int          s32;
typedef signed long long    s64;

typedef float               f32;
typedef double              f64;


// Standard includes

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>

// Utils


#define U8_MAX  ((1 << 8)  - 1)
#define U16_MAX ((1 << 16) - 1)
#define U32_MAX ((1 << 32) - 1)
#define U64_MAX ((1 << 64) - 1)

#define MULTI_LINE_DEFINITION_WRAPPER(def) do { def } while (0)
// Logging

#if defined(_DEBUG)
#   define NOT_IMPLEMENTED() assert(0 && "Not Implemented!")
#   define UNREACHABLE(msg) assert(0 && "ERROR: Unreachable code section!" && msg)
#   define UNUSED(v) (void)(v)
#   define ANSI_COLOR_RED     "\x1b[31m"
#   define ANSI_COLOR_GREEN   "\x1b[32m"
#   define ANSI_COLOR_YELLOW  "\x1b[33m"
#   define ANSI_COLOR_BLUE    "\x1b[34m"
#   define ANSI_COLOR_MAGENTA "\x1b[35m"
#   define ANSI_COLOR_CYAN    "\x1b[36m"
#   define ANSI_COLOR_RESET   "\x1b[0m"
#   define LOG_INFO(...) \
        MULTI_LINE_DEFINITION_WRAPPER(\
            printf(ANSI_COLOR_GREEN "[INFO]: " ANSI_COLOR_RESET __VA_ARGS__); \
            printf("\n"); \
        )
#   define LOG_ERROR(...)\
        MULTI_LINE_DEFINITION_WRAPPER( \
            printf(ANSI_COLOR_RED "[ERROR]: " ANSI_COLOR_RESET __VA_ARGS__);\
            printf("\nFile: '%s', Line #%d.", __FILE__, __LINE__);\
            printf("\n");\
        )
#   define LOG_WARNING(...)\
        MULTI_LINE_DEFINITION_WRAPPER( \
            printf(ANSI_COLOR_YELLOW "[WARNING]: " ANSI_COLOR_RESET __VA_ARGS__);\
            printf( "\n"); \
        )
#else
#   define NOT_IMPLEMENTED()
#   define UNREACHABLE(msg) 
#   define UNUSED(v) 
#   define LOG_INFO(...)
#   define LOG_ERROR(...)
#   define LOG_WARNING(...)
#endif