# OperatingSystems_project
# Berina Crnčalo & Ahmed Hasković

Files uploaded:
1. shell.c
2. systemcalls.c
3. README.md

Question 1.5.1: If we have a single-core, uniprocessor system that supports multiprogramming, how many processes can be in a running state in such a system, at any given time?

Answer 1.5.1: In a single-core, uniprocessor system with multiprogramming support, multiple processes can be loaded into memory simultaneously. However, due to the limitation of having only one CPU core, only one process can actively execute instructions at any given time. Therefore, in such a system, only one process can be in the running state at a time, but the computer can quickly switch between programs to give the appearance of them all running simultaneously.


Question 1.5.2: Explain why system calls are needed for a shared memory method of inter-process communication (IPC). If there are multiple threads in one process, are the system calls needed for sharing memory between those threads?

Answer 1.5.2: System calls are necessary for establishing shared memory regions between processes in inter-process communication (IPC) using shared memory. These calls manage memory protection, address space mapping, and synchronization mechanisms to ensure safe and controlled access to shared memory segments. However, when multiple threads exist within a single process, system calls are generally not needed for sharing memory between them. Threads within the same process share the same address space, allowing them to directly access and modify shared memory without the need for additional system calls. Instead, synchronization primitives like mutexes or semaphores are often used to coordinate access to shared resources among threads within the same process.


Question 1.5.3: Consider the following sample code from a simple shell program. Now, suppose the shell wishes to redirect the output of the command not to STDOUT but to a file “foo.txt”. Show how you would modify the above code to achieve this output redirection.

command = read_from user ();
int rc = fork();
if (rc == 0) { //child
    exec(command);
}
else { //parent
    wait();
}

Answer 1.5.3: 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main() {
    char *command = "ls"; // Example command
    int rc = fork();

    if (rc == 0) { // Child process
        freopen("foo.txt", "w", stdout); // Redirect stdout to "foo.txt"
        execlp(command, command, NULL); // Execute the command
    } else if (rc > 0) { // Parent process
        wait(NULL); // Wait for the child process to complete
    } else { // Fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}

An outline of what we did for the assignment:

- Developed a basic shell interface allowing user input and execution of external programs.
- Implemented robust error handling to prevent crashes beyond machine failure.
- Introduced prompt customization offering two options: basic prompt prompt$ and advanced prompt machinename@username:~$.
- Dynamically retrieved machine name and username using gethostname() and getlogin_r() functions.
- Implemented basic functionality for wc, grep, df, and cmatrix programs.
- Enhanced functionality and creativity by providing options and/or arguments for at least two programs.
- Modified the shell code to allow output redirection using the > operator.
- Implemented functionality to redirect command output to a text file.
- Implemented the system calls fork(), wait(), and exec() within the chosen C-programming example.
- Demonstrated process creation and execution through these system calls.
- Explored and implemented the forkbomb as part of the system calls exploration.
- Provided concise and descriptive answers to questions regarding limitations of running processes in a single-core uniprocessor system.
- Explained the necessity of system calls for shared memory IPC in multi-threaded processes.
- Enhanced the visual appeal of the shell prompt by adding colors using ANSI escape codes.
- Named the shell according to the assignment requirements.
- Following these steps, the assignment requirements were met, covering basic, intermediate, and advanced functionalities of a custom shell implementation.

Instructions to run files:
gcc shell.c -o shell
./shell
gcc systemcalls.c -o systemcalls
./systemcalls
