// Copyright 2025 Mason H.
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *read_from_stdin() {
    char *line = NULL;
    size_t linelen = 0;
    if (getline(&line, &linelen, stdin) == -1) {
        perror("csh");
        exit(1);
    }
    return line;
}

char **split_line(char *str, char *delim) {
    char **split = calloc(1024 * sizeof(char *), 1);
    char *tok = strtok(str, delim);
    int i = 0;
    while (tok) {
        split[i] = tok;
        ++i;
        tok = strtok(NULL, delim);
    }

    return split;
}

void exec_program_with_args(char **input) {
    pid_t pid;
    int pstatus = 0;
    pid = fork();
    if (pid == -1) {
        perror("csh");
    } else if (pid == 0) {
        if (execvp(input[0], input) == -1) {
            perror("csh");
        }
        exit(EXIT_FAILURE);
    } else {
        do {
            waitpid(pid, &pstatus, WUNTRACED);
        } while (!WIFEXITED(pstatus) && !WIFSIGNALED(pstatus));
    }
}

int main(int argc, char *argv[]) {
    while (1) {
        char *line;
        char **input;

        printf("$ ");

        line = read_from_stdin();
        input = split_line(line, " \n");

        if (strcmp("exit", input[0]) == 0) {
            return EXIT_SUCCESS;
        } else {
            exec_program_with_args(input);
        }

        fflush(stdout);
        free(line);
        free(input);
    }

    return EXIT_SUCCESS;
}
