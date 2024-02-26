#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "urls.h"

//#define DEBUG
#include "main.h"

void urls_set(UrlsManager *urls_manager)
{
    // Set project urls here
    url_push(urls_manager, "/", "index", "/index.html");
    url_push(urls_manager, "/info", "extrainfo", "/other.html");

    // Project default urls
    url_push(urls_manager, "/style.css", "style", "/style.css");
    url_push(urls_manager, "/script.js", "script", "/script.js");
    url_push(urls_manager, PAGE_PATH_404, PAGE_NAME_404, PAGE_PATH_404);

    _Debug({
            for (size_t i = 0; i < urls_manager->capacity; ++i) {
                LogDebug("\'%s\' added at the address \'%s\' (%li token)\n", \
                        urls_manager->urls[i].file_path, \
                        urls_manager->urls[i].url, i + 1);
            }
    });
}

size_t url_exist(const UrlsManager *urls_manager, const char *url)
{
    if (urls_manager->urls == NULL) { return SIZE_MAX; }

    for (size_t i = 0; i < urls_manager->capacity; ++i) {
        if (strcmp(urls_manager->urls[i].url, url) == 0) {
            return i;
        }
    }

    return SIZE_MAX;
}

void url_push(UrlsManager *urls_manager, char *url_obj, char *name_obj, char *file_path_obj)
{
    ++urls_manager->capacity;

    urls_manager->urls = realloc(urls_manager->urls, (sizeof(UrlObject) * urls_manager->capacity));
    if (urls_manager->urls == NULL) {
        LogExit("Could not allocate memory to store new url object.\n");
        return;
    }

    UrlObject new_url = {
        .url = url_obj,
        .name = name_obj,
        .file_path = file_path_obj,
    };

    memcpy(&(urls_manager->urls[urls_manager->capacity - 1]), &new_url, sizeof(UrlObject));
}
