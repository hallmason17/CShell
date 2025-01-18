// Copyright 2025 Mason H.
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define TOK_DELIM " \r\n"
#define BUF_SIZE 1024
#define PATH_MAX 4096
#define NAME_MAX 255
#define MAX_PATH_ENTRIES 250 * PATH_MAX
#define MAX_FILE_NAME_SIZE PATH_MAX + NAME_MAX

static char *builtins[3] = {"type", "echo", "cd"};

typedef struct {
    char *cmd;
    int num_args;
    char *args[];
} command;

command *parse_line(char *line) {
    command *cmd = malloc(sizeof(command) + BUF_SIZE * sizeof(char *));
    char *tok = NULL;
    cmd->num_args = 0;
    tok = strtok(line, TOK_DELIM);
    while (tok) {
        cmd->args[cmd->num_args] = tok;
        cmd->num_args++;
        tok = strtok(NULL, TOK_DELIM);
    }
    cmd->args[cmd->num_args] = NULL;
    cmd->cmd = cmd->args[0];

    return cmd;
}

char *read_from_stdin() {
    char *line = NULL;
    size_t linelen = 0;
    if (getline(&line, &linelen, stdin) == -1) {
        perror("csh");
        exit(EXIT_FAILURE);
    }
    return line;
}

void exec_program_with_args(command *cmd) {
    pid_t pid;
    int pstatus = 0;
    pid = fork();
    if (pid == -1) {
        perror("csh");
    } else if (pid == 0) {
        if (execvp(cmd->cmd, cmd->args) == -1) {
            perror("csh");
        }
    } else {
        do {
            waitpid(pid, &pstatus, WUNTRACED);
        } while (!WIFEXITED(pstatus) && !WIFSIGNALED(pstatus));
    }
}

int main(int argc, char *argv[]) {
    char *line = NULL;
    while (1) {
        char cwd[PATH_MAX] = {0};
        getcwd(cwd, sizeof(cwd));

        printf("%s ", cwd);

        line = read_from_stdin();
        command *input = parse_line(line);

        if (strcmp("exit", input->cmd) == 0) {
            exit(EXIT_SUCCESS);
        } else if (strcmp("cd", input->cmd) == 0) {
            if (input->num_args == 0) {
                char *home = getenv("HOME");
                if (chdir(home) != 1) {
                    fprintf(stderr, "Unable to cd to %s", home);
                }
            } else if (input->num_args == 2) {
                if (chdir(input->args[1]) != 0) {
                    perror("csh");
                }
            } else {
                fprintf(stderr, "csh: cd only takes one argument\n");
            }
        } else {
            exec_program_with_args(input);
        }
        fflush(stdout);
        free(line);
        free(input);
    }

    return EXIT_SUCCESS;
}
