#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "urls.h"

//#define DEBUG
#include "main.h"

void urls_set(void)
{
    // Set project urls here
    url_push(&urls_manager, "/", "index", "index.html");

    // Free urls
    free(urls_manager.urls);
    urls_manager.urls = NULL;
}

void url_push(UrlsManager *urls_manager, char *url_obj, char *name_obj, char *file_path_obj)
{
    urls_manager->urls = realloc(urls_manager->urls, (sizeof(*urls_manager->urls) * urls_manager->capacity++));
    if (urls_manager->urls == NULL) {
        LogExit("Could not allocate memory to store new url object.\n");
        return;
    }

    UrlObject new_url = {
        .url = url_obj,
        .name = name_obj,
        .file_path = file_path_obj,
    };

    memcpy(&urls_manager->urls[urls_manager->capacity - 1], &new_url, sizeof(*urls_manager->urls));

    _Debug({
            LogDebug("UrlObj added (url: \"%s\", name: \"%s\", file_path: \"%s\").\n", new_url.url, new_url.name, new_url.file_path);
    });

    return;
}
