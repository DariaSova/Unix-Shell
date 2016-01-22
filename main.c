#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>

void parseInputString(char *inputString, char **args, char* delimiter)
{
  int i =0;
  char *token;
  token = strtok(inputString, delimiter);
  while(token!=NULL)
  {
    args[i] = token;
    i++;
    token = strtok(NULL, delimiter);
  }
}

int main ( void )
{
  for (;;)
  {

  char *buf=0;
  char *data = getcwd(buf, 258);
  char *current_dir=strcat(data,">shell>>");
  char 	*cmd = readline (data);

    printf ("Got: [%s]\n", cmd);

    char** args = calloc(100, sizeof(char*));
    parseInputString(cmd, args, " \n");

    if(strcmp(args[0], "cd")==0)
    {
        int ret = chdir(args[1]);
        printf ("Directory: [%s]",args[1] );
    } else {
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
    free (current_dir);
  }
}
