#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#define PARSER_MAX_PATH_LEN 1024
#define PARSER_MAX_HEADERS 16
#define PARSER_MAX_HEADER_NAME_LEN 64
#define PARSER_MAX_HEADER_VALUE_LEN 256
#define PARSER_MAX_METHOD_LEN 16
#define PARSER_MAX_VERSION_LEN 10
#define PARSER_MAX_REQUEST_LEN 1023

typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_UNKNOWN
} HttpMethod;

typedef enum {
    PARSE_OK,
    PARSE_ERROR,
    PARSE_BAD_REQUEST,
    PARSE_TOO_LARGE,
    PARSE_UNSUPPORTED_VERSION
} ParserResult;

typedef struct {
    char name[PARSER_MAX_HEADER_NAME_LEN];
    char value[PARSER_MAX_HEADER_VALUE_LEN];
} HttpHeader;

typedef struct {
    HttpMethod method;
    char method_text[PARSER_MAX_METHOD_LEN];
    char path[PARSER_MAX_PATH_LEN];
    char version[PARSER_MAX_VERSION_LEN];
    size_t header_count;
    HttpHeader headers[PARSER_MAX_HEADERS];
} HttpRequest;


void http_request_init(HttpRequest *req);
ParserResult http_parse_request(HttpRequest *req, const char *buf, size_t len);
const char *http_method_str(HttpMethod method);

#endif
