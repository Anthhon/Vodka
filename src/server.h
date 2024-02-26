#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <arpa/inet.h>

#define THREAD_POOL_SIZE 4

typedef struct {
    int server_socket;
    int client_socket;
    socklen_t client_len;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    const uint16_t port;
} ServerInfo;

const char *get_header_by_type(const char *request);
void handle_shutdown(int sig);
void handle_404(const char *request);
void get_content(const char *request, size_t url_id);
void handle_request(void *args);
int server_init(void);
void server_run(void);

#endif // SERVER_H
