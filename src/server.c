#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "server.h"
#include "config.h"
#include "templates.h"
#include "urls.h"

//#define DEBUG
#include "main.h"

void handle_shutdown()
{
    if (close(server_info.server_socket) == -1) {
        LogExit("Could not close server properly.\n");
    }
    LogInfo("Shutting down server...\n");
    exit(EXIT_SUCCESS);
}

void handle_request(void)
{
    char received_request[2048] = {0};
    ssize_t received_bytes = recv(server_info.client_socket, received_request, sizeof(received_request) - 1, 0);
    if (received_bytes <= 0) {
        LogError("Error receiving request from client.\n");
        return;
    }

    _Debug({
            LogDebug("Received request {\n%s.\n}\n", received_request);
            });

    char *page_content = NULL;
    const char *page_path = "/index.html";
    const char *response_template = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %lu\r\n"
        "\r\n%s";

    _Debug({
            LogDebug("Page filename: \"%s\".\n", page_path);
            });

    page_content = read_files(page_path);

    if (page_content != NULL) {
        // 32 is used to make a room for the '%lu' placeholder
        char response[strlen(response_template) + strlen(page_content) + 32]; 
        memset(response, 0, sizeof(response));

        if (snprintf(response, sizeof(response),
                    response_template, strlen(page_content),
                    page_content) < 0) {
            LogError("Could not write response content.\n");
            return;
        }

        if (send(server_info.client_socket, response,
                    strlen(response), 0) == -1) {
            LogError("Could not send response to client.\n");
            return;
        }

        free(page_content);
        page_content = NULL;
    }

    if (close(server_info.client_socket)) {
        LogError("Could not close client socket properly.\n");
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

int server_run()
{
    signal(SIGINT, handle_shutdown);

    LogInfo("Server starting...\n");
    server_init();

    LogInfo("Setting urls...\n");
    urls_set();

    LogInfo("Server is running!\n");
    LogInfo("Visit http://localhost:%i in your web browser to see your server.\n", server_info.port);
    LogInfo("Press <CTRL+C> to close server.\n");

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

        // TODO: Implement multi-threading to handle connections
        handle_request();
    }

    if (close(server_info.server_socket) == -1) {
        LogExit("Could not close server properly.\n");
    }

    return 0;
}
