#include "file_system.h"

#include "defines.h"

#define FILE_SYSTEM_MAX_FILE_SIZE_IN_BYTES (__UINT16_MAX__ - 1)

char *read_entire_file(const char *path)
{
    FILE* f = fopen(path, "rb");

    if (f == NULL)
    {
        LOG_ERROR("Can't open file at: '%s'", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    
    assert(pos > 0 && pos < FILE_SYSTEM_MAX_FILE_SIZE_IN_BYTES);

    char* data = (char*)malloc((pos + 1) * sizeof(char));
    
    fseek(f, 0, SEEK_SET);
    
    fread(data, sizeof(char), pos, f);
    data[pos] = '\0';

    fclose(f);
    return data;
}
