/*
Author: Matheus Jun Ota
Class: CS620
Assignment: 2
Date: 02/23/2016
*/

#include "shell.h"

ExecBlock* parse(char* word)
{
	char *block = strsep(&word, "|");
	if(!block)
		return 0;
	int wordCount = 0, inWord = 0;
	for (char *i = block; *i; i++)
	{
		if (*i == '<' || *i == '>')
			break;
		if (!inWord && *i != ' ' && *i != '\t')
			inWord = 1;

		if (inWord && (*i == ' ' || *i == '\t'))
		{
			inWord = 0;
			wordCount++;
		}
	}
	if(inWord) wordCount++;
	ExecBlock *out = malloc(sizeof(ExecBlock));
	if (!out) {
		perror("Failed to allocate memory in parse\n");
		exit(0);
	}
	inWord = 0;
	int inInput=0, inOutput=0, wordIndex=0;
	char *wordBase;
	out->argv = malloc(sizeof(char*) * (wordCount+1));
	out->argv[wordCount] = 0;
	out->fileIn = 0;
	out->fileOut = 0;
	for(char *i = block; *i; i++)
	{
		
		if (!inWord && *i != ' ' && *i != '\t' && *i != '<' && *i != '>')
		{
			inWord = 1;
			wordBase = i;
		}
		if (inWord && (*i == ' ' || *i == '\t' || *i == '<' || *i == '>'))
		{
			inWord = 0;
			if(inOutput) {
				out->fileOut = wordBase;
				inOutput = 0;
			} else if (inInput) {
				out->fileIn = wordBase;
				inInput = 0;
			} else
				out->argv[wordIndex++] = wordBase;
			if(*i == '<')
				inInput = 1;
			if(*i == '>')
				inOutput = 1;
			*i = '\0';
		}
		if(*i == '<')
			inInput = 1;
		if(*i == '>')
			inOutput = 1;
	}
	if (inWord)
	{
		if(inOutput) {
			out->fileOut = wordBase;
		} else if (inInput) {
			out->fileIn = wordBase;
		} else
			out->argv[wordIndex++] = wordBase;
	}
	out->pipe = parse(word);
	return out;
}


void executeCommand(ExecBlock* block)
{
    int fd[2], aux = 0;
    pid_t pid;
    FILE *f;
    
    while(block != NULL){
        pipe(fd);
        
        //if we have a redirection for input
        if(block -> fileIn != NULL){
            f = fopen(block -> fileIn, "r");
            aux = fileno(f);
        }
        
        //fork and check for errors
        if((pid = fork()) == -1)
            exit(EXIT_FAILURE);
        
        //child receives the input from the pipe(or stdin at first)
        if (pid == 0) {
            dup2(aux, 0);
            
            //check for output redirection
            if(block -> fileOut != NULL){
                f = fopen(block -> fileOut, "w");
                dup2(fileno(f), 1);
            }
            
            //check for a next pipe to see if we need to forward the output
            else if(block -> pipe != NULL)
                dup2(fd[1], 1);
            
            //child does not need this
            close(fd[0]);
            
            //execute the command
            if(execvp(block -> argv[0], block -> argv) < 0)
                perror("error while executing\n");
            
            //fprintf(stderr, "end of execution\n");
            
            //close process
            exit(EXIT_FAILURE);
        }
        
        //parent forward stdout
        else{
            //wait for child finish his things
            wait(NULL);
            
            //parent does not need these
            close(fd[1]);
            
            //connect pipes
            aux = fd[0];
            
            //proceed to next command
            block = block -> pipe;
        }
    }
}

//This function shows how the ExecBlock can be used.
void printExecBlock(ExecBlock* block)
{
    ExecBlock *cur = block;
    while (cur)
    {
        for (char** i = cur->argv; *i; i++)
            printf("%s ", *i);
        printf("\nInput: %s\n", cur->fileIn);
        printf("Output: %s\n", cur->fileOut);
        cur = cur->pipe;
        if (cur)
            printf("Piping to\n");
    }
}


//This function should deallocate an ExecBlock including the ExecBlock(s) it might pipe to. 
void freeExecBlock(ExecBlock* block)
{
    //last block in the linked list
    if(block -> pipe == NULL){
        //free arguments
        free(block -> argv);
        
        //free the whole block
        free(block);
    }
    
    //recursive call
    else{
        freeExecBlock(block -> pipe);
        
        //free arguments
        free(block -> argv);
        
        //free the whole block
        free(block);
    }
}

int main(int arvc, char **argv)
{
    char *line;
    ExecBlock* block;
    
    while(1){
        line = readline("> ");
        
        if(line != NULL){
            //checks for exit command
            if(strcmp(line, "exit") == 0)
                break;
            else{
                //parse input
                block = parse(line);
                
                //execute it
                executeCommand(block);
                
                //free the list
                freeExecBlock(block);
            }
            
            //free the allocated line
            free(line);
            line = NULL;
        }
        
        //end of file
        else
            break;
    }
    
	return 0;
}
