#ifndef REQUEST_H
#define REQUEST_H

// GET /some/path/file.html HTTP/1.1
// Host: localhost:8080
// Connection: keep-alive
// Cache-Control: max-age=0
// sec-ch-ua: "Not_A Brand";v="8", "Chromium";v="120"
// sec-ch-ua-mobile: ?0
// sec-ch-ua-platform: "Linux"
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9,pt-BR;q=0.8,pt;q=0.7

typedef enum {
    GET = 0,
    POST,
} HttpMethod;

typedef enum {
    KEEP_ALIVE = 0,
    CLOSE,
} ConnType;

typedef struct {
    HttpMethod method;
    char *path;
} HttpRequest;

HttpRequest *request_parse(const char *request);

#endif // REQUEST_H
