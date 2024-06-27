#include "systemcalls.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    // This if statement will NOT execute if and only if the return value of system(cmd) is a 0
    if (system(cmd))
    {
        return false;
    }

    return true; // This indicates that the call to system returns a 0.

}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

// The do_exec function spawns a new process and
bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    // command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    int status;
    pid_t pid;

    fflush(stdout);
    pid = fork(); // when this is called, a second instruction pointer is
    // spawned that starts executing code in this function starting from the call to fork()
    // onward. the first instruction pointer continues as well, but
    // with one variable different: the return value of fork(). In the
    // parent process, pid will be the process id the OS assigned to the child
    // process, and in the child process, pid will be zero. Essentially, the child process can be designated and assigned to specific tasks when the pid reference starting a cod elbock is equated to zero.
    if (pid == -1)
    {
        va_end(args);
        return false; // failed to fork process
    }

    if (pid == 0) // the child process enters this codepath
    {
        execv(command[0], command); // child process replaces itself with the
        // program at the full path in command[0] and passes the remaining
        // strings in the command array to the program as arguments and
        // stops when it reaches the null pointer at command[count]
        // NOTE: the 1st element of the command array is automatically
        // skipped when the 2nd argument is parsed! Docs don't mention that.
        exit(-1); // the child process failed to replace itself with the callee
    }

    if (waitpid(pid, &status, 0) == -1) // parent process continues to this point,
    // then waitpid() blocks execution until the OS process with the process id
    // of the child process. if it returns -1, there was an error waiting for the process.
    {
        va_end(args);
        return false;
    }

    if (!WIFEXITED(status))
    {
        va_end(args);
        return false;
    }

    if (WEXITSTATUS(status))
    {
        va_end(args);
        return false;
    }

    va_end(args);

    return true; // successful system call
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    pid_t pid;
    int status;
    int fd;

    fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) { // failed to open file
      return false;
    }


    fflush(stdout);
    pid = fork();
    if (pid == -1) {
        close(fd);
        va_end(args);
        perror ("fork");
        return false;
    }

    if (pid == 0)
    {
        if (dup2(fd, 1) < 0)
        {
            exit(-1); // duplicating file descriptor failed
        }
        close(fd);
        execv(command[0], command);
        exit(-1);
    }

    close(fd);

    // Wait for the child process to terminate and catch the PID of the child using waitpid() call
    if(waitpid(pid, &status, 0) == -1){
      perror ("wait");
    }


    if (waitpid(pid, &status, 0) == -1)
    {
        va_end(args);
        return false;
    }

    if (!WIFEXITED(status))
    {
        va_end(args);
        return false;
    }

    if (WEXITSTATUS(status))
    {
        va_end(args);
        return false;
    }

    va_end(args);
    return true; // successful system call


        va_end(args);

        return true;
}
