#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "tasks.h"

extern thread_pool_t *thread_pool_global_ptr;

const char *get_header_by_type(const char *request);
void handle_shutdown(int sig);
void handle_404(const char *request, int *client_socket);
void get_content(const char *request, int *client_socket, size_t url_id);
void handle_request(void *client_socket);
int server_init(uint16_t port);
void server_run(uint16_t port);

#endif // SERVER_H
