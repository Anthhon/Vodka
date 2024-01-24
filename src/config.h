#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "server.h"
#include "urls.h"

extern UrlsManager urls_manager;
extern ServerInfo server_info;
extern const uint16_t SERVER_PORT;
extern const char REQUEST_KEYWORD_GET[];
extern const char REQUEST_KEYWORD_POST[];
extern const char ROOT_PATH[];
extern const char TEMPLATES_PATH[];
extern char *PAGE_PATH_404;
extern char *PAGE_NAME_404;

#endif // CONFIG_H
