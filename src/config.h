#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "server.h"
#include "urls.h"

typedef struct {
    // Operation variables
    volatile bool server_running;
    int server_socket;
    socklen_t client_len;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    // User defined variables
    char *root_path;
    char *static_path;
} ServerInfo;

extern UrlsManager urls_manager;
extern ServerInfo server_info;
extern const char REQUEST_KEYWORD_GET[];
extern const char REQUEST_KEYWORD_POST[];
extern char *TIMESTAMP_FORMAT;
extern char *ROOT_PATH;
extern char *TEMPLATES_PATH;
extern char *PAGE_PATH_404;
extern char *PAGE_NAME_404;
extern const size_t num_threads; // Number of threads project should use

void config_check(void *args);
void config_update(void);

#endif // CONFIG_H
