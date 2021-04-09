/*
	Operating System 20-2
	[HW1] My Simple Shell
	20172129 Yesol Park
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUF 256
 
int main(int argc, char** argv) {

    pid_t pid;
    char *message;    
    char buf[MAXBUF]; //get commands

    while(1) {
		// prompt should be “yourname$”.
		fprintf(stdout, "Yesol$ ");
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf)-1, stdin);
        buf[strlen(buf) - 1] = '\0'; 
	
		// If user inputs “exit” command, exit your shell.
        if(!strncmp(buf, "exit", strlen(buf))) {
            return -1;
        }

		// Each user command is executed by creating a child process and executing the command.
		message = ">> Command is executed by creating a child process...";
		puts(message);
        pid = fork();

        if(pid < 0) {
            perror("fork error\n");
            return -1;
        }
        else if (pid == 0) {
			message = ">> Command is executing by execlp()...\n";
			puts(message);
            execlp(buf, buf, NULL);
			message = ">> Done...";
			puts(message);
			exit(0);
        }
        else {
			//The shell process must wait until the command processing is completed in the child process.
			message = ">> The shell process wait until the command processing is completed in the child process...";
            puts(message);     
			wait(NULL);
        }
    }
    return 0;
}
