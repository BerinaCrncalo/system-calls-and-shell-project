#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h> 
#include <sys/statvfs.h>

#define MAX_LINE_LENGTH 2048
#define MAX_ARGS 128

void execute_command(char *args[]);
void print_shell_prompt(char*prompt_type,char *prompt_color, char *reset_color);
void wc(char *filename);
void grep(char *pattern, char *filename);
void df();
void cmatrix();

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
        print_shell_prompt("machinename", prompt_color, reset_color); //For machinename@uername:~$
        // print_shell_prompt("prompt", prompt_color, reset_color); //For prompt$
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

         // Check if the command is wc or grep
        if (strcmp(args[0], "wc") == 0) {
            wc(args[1]); // Pass filename as argument
        } else if (strcmp(args[0], "grep") == 0) {
            grep(args[1], args[2]); // Pass pattern and filename as arguments
        } else {
            execute_command(args);
            wait(&status);
        }
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

void print_shell_prompt(char *prompt_type, char *prompt_color, char *reset_color) {
    if (strcmp(prompt_type, "machinename") == 0) {
        char machinename[128];
        char *username = getlogin();
        
        if (gethostname(machinename, sizeof(machinename)) == -1) {
            perror("gethostname");
            return;
        }
        
        printf("%s%s@%s:%s$ %s", prompt_color, machinename, username, "~", reset_color);
    } else if (strcmp(prompt_type, "prompt") == 0) {
        printf("%s%s$ %s", prompt_color, "prompt", reset_color);
    } else {
        fprintf(stderr, "Invalid prompt type\n");
        return;
    }
    fflush(stdout);
}
void wc(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int lines = 0, words = 0, characters = 0;
    int c;
    int in_word = 0;

    while ((c = fgetc(file)) != EOF) {
        characters++;
        if (c == '\n') {
            lines++;
        }
        if (isspace(c)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            words++;
        }
    }

    fclose(file);
    printf(" %d %d %d %s\n", lines, words, characters, filename);
}

// Implementation of grep functionality
void grep(char *pattern, char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, pattern) != NULL) {
            printf("%s", line);
        }
    }

    fclose(file);
}

// Implementation of df (disk free) functionality
void df() {
    struct statvfs buffer;
    if (statvfs(".", &buffer) == -1) {
        perror("Error getting file system status");
        return;
    }

    unsigned long total_blocks = buffer.f_blocks * buffer.f_frsize;
    unsigned long free_blocks = buffer.f_bfree * buffer.f_frsize;

    printf("Filesystem     1K-blocks    Used Available Use%% Mounted on\n");
    printf(".                 %lu     %lu     %lu    %d%% / \n", total_blocks / 1024, (total_blocks - free_blocks) / 1024, free_blocks / 1024, (int)(((total_blocks - free_blocks) * 100) / total_blocks));
}

// Implementation of cmatrix functionality
void cmatrix() {
    printf("\033[2J"); // Clear screen
    printf("\033[1;1H"); // Move cursor to top-left corner

    char characters[] = {'|', '/', '-', '\\'};
    int index = 0;

    while (1) {
        printf("%c\r", characters[index]);
        fflush(stdout);
        usleep(100000); // Sleep for 100 milliseconds
        index = (index + 1) % 4;
    }
}