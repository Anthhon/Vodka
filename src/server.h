#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 8080

typedef struct {
    uint16_t port;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int server_socket;
    int client_socket;
} ServerInfo;

void server_close(void);
int server_init(void);
int server_run(void);

#endif // SERVER_H
