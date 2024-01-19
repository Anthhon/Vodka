#include <stdio.h>
#include <stdlib.h>

//#define DEBUG
#include "main.h"

char *read_files(char *filepath)
{
    size_t file_l = 0;

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
        LogDebug("Read %li bytes from \'%s\'\n", bytesRead, filepath);
    });

    fclose(file);

    return buffer;
}
