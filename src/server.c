#include <stdint.h>
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
#include "urls.h"
#include "templates.h"
#include "requests.h"

//#define DEBUG
#include "main.h"

// Request responses
static volatile bool server_running = true;
static const char response_html[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n%s";
static const char response_css[] = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nContent-Length: %lu\r\n\r\n%s";
static const char response_js[] = "HTTP/1.1 200 OK\r\nContent-Type: application/javascript\r\nContent-Length: %lu\r\n\r\n%s";

void get_datetime(char *dest)
{
    time_t rawtime;
    struct tm *info;

    time(&rawtime);
    info = localtime(&rawtime);
    strftime(dest, sizeof(dest), TIMESTAMP_FORMAT, info);
}

void handle_shutdown(int sig)
{
    UNUSED(sig);

    LogInfo("\rReceived SIGINT. Shutting down...\n");
    server_running = false;

    if (close(server_info.server_socket) == -1) {
        LogExit("Could not close server properly.\n");
    }

    free(urls_manager.urls);
    urls_manager.urls = NULL;

    exit(EXIT_SUCCESS);
}

void handle_404(const char *request)
{
    for (uint64_t i = 0; i < urls_manager.capacity; ++i) {
        if (strcmp(urls_manager.urls[i].name, PAGE_NAME_404) == 0) {
            get_content(request, i);
            break;
        }
    }
}

void handle_request(void)
{
    char new_request[2048] = {0};
    ssize_t received_bytes = recv(server_info.client_socket, new_request, sizeof(new_request) - 1, 0);
    if (received_bytes <= 0) {
        LogError("Error receiving request from client.\n");
        return;
    }

    HttpRequest *request_parsed = request_parse(new_request);
    if (request_parsed == NULL) {
        LogError("Not returning any content\n");
        handle_404(new_request);
        return;
    }

    size_t url_id = url_exist(&urls_manager, request_parsed->path);
    if (url_id == SIZE_MAX) {
        LogError("Redirecting user to 404. Page '%s' does not exist or is not registered.\n", request_parsed->path);
        handle_404(new_request);
    } else {
        get_content(new_request, url_id);
    }

    free(request_parsed->path);
    free(request_parsed);

    if (close(server_info.client_socket)) {
        LogError("Could not close client socket properly.\n");
    }
}

const char *get_header_by_type(const char *request)
{
    switch (request_get_type(request)) {
        case HTML:
            return response_html;
        case CSS:
            return response_css;
        case JS:
            return response_js;
        default:
            return NULL;
    }
}

void get_content(const char *request, size_t url_id)
{
    const char *CONTENT_PLACEHOLDER = get_header_by_type(request);
    char *page_content = read_files(urls_manager.urls[url_id].file_path);

    if (page_content != NULL) {
        // 32 is used to make room for the '%lu' placeholder
        char response[strlen(CONTENT_PLACEHOLDER) + strlen(page_content) + 32]; 
        memset(response, 0, sizeof(response));

        if (snprintf(response, sizeof(response), CONTENT_PLACEHOLDER, strlen(page_content), page_content) < 0) {
            LogError("Could not write response content.\n");
            free(page_content);
            return;
        }
        if (send(server_info.client_socket, response, strlen(response), 0) == -1) {
            _Debug({ LogDebug("Response {\n%s\n}\n", response); });
            LogError("Could not send response to client.\n");
        }
        free(page_content);
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
        LogExit("Could not bind socket to port \'%i\'.\n", server_info.port);
    }

    if (listen(server_info.server_socket, 5) == -1) {
        LogExit("Could not listen the server socket.\n");
    }

    return server_info.server_socket;
}

void server_run(void)
{
    signal(SIGINT, handle_shutdown);

    LogInfo("Server starting...\n");
    server_init();

    LogInfo("Setting urls...\n");
    urls_set(&urls_manager);

    LogInfo("Server is running!\n");
    LogInfo("Visit http://localhost:%i in your web browser to see your server.\n", server_info.port);
    LogInfo("Press <CTRL+C> to close server.\n\n");

    while (server_running) {
        server_info.client_socket = accept(server_info.server_socket, (struct sockaddr *)&server_info.client_addr, &server_info.client_len);
        if (server_info.client_socket == -1) {
            LogError("Error accepting client connection");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(server_info.client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        char timestamp[80] = {0};
        get_datetime(timestamp);
        LogInfo("[%s] REQUEST FROM %s:%d\n", timestamp, client_ip, ntohs(server_info.client_addr.sin_port));

        // TODO: Implement multi-threading to handle connections
        handle_request();
    }
}
