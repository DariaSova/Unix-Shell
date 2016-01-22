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
  token = strtok(cmd, " \n");
  while(token!=NULL)
  {
    args[i] = token;
    i++;
    token = strtok(NULL, " \n");
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

    if(strcmp(args[0], "cd")==0)
    {
      int ret = chdir(args[1]);
      printf ("Directory: [%s]",args[1] );
    } else if ( strcmp(args[0], "pwd")==0) {
      char *buf;
      char *data = getcwd(buf, 258);
      printf("%s\n", data);
      free(buf);
    } else{
      int pid= fork();              //fork child

      if(pid==0){               //Child
        execvp(args[0], args);
        printf( "Child process could not do execvp\n");

      } else {                    //Parent
        wait(NULL);
        printf("Child exited\n");
      }
    }

    free (cmd);
    free (args);
  }
}
