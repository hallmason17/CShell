// Copyright 2025 Mason H.
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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

char *read_from_stdin() {
    char *line;
    size_t linelen;
    if (getline(&line, &linelen, stdin) == -1) {
        perror("csh");
        exit(1);
    }
    return line;
}

char **split_line(char *str, char *delim) {
    char **split = malloc(1024 * sizeof(char));
    char *tok = strtok(str, delim);
    int i = 0;
    while (tok) {
        split[i] = tok;
        ++i;
        tok = strtok(NULL, delim);
    }
    return split;
}

int main(int argc, char *argv[]) {
    char *line;
    char **input;
    while (1) {
        printf("$ ");
        line = read_from_stdin();
        input = split_line(line, " \n");
        fflush(stdout);
        free(line);
        free(input);
    }

    return 0;
}
