#include <pthread.h>
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
#include "tasks.h"

//#define DEBUG
#include "main.h"

// Request responses
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

// Maintaining a global pointer to the
// thread pool so it can be used easily
// by the shutdown method to take it down
thread_pool_t *thread_pool_global_ptr = NULL;

void handle_shutdown(int sig)
{
    UNUSED(sig);

    LogInfo("Shutting down...\n");
    server_info.server_running = false;
    free(server_info.root_path);
    server_info.root_path = NULL;
    free(server_info.static_path);
    server_info.static_path = NULL;

    if (close(server_info.server_socket) == -1) {
        LogExit("Could not close server properly.\n");
    }

    LogInfo("Cleaning URLs info...\n");
    free(urls_manager.urls);
    urls_manager.urls = NULL;

    LogInfo("Shutting threads down...\n");
    thread_pool_wait(thread_pool_global_ptr);
    thread_pool_destroy(thread_pool_global_ptr);

    LogInfo("Server closed successfully!\n");
    exit(EXIT_SUCCESS);
}

void handle_404(const char *request, int *client_socket)
{
    for (uint64_t i = 0; i < urls_manager.capacity; ++i) {
        if (strcmp(urls_manager.urls[i].name, PAGE_NAME_404) == 0) { 
            get_content(request, client_socket, i);
            break;
        }
    }
}

// TODO: Implement caching to avoid reading same file multiple times
void handle_request(void *client_socket)
{
    char new_request[2048] = {0};
    int new_socket = *((int*) client_socket);
    ssize_t received_bytes = recv(new_socket, new_request, sizeof(new_request) - 1, 0);
    if (received_bytes <= 0) {
        LogError("Error receiving request from client.\n");
        return;
    }

    HttpRequest *request_parsed = request_parse(new_request);
    if (request_parsed == NULL) {
        LogError("Not returning any content\n");
        handle_404(new_request, &new_socket);
        return;
    }

    size_t url_id = url_exist(&urls_manager, request_parsed->path);
    if (url_id == SIZE_MAX) {
        LogError("Redirecting user to 404. Page '%s' does not exist or is not registered.\n", request_parsed->path);
        handle_404(new_request, &new_socket);
    } else {
        get_content(new_request, &new_socket, url_id);
    }

    free(client_socket);
    free(request_parsed->path);
    free(request_parsed);

    if (close(new_socket)) {
        LogError("Could not close client socket properly.\n");
    }
}

const char *get_header_by_type(const char *request)
{
    switch (request_get_type(request)) {
        case HTML:
            _Debug({ LogDebug("Returning HTML content (thread %lu)\n", pthread_self()); });
            return response_html;
        case CSS:
            _Debug({ LogDebug("Returning CSS content (thread %lu)\n", pthread_self()); });
            return response_css;
        case JS:
            _Debug({ LogDebug("Returning JS content (thread %lu)\n", pthread_self()); });
            return response_js;
        default:
            return NULL;
    }
}

void get_content(const char *request, int *client_socket, size_t url_id)
{
    const char *CONTENT_PLACEHOLDER = get_header_by_type(request);
    char *page_content = read_static_file(urls_manager.urls[url_id].file_path);

    if (page_content != NULL) {
        // 32 is used to make room for the '%lu' placeholder
        char response[strlen(CONTENT_PLACEHOLDER) + strlen(page_content) + 32]; 
        memset(response, 0, sizeof(response));

        if (snprintf(response, sizeof(response), CONTENT_PLACEHOLDER, strlen(page_content), page_content) < 0) {
            LogError("Could not write response content.\n");
            free(page_content);
            return;
        }
        if (send(*client_socket, response, strlen(response), 0) == -1) {
            LogError("Could not send response to client.\n");
        }
        free(page_content);
    }
}

int server_init(uint16_t port)
{
    server_info.server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_info.server_socket == -1) {
        LogExit("Could not create server socket.\n");
    }

    memset((char *)&server_info.server_addr, 0, sizeof(server_info.server_addr));
    server_info.server_addr.sin_family = AF_INET;
    server_info.server_addr.sin_addr.s_addr = INADDR_ANY;
    server_info.server_addr.sin_port = htons(port);

    if (bind(server_info.server_socket, \
                (struct sockaddr *)&server_info.server_addr, \
                sizeof(server_info.server_addr)) == -1) {
        LogExit("Could not bind socket to port \'%i\'.\n", port);
    }

    if (listen(server_info.server_socket, 5) == -1) {
        LogExit("Could not listen the server socket.\n");
    }

    return server_info.server_socket;
}

void server_run(uint16_t port)
{
    signal(SIGINT, handle_shutdown);

    LogInfo("Server starting...\n");
    server_init(port);

    LogInfo("Initializing server multi-threading...\n");
    thread_pool_t *thread_pool = thread_pool_create(num_threads);
    thread_pool_global_ptr = thread_pool;

    LogInfo("Loading config file...\n");
    config_update();
    thread_pool_add_work(thread_pool, config_check, NULL);

    LogInfo("Setting urls...\n");
    urls_set(&urls_manager);

    LogInfo("Server is running!\n");
    LogInfo("Visit http://localhost:%i in your web browser to see your server.\n", port);
    LogInfo("Press <CTRL+C> to close server.\n\n");

    while (server_info.server_running) {
        int *new_socket = malloc(sizeof(*new_socket));
        *new_socket = accept(server_info.server_socket, (struct sockaddr *)&server_info.client_addr, &server_info.client_len);
        if (*new_socket == -1) {
            LogError("Error accepting client connection\n");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(server_info.client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        char timestamp[80] = {0};
        get_datetime(timestamp);
        LogInfo("[%s] REQUEST FROM %s:%d\n", timestamp, client_ip, ntohs(server_info.client_addr.sin_port));

        thread_pool_add_work(thread_pool, handle_request, new_socket);
    }
    thread_pool_wait(thread_pool);
    thread_pool_destroy(thread_pool);
}
