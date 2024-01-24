#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "requests.h"

//#define DEBUG
#include "main.h"

const uint8_t GET_L = 3;
const uint8_t POST_L = 3;

bool is_space(char ch)
{
    return (ch == ' ') ? true : false;
}

RequestType request_get_type(const char *request)
{
    if (strstr(request, ".css") != NULL) {
        _Debug({ LogDebug("Returning CSS header.\n"); });
        return CSS;
    }
    else if (strstr(request, ".js") != NULL) {
        _Debug({ LogDebug("Returning JS header.\n"); });
        return JS;
    }
    else {
        _Debug({ LogDebug("Returning HTML header.\n"); });
        return HTML;
    }
}

HttpRequest *request_parse(const char *request)
{
    HttpRequest *request_token = calloc(1, sizeof(*request_token));
    if (request_token == NULL) {
        LogError("Could not allocate memory to store HTTP request object.\n");
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

    // Parse path from request
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
