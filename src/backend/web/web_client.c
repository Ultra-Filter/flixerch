#include <curl/curl.h>
#include "web_client.h"

typedef struct
{
    u8 *buffer;
    u64 size;
    u64 offset;
} memory_chunk;

static CURL *curl_handle;

static u64 load_to_memory(void *content, u64 type_size, u64 count, void *user)
{
    memory_chunk *mem = (memory_chunk *)user;
    u64 chunk_size = type_size * count;
    if (mem->offset + chunk_size >= mem->size)
    {
        if (mem->size == 0)
        {
            mem->size = 1;
        }
        while (mem->size < chunk_size + mem->offset)
        {
            mem->size *= 2;
        }
        void *block = realloc(mem->buffer, mem->size);
        if (block == NULL)
        {
            LOG_ERROR("try realloc and failed");
            return 0;
        }
        mem->buffer = block;
    }
    memcpy(mem->buffer + mem->offset, content, chunk_size);
    mem->offset += chunk_size;
    return chunk_size;
}

bool web_client_init(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    return curl_handle != NULL;
}

void web_client_shutdown(void)
{
    if (curl_handle != NULL)
    {
        curl_easy_cleanup(curl_handle);
    }
    curl_global_cleanup();
}

void web_client_free(void * block)
{
    if(block!=NULL){
        free(block);
    }
}

u8 *web_client_request_all(const char *url)
{
    CURLcode res;
    res = curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    if (res != CURLE_OK)
    {
        LOG_WARNING("url is not valid, url: %s", url);
        return NULL;
    }
    res = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, load_to_memory);
    if (res != CURLE_OK)
    {
        LOG_ERROR("write function not acceptable by curl");
        return NULL;
    }

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, 
        "Authorization: Bearer "
        "eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJlMjMwNWQ1ZWZlYWI2NGZjZGZiMTAwNzg3NDUxM2U2NCIsIm5iZiI6MTc4NTc2NjY1My43ODcwMDAyLCJzdWIiOiI2YTcwYTJmZGNmYjZhMjVlYmE5YmVlNjQiLCJzY29wZXMiOlsiYXBpX3JlYWQiXSwidmVyc2lvbiI6MX0.h5jhpXpDmx13Bhfat8dIxr8cFnKQX-4dPOb2bGldC8c");
    res = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    if (res != CURLE_OK)
    {
        LOG_WARNING("header is not set, url: %s", url);
        return NULL;
    }


    memory_chunk mem = {0};

    res = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &mem);
    if (res != CURLE_OK)
    {
        LOG_ERROR("memory chunk not acceptable by curl");
        return NULL;
    }
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK)
    {
        LOG_WARNING("failed getting result from url: %s", url);
        if (mem.size > 0)
        {
            free(mem.buffer);
        }
        return NULL;
    }
    return mem.buffer;
}
