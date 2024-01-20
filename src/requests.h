#ifndef REQUEST_H
#define REQUEST_H

typedef enum {
    GET = 0,
    POST = 0,
} MethodType;

typedef struct {
    MethodType method;
    const char *host;
} UserRequest;

#endif // REQUEST_H
