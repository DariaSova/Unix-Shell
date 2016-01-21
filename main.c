#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>


void parseInputString(char *cmd, char **args)
{
  int i =0;
  char *token;
  token = strtok(cmd, " ");
  while(token!=NULL)
  {
    args[i] = token;
    i++;
    token = strtok(NULL, " ");
  }
}
int main ( void )
{
	for (;;)
	{
		char 	*cmd = readline ("shell>");
		printf ("Got: [%s]\n", cmd);


                char** args = calloc(100, sizeof(char*));
                parseInputString(cmd, args);

                int pid= fork();              //fork child

                if(pid==0){               //Child
                  execvp(args[0], args);
                  printf( "Child process could not do execvp\n");

                }else{                    //Parent
                  wait(NULL);
                  printf("Child exited\n");
                }

                free (cmd);
                free (args);
        }	
}
