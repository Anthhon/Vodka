#ifndef PROPARSER_H
#define PROPARSER_H

typedef enum {
    CONFIG_UNKNOWN_T,
    CONFIG_TIMESTAMP_FORMAT_T,
    CONFIG_ROOT_PATH_T,
    CONFIG_TEMPLATES_PATH_T,
} ConfigType;

typedef struct {
    char *config;
    char *value;
    ConfigType type;   
} ConfigToken;

ConfigToken *parse_next_config(const char *CONFIG_FILE_PATH);
ConfigType parse_config_type(char *str);

#endif // PROPARSER_H
