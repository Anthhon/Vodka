#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "templates.h"

//#define DEBUG
#include "main.h"

char *build_full_path_from_filename(const char *filepath)
{
    size_t buffer_s = strlen(server_info.root_path) + strlen(server_info.static_path) + strlen(filepath) + 3;
    char *buffer= calloc(buffer_s, sizeof(*buffer));
    if (buffer == NULL) {
        LogError("Could not allocate memory to build \'%s\' file full path.\n", filepath);
        return NULL;
    }

    snprintf(buffer, buffer_s, "%s%s%s", server_info.root_path, server_info.static_path, filepath);

    _Debug({
            LogDebug("Built full path: %s.\n", buffer);
    });

    return buffer;
}

char *read_static_file(const char *filepath)
{
    char *full_filepath = build_full_path_from_filename(filepath);
    if (full_filepath == NULL) {
        LogError("Could not build full path for \'%s\'.\n", filepath);
        return NULL;
    }

    char *return_ptr = read_file(full_filepath);
    free(full_filepath);

    return return_ptr;
}

char *read_file(const char *filepath)
{
    _Debug({ LogDebug("Reading file: %s\n", filepath); });
    
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        LogError("Error opening \'%s\' template file.\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSizeLong = ftell(file);
    if (fileSizeLong == -1) {
        LogError("Error getting \'%s\' file size.\n", filepath);
        fclose(file);
        return NULL;
    }

    size_t file_l = (size_t)fileSizeLong;
    rewind(file);

    char *buffer = calloc(file_l + 1, sizeof(*buffer));
    if (buffer == NULL) {
        LogError("Could not allocate more memory to template.\n");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, file_l, file);
    if (bytesRead != file_l) {
        LogError("Error reading file.\n");
        free(buffer);
        fclose(file);
        return NULL;
    }

    _Debug({ LogDebug("Read %li bytes from \'%s\'\n", bytesRead, filepath); });

    fclose(file);

    return buffer;
}
