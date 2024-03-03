#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define FLAG_RUN "run"
#define FLAG_RUN_L 1
#define FLAG_HELP "help"
#define FLAG_HELP_L 1
#define FLAG_PORT "-p"
#define FLAG_PORT_L 2

#define LogInfo(...) fprintf(stdout, "[+] "); \
    fprintf(stdout, __VA_ARGS__)
#define LogDebug(...) fprintf(stdout, "[DEBUG] "); \
    fprintf(stdout, __VA_ARGS__)
#define LogWarning(...) fprintf(stderr, "[WARNING] "); \
    fprintf(stderr, __VA_ARGS__);
#define LogError(...) fprintf(stderr, "[ERROR] "); \
    fprintf(stderr, __VA_ARGS__);
#define LogExit(...) fprintf(stderr, "[!] "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stdout, "- INFO -------------\n"); \
    perror(""); \
    fprintf(stdout, "--------------------\n"); \
    exit(EXIT_FAILURE)
#define UNUSED(expr) (void)(expr);

// If this define is uncommented all the codebase DEBUG
// is gonna be active, so be prepared to read some noise info
//#define DEBUG
#ifdef DEBUG
#define _Debug(code) \
    do { \
        code \
    } while(0)
#else
#define _Debug(code)
#endif

void print_usage(char *program);
void print_banner(void);
int main(int argc, char *argv[]);

#endif // MAIN_H
