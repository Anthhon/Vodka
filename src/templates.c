#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "urls.h"
#include "templates.h"

//#define DEBUG
#include "main.h"

char *build_full_path_from_filename(const char *filepath)
{
    size_t buffer_s = strlen(ROOT_PATH) + strlen(TEMPLATES_PATH) + strlen(filepath) + 3;
    char *buffer= malloc(buffer_s);
    if (buffer == NULL) {
        LogError("Could not allocate memory to \'%s\' file full path.\n", filepath);
        return NULL;
    }

    snprintf(buffer, buffer_s, "%s%s%s", ROOT_PATH, TEMPLATES_PATH, filepath);
    buffer[buffer_s] = '\0';

    _Debug({
            LogDebug("Built full path: %s.\n", buffer);
    });

    return buffer;
}

char *read_files(const char *filepath)
{
    size_t file_l = 0;

    char *full_filepath = build_full_path_from_filename(filepath);
    if (full_filepath == NULL) {
        LogError("Could not build full path for \'%s\'.\n", filepath);
        return NULL;
    }

    FILE* file = fopen(full_filepath, "rb");
    if (!file) {
        LogError("Error opening \'%s\' template file.\n", full_filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSizeLong = ftell(file);
    if (fileSizeLong == -1) {
        LogError("Error getting \'%s\' file size.\n", full_filepath);
        fclose(file);
        return NULL;
    }
    file_l = (size_t)fileSizeLong;
    rewind(file);

    char* buffer = (char*)malloc(file_l + 1);
    if (!buffer) {
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
    buffer[file_l] = '\0';

    _Debug({
        LogDebug("Read %li bytes from \'%s\'\n", bytesRead, full_filepath);
    });

    fclose(file);

    return buffer;
}
