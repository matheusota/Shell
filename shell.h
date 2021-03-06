
#ifndef SHELL_H
#define SHELL_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <readline/readline.h>


typedef struct ExecBlock ExecBlock;
struct ExecBlock 
{
	ExecBlock *pipe; //Pointer to the command to pipe to 
	pid_t threadId; //Pid of the process running the command 
	char **argv; //Arguments for the command
	char *fileIn; //The name of the file to read from
	char *fileOut; //The name of the file to output to
};


ExecBlock* parse(char*);
void executeCommand(ExecBlock*);
void freeExecBlock(ExecBlock*);

#endif
