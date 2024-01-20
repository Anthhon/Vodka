#ifndef URLS_H
#define URLS_H

#include <stdint.h>

typedef struct {
    const char *url;
    const char *name;
    const char *file_path;
} UrlObject;

typedef struct {
    UrlObject *urls;
    uint64_t capacity;
} UrlsManager;

void urls_set(void);
void url_push(UrlsManager *urls_manager, char *url_obj, char *name_obj, char *file_path_obj);

#endif // URLS_H
