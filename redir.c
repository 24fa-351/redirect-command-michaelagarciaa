#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void redir(const char *inp, const char *cmd, const char *out) {
    int in_fd, out_fd;


    char *cmd_copy = strdup(cmd);
    char *args[128];
    int i = 0;
    char *token = strtok(cmd_copy, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;  

   
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  

        if (strcmp(inp, "-") != 0) {
            in_fd = open(inp, O_RDONLY);
            if (in_fd < 0) {
                perror("Input file open failed");
                exit(EXIT_FAILURE);
            }
            if (dup2(in_fd, STDIN_FILENO) < 0) {
                perror("dup2 for input failed");
                close(in_fd);
                exit(EXIT_FAILURE);
            }
            close(in_fd);
        }


        if (strcmp(out, "-") != 0) {
            out_fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("Output file open failed");
                exit(EXIT_FAILURE);
            }
            if (dup2(out_fd, STDOUT_FILENO) < 0) {
                perror("dup2 for output failed");
                close(out_fd);
                exit(EXIT_FAILURE);
            }
            close(out_fd);
        }


        execvp(args[0], args);


        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {  
        int status;
        waitpid(pid, &status, 0);  
    }

    free(cmd_copy);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    redir(argv[1], argv[2], argv[3]);
    return 0;
}
