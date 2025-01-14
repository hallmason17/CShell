// Copyright 2025 Mason H.
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096
#define NAME_MAX 255
#define MAX_PATH_ENTRIES 250 * PATH_MAX
#define MAX_FILE_NAME_SIZE PATH_MAX + NAME_MAX

/**
 * Returns true if command is in given path. Writes dir and filename into loc
 * param.
 * @param loc buffer to write dir/filename into
 * @param path_dirs path array
 * @param num_dirs entries in path
 * @param command command to search
 * @return true if found in path, false otherwise. writes path into loc.
 */
bool get_cmd_from_path(char *loc, char **path_dirs, size_t num_dirs,
                       char *command) {
    char cmdexe[50];
    struct dirent *pdirent;
    snprintf(cmdexe, strlen(command) + 5, "%s.%s", command, "exe");
    for (int j = 0; j < num_dirs; ++j) {
        DIR *pdir = opendir(path_dirs[j]);
        if (pdir) {
            while ((pdirent = readdir(pdir))) {
                if (strcmp(command, pdirent->d_name) == 0 ||
                    strcmp(cmdexe, pdirent->d_name) == 0) {
                    snprintf(loc,
                             strlen(path_dirs[j]) + strlen(pdirent->d_name) + 2,
                             "%s/%s", path_dirs[j], pdirent->d_name);
                    return true;
                }
            }
            closedir(pdir);
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: csh [COMMAND]\n");
        return EXIT_FAILURE;
    }
    char *pathenv = getenv("PATH");

    char *path_dirs[MAX_PATH_ENTRIES] = {0};
    char *saveptr;
    char *delim = ":";
    char *tok = strtok_r(pathenv, delim, &saveptr);
    int i = 0;
    while (tok) {
        path_dirs[i] = tok;
        ++i;
        tok = strtok_r(NULL, delim, &saveptr);
    }
    char *cmd = argv[1];
    char cmdloc[MAX_FILE_NAME_SIZE];
    int found = get_cmd_from_path(cmdloc, path_dirs, i, cmd);
    if (found) {
        printf("%s\n", cmdloc);
    } else {
        fprintf(stderr, "csh: Command not found: %s\n", cmd);
    }
    return EXIT_SUCCESS;
}
