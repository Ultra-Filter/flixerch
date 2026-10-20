
#define JSON_PARSER_IMPLEMENTATION
#include "../src/core/decoders/json_parser.h"
#define DSTRING_IMPLEMENTATION
#include "../src/core/utils/containers/dstring.h"
#define DBUILD_IMPLEMENTATION
#include "dbuild.h"

int main(int argc, char** argv)
{
    dbuild(argc, argv);
    return 0;
}
