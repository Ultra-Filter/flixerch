#include "file_system.h"

#include "defines.h" 

char *read_entire_file(const char *path)
{
    FILE* f = fopen(path, "rb");

    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    
    char* data = (char*)malloc((pos + 1) * sizeof(char));
    
    fseek(f, 0, SEEK_SET);
    
    fread(data, sizeof(char), pos, f);
    data[pos] = '\0';
    return data;
}
