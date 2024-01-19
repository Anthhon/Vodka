#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "server.h"
#include "templates.h"

//#define DEBUG
#include "main.h"

ServerInfo server_info = {
    .port = DEFAULT_PORT,
    .server_addr = {0},
    .client_addr = {0},
    .client_len = sizeof(server_info.client_addr),
};

void handle_request() {
    char *page = NULL;
    const char *response_template = "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: text/html\r\n"
                                    "Content-Length: %lu\r\n"
                                    "\r\n%s";

    char temp_index_file_path[] = "/home/anthony/Programming/personal/vodka/templates/index.html";
    page = read_files(temp_index_file_path);

    if (page != NULL) {
        // 32 is used to make a room for the '%lu' placeholder
        char response[strlen(response_template) + strlen(page) + 32]; 

        if (snprintf(response, sizeof(response), response_template, strlen(page), page) < 0) {
            LogError("Could not write response content.\n");
        }

        if (send(server_info.client_socket, response, strlen(response), 0) == -1) {
            LogError("Could not send response to client.\n");
        }

        free(page);
        page = NULL;
    }

    if (close(server_info.client_socket)) {
        LogError("Could not close client socket properly.\n");
    }
}

void server_close(void)
{
    if (close(server_info.server_socket) == -1) {
        LogExit("Could not close server properly.\n");
    }
}

int server_init(void)
{
    server_info.server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_info.server_socket == -1) {
        LogExit("Could not create server socket.\n");
    }

    memset((char *)&server_info.server_addr, 0, sizeof(server_info.server_addr));
    server_info.server_addr.sin_family = AF_INET;
    server_info.server_addr.sin_addr.s_addr = INADDR_ANY;
    server_info.server_addr.sin_port = htons(server_info.port);

    if (bind(server_info.server_socket, \
                (struct sockaddr *)&server_info.server_addr, \
                sizeof(server_info.server_addr)) == -1) {
        LogExit("Could not bind socket to address and port.\n");
    }

    if (listen(server_info.server_socket, 5) == -1) {
        LogExit("Could not listen the server socket.\n");
    }

    return server_info.server_socket;
}

int server_run(void)
{
    LogInfo("Server starting...\n");
    server_init();

    LogInfo("Server is running!\n");
    LogInfo("Visit http://localhost:%i in your web browser to see your server.\n", server_info.port);

    while (1) {
        server_info.client_socket = accept(server_info.server_socket, \
                (struct sockaddr *)&server_info.client_addr, \
                &server_info.client_len);
        if (server_info.client_socket == -1) {
            LogError("Error accepting client connection");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(server_info.client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        time_t rawtime;
        struct tm *info;
        char timestamp[80];

        time(&rawtime);
        info = localtime(&rawtime);

        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", info);
        LogInfo("[%s] REQUEST FROM %s:%d\n", timestamp, client_ip, ntohs(server_info.client_addr.sin_port));

        // TODO: Implement thread pool to handle connections
        handle_request();
    }

    server_close();

    return 0;
}
