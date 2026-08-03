#pragma once
#include "../../core/utils/defines.h"


bool web_client_init(void);
void web_client_shutdown(void);
void web_client_free(void * block);
u8 * web_client_request_all(const char * url);
// u8 * web_client_request_chunk(const char * url);

