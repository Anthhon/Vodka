#ifndef URLS_H
#define URLS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    const char *url;
    const char *name;
    const char *file_path;
} UrlObject;

typedef struct {
    UrlObject *urls;
    uint64_t capacity;
} UrlsManager;

void print_urls(UrlsManager *urls_manager);
void urls_set(UrlsManager *urls_manager);
size_t url_exist(const UrlsManager *urls_manager, const char *url);
void url_push(UrlsManager *urls_manager, char *url_obj, char *name_obj, char *file_path_obj);

#endif // URLS_H
