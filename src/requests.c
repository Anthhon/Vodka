#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "requests.h"
#include "main.h"

const uint8_t GET_L = 3;
const uint8_t POST_L = 3;

bool is_space(char ch)
{
    return (ch == ' ') ? true : false;
}

HttpRequest *request_parse(const char *request)
{
    HttpRequest *request_token = calloc(1, sizeof(*request_token));
    if (request_token == NULL) {
        LogError("Could not allocate memory to store HTTP request token.\n");
        return NULL;
    }

    if (strncmp(request, "GET", GET_L) == 0) {
        request_token->method = GET;
    }
    else if (strncmp(request, "POST", POST_L) == 0) {
        request_token->method = POST;
    }
    else {
        LogError("Request type not supported yet.\n");
        free(request_token);
        request_token = NULL;
        return NULL;
    }

    char *path_str_pos = strstr(request, "/");
    if (path_str_pos == NULL) {
        LogError("Could not find requested path.\n");
        free(request_token);
        request_token = NULL;
        return NULL;
    }

    size_t path_l = strcspn(path_str_pos, " \t\r\n\f\v");
    request_token->path = calloc(path_l + 1, sizeof(*request_token->path));
    if (request_token->path == NULL) {
        LogError("Could not allocate memory for the new request token path.\n");
        free(request_token);
        request_token = NULL;
        return NULL;
    }

    strncpy(request_token->path, path_str_pos, path_l);
    request_token->path[path_l] = '\0';

    return request_token;
}
