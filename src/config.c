#include <stdint.h>
#include "config.h"
#include "requests.h"
#include "server.h"
#include "urls.h"

const char REQUEST_KEYWORD_GET[] = "GET";
const char REQUEST_KEYWORD_POST[] = "POST";

// Check 'urls.c' to see more about 'urls_manager'
UrlsManager urls_manager = {0};

// All project settings should be define in this file.
// TODO: Implement reading of 'config' file to dinamically configure server

const uint16_t SERVER_PORT = 8181; // Server port
const char TIMESTAMP_FORMAT[] = "%Y-%m-%d %H:%M:%S"; // Timestamp to logs

const char ROOT_PATH[] = "/home/anthony/Programming/personal/vodka"; // Project path
const char TEMPLATES_PATH[] = "/static"; // Static files path

char *PAGE_PATH_404 = "/404.html"; // 404 path
char *PAGE_NAME_404 = "404"; // 404 page name

ServerInfo server_info = {
    .server_socket = 0,
    .server_addr = {0},
    .client_socket = 0,
    .client_addr = {0},
    .client_len = sizeof(server_info.client_addr),
    .port = SERVER_PORT,
};
