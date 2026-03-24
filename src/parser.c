#include "parser.h"
#include <string.h>

void http_request_init(HttpRequest *req) {
    if (req == NULL) {
        return;
    }

    req->method = HTTP_UNKNOWN;
    req->method_text[0] = '\0';
    req->path[0] = '\0';
    req->version[0] = '\0';
    req->header_count = 0;
    memset(req->headers, 0, sizeof(req->headers));
}

ParserResult http_parse_request(HttpRequest *req, const char *buf, size_t len) {
    size_t line_end = 0;
    int found_crlf = 0;

    if (req == NULL || buf == NULL || len == 0) {
        return PARSE_ERROR;
    }

    if (len > PARSER_MAX_REQUEST_LEN) {
        return PARSE_TOO_LARGE;
    }

    http_request_init(req);

    for (size_t i = 0; i + 1 < len; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n') {
            line_end = i;
            found_crlf = 1;
            break;
        }
    }

    if (!found_crlf || line_end == 0) {
        return PARSE_BAD_REQUEST;
    }

    char line[PARSER_MAX_REQUEST_LEN + 1];
    if (line_end >= sizeof(line)) {
        return PARSE_TOO_LARGE;
    }

    memcpy(line, buf, line_end);
    line[line_end] = '\0';

    char *method = line;
    char *space1 = strchr(method, ' ');
    if (space1 == NULL) {
        return PARSE_BAD_REQUEST;
    }
    *space1 = '\0';

    char *path = space1 + 1;
    while (*path == ' ') {
        path++;
    }
    if (*path == '\0') {
        return PARSE_BAD_REQUEST;
    }

    char *space2 = strchr(path, ' ');
    if (space2 == NULL) {
        return PARSE_BAD_REQUEST;
    }
    *space2 = '\0';

    char *version = space2 + 1;
    while (*version == ' ') {
        version++;
    }
    if (*version == '\0') {
        return PARSE_BAD_REQUEST;
    }

    if (strchr(version, ' ') != NULL) {
        return PARSE_BAD_REQUEST;
    }

    size_t method_len = strlen(method);
    size_t path_len = strlen(path);
    size_t version_len = strlen(version);

    if (method_len == 0 || method_len >= PARSER_MAX_METHOD_LEN) {
        return PARSE_BAD_REQUEST;
    }
    if (path_len == 0 || path_len >= PARSER_MAX_PATH_LEN) {
        return PARSE_BAD_REQUEST;
    }
    if (version_len == 0 || version_len >= PARSER_MAX_VERSION_LEN) {
        return PARSE_BAD_REQUEST;
    }

    memcpy(req->method_text, method, method_len + 1);
    memcpy(req->path, path, path_len + 1);
    memcpy(req->version, version, version_len + 1);

    if (strcmp(req->method_text, "GET") == 0) {
        req->method = HTTP_GET;
    } else if (strcmp(req->method_text, "POST") == 0) {
        req->method = HTTP_POST;
    } else {
        req->method = HTTP_UNKNOWN;
    }

    if (strcmp(req->version, "HTTP/1.0") != 0 &&
        strcmp(req->version, "HTTP/1.1") != 0) {
        return PARSE_UNSUPPORTED_VERSION;
    }

    size_t cursor = line_end + 2;

    while (cursor + 1 < len) {
        size_t header_end = 0;
        int found_header_crlf = 0;

        for (size_t i = cursor; i + 1 < len; i++) {
            if (buf[i] == '\r' && buf[i + 1] == '\n') {
                header_end = i;
                found_header_crlf = 1;
                break;
            }
        }

        if (!found_header_crlf) {
            return PARSE_BAD_REQUEST;
        }

        if (header_end == cursor) {
            return PARSE_OK;
        }

        size_t header_line_len = header_end - cursor;
        char header_line[PARSER_MAX_HEADER_NAME_LEN + PARSER_MAX_HEADER_VALUE_LEN + 2];

        if (header_line_len >= sizeof(header_line)) {
            return PARSE_TOO_LARGE;
        }

        memcpy(header_line, buf + cursor, header_line_len);
        header_line[header_line_len] = '\0';

        char *colon = strchr(header_line, ':');
        if (colon == NULL || colon == header_line) {
            return PARSE_BAD_REQUEST;
        }

        *colon = '\0';

        char *name = header_line;
        char *value = colon + 1;

        while (*value == ' ' || *value == '\t') {
            value++;
        }

        size_t name_len = strlen(name);
        size_t value_len = strlen(value);

        if (req->header_count >= PARSER_MAX_HEADERS) {
            return PARSE_TOO_LARGE;
        }
        if (name_len == 0 || name_len >= PARSER_MAX_HEADER_NAME_LEN) {
            return PARSE_TOO_LARGE;
        }
        if (value_len >= PARSER_MAX_HEADER_VALUE_LEN) {
            return PARSE_TOO_LARGE;
        }

        memcpy(req->headers[req->header_count].name, name, name_len + 1);
        memcpy(req->headers[req->header_count].value, value, value_len + 1);
        req->header_count++;

        cursor = header_end + 2;
    }

    return PARSE_BAD_REQUEST;

}
const char *http_method_str(HttpMethod method) {
    switch (method) {
        case HTTP_GET:
            return "GET";
        case HTTP_POST:
            return "POST";
        default:
            return "UNKNOWN";
    }
}
