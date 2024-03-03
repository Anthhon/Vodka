#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "config.h"
#include "urls.h"
#include "proparser.h"

//#define DEBUG
#include "main.h"

const char REQUEST_KEYWORD_GET[] = "GET";
const char REQUEST_KEYWORD_POST[] = "POST";

const char CONFIG_FILE_PATH[] = "./config.properties";

// Check 'urls.c' to see more about 'urls_manager'
UrlsManager urls_manager = {0};
const size_t num_threads = 4; // Number of threads project should use

char *TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S"; // Timestamp to logs

char *PAGE_PATH_404 = "/404.html"; // 404 path
char *PAGE_NAME_404 = "404"; // 404 page name

ServerInfo server_info = {
    .server_running = true,
    .server_socket = 0,
    .server_addr = {0},
    .client_addr = {0},
    .client_len = sizeof(server_info.client_addr),

    .root_path = NULL,
    .static_path = NULL,
};

void config_check(void *args)
{
    UNUSED(args);
    static time_t last_modification_t = 0;

    // Initialize before checking
    static struct stat file_stat;
    if (stat(CONFIG_FILE_PATH, &file_stat) == -1) {
        LogExit("Could not read config file: %s\n", CONFIG_FILE_PATH);
    }
    last_modification_t = file_stat.st_mtime;

    while (server_info.server_running) {
        static struct stat file_stat;
        if (stat(CONFIG_FILE_PATH, &file_stat) == -1) {
            continue;
        }

        if (file_stat.st_mtime > last_modification_t) {
            _Debug({ LogDebug("Config modification at: %li\n", file_stat.st_mtime); });

            last_modification_t = file_stat.st_mtime;

            // TODO: Implement dynamic config info update
            //
            // While dynamic update is not implemented, it should
            // only make a warning log
            LogWarning("Server identified config update...\n");
            LogWarning("Configurations are not gonna be up to date, until server is restarted!\n");
        }
    }

}

void config_update(void)
{
    ConfigToken *token = NULL;
    while ((token = parse_next_config(CONFIG_FILE_PATH))) {
        // This code section print all the tokens and its content,
        // which could be useful for detailed debugging, but can be
        // too much noisy in normal context, so its commented by default.
        //
        // If you are trying to find some bug into the config parser
        // this snippet can help a lot.
        //
        //_Debug({
        //        LogDebug("Token Received {\n");
        //        LogDebug("\tconfig: %s\n", token->config);
        //        LogDebug("\tvalue: %s\n", token->value);
        //        LogDebug("\ttype: %i\n", token->type);
        //        LogDebug("}\n");
        //        });

        switch (token->type) {
            case CONFIG_ROOT_PATH_T:
                server_info.root_path = token->value;
                break;
            case CONFIG_TEMPLATES_PATH_T:
                server_info.static_path = token->value;
                break;
            case CONFIG_TIMESTAMP_FORMAT_T:
                TIMESTAMP_FORMAT = token->value;
                break;
            case CONFIG_UNKNOWN_T:
                LogError("Undefined token config at: %s\n", token->config);
                break;
            default:
                LogError("Congratulations, you generated and impossible token with your config file!\n");
                break;
        }
        free(token);
        token = NULL;
    }
    LogInfo("Configurations updated successfully!\n");
}
