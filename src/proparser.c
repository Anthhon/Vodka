#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "templates.h"
#include "proparser.h"

//#define DEBUG
#include "main.h"

// Code snippet extracted, and adapted to project, from this stackoverflow question:
// https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
static char *trim(char *str)
{
    size_t str_l = 0;
    char *front_ptr = str;
    char *end_ptr = NULL;

    if(str == NULL) { return NULL; }
    if(str[0] == '\0') { return str; }

    str_l = strlen(str);
    end_ptr = str + str_l;

    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while (isspace((unsigned char) *front_ptr)) { ++front_ptr; }
    if( end_ptr != front_ptr ) {
        while( isspace((unsigned char) *(--end_ptr)) && end_ptr != front_ptr ) {}
    }

    if( front_ptr != str && end_ptr == front_ptr ) {
        *str = '\0';
    } else if (str + str_l - 1 != end_ptr) {
        *(end_ptr + 1) = '\0';
    }

    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of end_ptr to mean the front of the string buffer now.
     */
    end_ptr = str;
    if (front_ptr != str) {
        while (*front_ptr) { *end_ptr++ = *front_ptr++; }
        *end_ptr = '\0';
    }

    return str;
}

// TODO: Elaborate more flexible way of adding configuration type check
ConfigType parse_config_type(char *str)
{
    if (strcasecmp("timestamp_format", str) == 0) {
        return CONFIG_TIMESTAMP_FORMAT_T;
    } else if (strcasecmp("root_path", str) == 0) {
        return CONFIG_ROOT_PATH_T;
    } else if (strcasecmp("templates_path", str) == 0) {
        return CONFIG_TEMPLATES_PATH_T;
    } else {
        return CONFIG_UNKNOWN_T;
    }
}

static char *curr_pos = NULL;
static char *config_content = NULL;
ConfigToken *parse_next_config(const char *CONFIG_FILE_PATH)
{
    if (!curr_pos) {
        config_content = read_file(CONFIG_FILE_PATH);
        curr_pos = config_content;
    }

    char *token = NULL;
    ConfigToken *config_tk = calloc(1, sizeof(*config_tk));

    while ((token = strtok_r(curr_pos, "=\n\t", &curr_pos))) {
        if (config_tk->config != NULL && config_tk->value != NULL) {
            LogExit("Extra value found in config file: %s\n", token);
        }

        char *trimmed_token = trim(token);
        uint16_t token_l = strlen(trimmed_token);
        char *str_ptr = calloc(token_l + 1, sizeof(*trimmed_token));

        memcpy(str_ptr, trimmed_token, token_l);

        // TODO: Improve comment parsing/recognition
        if (str_ptr[0] == '#' || str_ptr[0] == '!') {
            continue;
        }

        if (config_tk->config == NULL) {
            config_tk->config = str_ptr;
            config_tk->type = parse_config_type(str_ptr);
        } else {
            config_tk->value = str_ptr;
        }
        
        if (config_tk->config && config_tk->value) {
            return config_tk;
        }
    }

    if (config_tk->config && !config_tk->value) {
        LogError("%s config does not have a value attributed to it\n", config_tk->config);
    }

    _Debug({ LogDebug("Reached configurations end\n"); });
    free(config_content);
    config_content = NULL;
    curr_pos = NULL;

    return NULL;
}
