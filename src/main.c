#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

//#define DEBUG
#include "main.h"

void print_usage(char *program)
{
    fprintf(stdout, "Usage: %s [options]\n", program);
    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  %s\t\t\tRun the server\n", FLAG_RUN);
    fprintf(stdout, "  %s\t\t\tDisplay this help message\n", FLAG_HELP);
    fprintf(stdout, "\n");
    fprintf(stdout, "Example:\n");
    fprintf(stdout, "  %s %s\n", program, FLAG_RUN);
    fprintf(stdout, "  %s %s\n", program, FLAG_HELP);
    fprintf(stdout, "\n");
    fprintf(stdout, "For more information, visit: https://github.com/Anthhon/Vodka\n");
    exit(EXIT_SUCCESS);
}

void print_banner(void)
{
    fprintf(stdout, " ▌ ▐·      ·▄▄▄▄  ▄ •▄  ▄▄▄· \n");
    fprintf(stdout, "▪█·█▌▪     ██▪ ██ █▌▄▌▪▐█ ▀█ \n");
    fprintf(stdout, "▐█▐█• ▄█▀▄ ▐█· ▐█▌▐▀▀▄·▄█▀▀█ \n");
    fprintf(stdout, " ███ ▐█▌.▐▌██. ██ ▐█.█▌▐█ ▪▐▌\n");
    fprintf(stdout, ". ▀   ▀█▄▀▪▀▀▀▀▀• ·▀  ▀ ▀  ▀ \n");
}

int main(int argc, char *argv[])
{
    if (argc == 1) print_usage(argv[0]);
    
    char *program = argv[0];

    print_banner();
    for (uint16_t i = 1; i <= argc; ++i) {
        _Debug({
                LogDebug("Checking argument: \"%s\"\n", argv[i]);
        });
        if (strncmp(FLAG_RUN, argv[i], FLAG_RUN_L) == 0) {
            return server_run();
        }
        if (strncmp(FLAG_HELP, argv[i], FLAG_HELP_L) == 0) {
            print_usage(program);
        }
    }

    return 0;
}
