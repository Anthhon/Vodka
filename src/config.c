#include <stdint.h>
#include "config.h"
#include "requests.h"
#include "server.h"
#include "urls.h"

// Check 'urls.c' to see more about 'urls_manager'
UrlsManager urls_manager = {0};

// Project settings
const uint16_t SERVER_PORT = 8181;
const char REQUEST_KEYWORD_GET[] = "GET";
const char REQUEST_KEYWORD_POST[] = "POST";

const char ROOT_PATH[] = "/home/anthony/Programming/personal/vodka";
const char TEMPLATES_PATH[] = "/static";
char *PAGE_PATH_404 = "/404.html";
char *PAGE_NAME_404 = "404";

// Server info
ServerInfo server_info = {
    .server_socket = 0,
    .server_addr = {0},
    .client_socket = 0,
    .client_addr = {0},
    .client_len = sizeof(server_info.client_addr),
    .port = SERVER_PORT,
};
