#include <stdio.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include <unistd.h>			// fork(), getpid(), exec()
#include <sys/wait.h>		// wait()
#include <signal.h>			// signal()
#include <fcntl.h>			// close(), open()

char** parseInput(char* line)
{
	// This function will parse the input string into multiple commands or a single command with comuments depending on the delimiter (&&, ##, >, or spaces).
	char* key;
	char **keys = malloc(64*sizeof(char*));
	int pos = 0;
	while((key = strsep(&line," ")) != NULL)
	{
		keys[pos] = key;
		pos++;
	}
	keys[pos] = NULL;
	return keys;
}


void executeCommand(char** com)
{

	if(strcmp(com[0],"cd") == 0)
	{
		chdir(com[1]); // changing directory
		return;
	}
	else
	{
	  int rc = fork();
		if (rc < 0)
		{
			exit(0); // There is error forking
	  }
	  else if (rc == 0)
		{
	    // inside child process
	    if (execvp(com[0], com) < 0)
			{
	      printf("Shell: Incorrect command\n");
				exit(1);
	    }
	  }
		else
		{
	    // Parent process
	    int rc_wait = wait(NULL);
	  }
	}
  return;
	// This function will fork a new process to execute a command
}

void executeParallelCommands(char ** com)
{
	int i = 0;
	int first = i;
	while(com[i] != NULL)
	{
		while(com[i] != NULL && strcmp(com[i],"&&") != 0)
		{
			i++;
		}
		com[i] = NULL;
		int rc = fork();
		if(rc < 0)
		{
			exit(1);
		}
		else if(rc == 0)
		{
			if(execvp(com[first], &com[first]) < 0)
			{
				printf("Shell: Incorrect command\n");
				exit(1);
			}
		}
		i++;
		first = i;
	}
	 int rc_wait = wait(NULL);
	// This function will run multiple commands in parallel
}

void executeSequentialCommands(char** com)
{
	//printf("## \n");
	// This function will run multiple commands in parallel
	int i=0;
	int first = i;
	while(com[i] != NULL)
	{
		while(com[i]!=NULL && strcmp(com[i],"##") != 0)
		{
			i++;
		}
		com[i] = NULL;
	 	executeCommand(&com[first]);
		i++;
		first = i;
	}

}

void executeCommandRedirection(char** com)
{
	// This function will run a single command with output redirected to an output file specificed by user
	int i = 0;
	int first = i;
		while(com[i] != NULL && strcmp(com[i],">") != 0)
		{
			i++;
		}
		com[i] = NULL;
		int rc = fork();
		if(rc < 0)  // fork failed, so exits
		{
			exit(1);
		}
		else if(rc == 0)
		{
			if(com[i+1] == NULL)
				return;
			close(STDOUT_FILENO);
			open(com[i+1], O_CREAT | O_WRONLY | O_APPEND);
			if(execvp(com[0],com)<0) // in case of wrong command
			{
				printf("Shell: Incorrect command");
				exit(1);
			}
		}
		else
		{
			int rc_wait = wait(NULL); // parent process wait for the Child
		}

}


void print_path(char s) // to print the path of the currentWorkingDirectory
{
	char path[2048];
	getcwd(path, sizeof(path));
	printf("%s%c",path,s);
}

int main()
{

	// Initial declarations
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	long unsigned int size = 10;
	char* line;
	line = (char *)malloc(size);
	char ** com;
	int flag = 0;
	while(1)	// This loop will keep your shell running until user exits.
	{
		int i = 0;
		// Print the prompt in format - currentWorkingDirectory$
		print_path('$');
		// accept input with 'getline()'
		getline(&line,&size,stdin);		
		while(line[i] != '\0')
		{
			if(line[i] == EOF || line[i] == '\n')
			{
				line[i] = '\0';
			}
			i++;
		}
		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		com = parseInput(line);
		if(strcmp(com[0],"exit") == 0)	// When user uses exit command.
		{
			printf("Exiting shell...\n");
			break;
		}
		i = 0;
		while(com[i] != NULL)
		{
			//printf("Inside while loop with i = %d \n",i);
			if(strcmp(com[i],"&&") == 0)
			{
				flag = 1;
			}
			else if(strcmp(com[i],"##") == 0)
			{
				flag = 2;
			}
			else if(strcmp(com[i],">") == 0)
			{
				flag = 3;
			}
			i++;
		}

	 	if(flag == 1)
			executeParallelCommands(com);		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(flag == 2)
			executeSequentialCommands(com);	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(flag == 3)
			executeCommandRedirection(com);	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand(com);		// This function is invoked when user wants to run a single commands

	}

	return 0;
}
