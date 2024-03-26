#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h> 

#define MAX_LINE_LENGTH 2048
#define MAX_ARGS 128

void execute_command(char *args[]);
void print_shell_prompt(char *shell_name, char *prompt_color, char *reset_color);

int main(int argc, char *argv[]) {
    char line[MAX_LINE_LENGTH];
    char *args[MAX_ARGS];
    int status;

    char *shell_name = "Berina&AhmedShell";
    char *prompt_color = "\x1b[34m"; // Blue color ANSI escape code
    char *reset_color = "\x1b[0m";

    if (argc > 1) {
        shell_name = argv[1]; // Set custom shell name from command-line argument
    }

    while (1) {
        print_shell_prompt(shell_name, prompt_color, reset_color);
        if (fgets(line, MAX_LINE_LENGTH, stdin) == NULL) {
            break;
        }

        line[strcspn(line, "\n")] = '\0';

        // Parse user input into arguments
        int i = 0;
        args[i] = strtok(line, " ");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            // Trim leading and trailing whitespace from each argument
            while (isspace(*args[i])) {
                args[i]++;
            }
            char *end = args[i] + strlen(args[i]) - 1;
            while (end > args[i] && isspace(*end)) {
                *end-- = '\0';
            }
            i++;
            args[i] = strtok(NULL, " ");
        }
        if (args[i] != NULL) {
            fprintf(stderr, "Error: Too many arguments. Maximum number of arguments exceeded.\n");
            // Discard excess arguments
            while (strtok(NULL, " ") != NULL);
        }
        args[i] = NULL; // Terminate argument list

        execute_command(args);
        wait(&status);
    }
    return 0;
}

void execute_command(char *args[]) {
    pid_t pid;

    if (args[0] == NULL) {
        return; // No command to execute
    }
    int redirect_stdout =0; // Flag for redirecting stdout to a file
    char *output_file = NULL; //Name of the file to redirect stdout to
    int i=0;
    while (args[i]!=NULL)
    {
       if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "Error: Missing filename after '>'\n");
                return;
            }
            redirect_stdout = 1;
            output_file = args[i + 1];
            args[i] = NULL; // Remove '>' from arguments
        }
        i++;
    }

    // If output redirection is requested, open the output file for writing
    int fd_out;
    if (redirect_stdout) {
        fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0) {
            perror("open");
            return;
        }
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        // If output redirection is requested, set stdout to the output file
        if (redirect_stdout) {
            if (dup2(fd_out, STDOUT_FILENO) < 0) {
                perror("dup2");
                exit(1);
            }
            close(fd_out); // Close the file descriptor after duplicating
        }

        // Execute the command
        execvp(args[0], args);
        // If execvp fails, the command and error message are printed
        fprintf(stderr, "Error executing command: %s\n", args[0]);
        perror("execvp");
        exit(1); // Exit from child process
    } else {
        // Parent process
        wait(NULL); // Waiting for the child process to finish
    }
}

// Print the shell prompt with specified color and shell name
void print_shell_prompt(char *shell_name, char *prompt_color, char *reset_color) {
    printf("%s%s$ %s", prompt_color, shell_name, reset_color);
    fflush(stdout); // Flush the output buffer to ensure prompt is displayed immediately
}
