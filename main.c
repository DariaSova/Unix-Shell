#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

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

void change_dir(char **args)
{
      //int ret = chdir(args[1]);
      chdir(args[1]);
      printf ("Directory: [%s]",args[1] );
}

char* get_cmd()
{
  char *buf=0;
  char *data = getcwd(buf, 258);
  strcat(data,">shell>>");
  return readline (data);
}

void bg_list(struct bg_job *jobs_list, int counter)
{
  for(int i=0; i<counter, i++)
  {
    printf("%d:\t%s", i, jobs_list[i].command);
  }
}

//void check_ch_process()
//{
//}

int main ( void )
{

    struct bg_job
    {
      int pid;
      char *command;
    };

    typedef struct bg_job BG_JOB;
    BG_JOB bg_jobs[5];
    //counter for gb jobs
    int bgj_counter = 0;

  for (;;)
  {

    int status=0;
    pid_t pid;

    while((pid = waitpid(-1, &status, WNOHANG))>0)
    {
      printf ("\nChild is DONE");
      printf("[proc %d exited with code %d]\n", pid, WEXITSTATUS(status));
    }
    printf ("\nStatus pid %d", waitpid(-1, &status, WNOHANG));
    printf ("\nPID is %d\n",pid);
    //check_ch_process();
    char *cmd = get_cmd();


    printf ("Got: [%s]\n", cmd);

    char** args = calloc(100, sizeof(char*));
    parseInputString(cmd, args, " \n");


    if(strcmp(args[0], "cd")==0)
    {
      change_dir(args);
    } else {

      int pid= fork();              //fork child

      if(pid==0){               //Child
        if(strcmp(args[0], "bg")==0)
        {
          execvp(args[1], &args[1]);
          exit(1);
        }
        else
        {
          execvp(args[0], args);
          printf( "Child process could not do execvp\n");
          exit(1);
        }
      } else if (pid<0){
        printf( "Forking failed...\n");
      }



      else {                    //Parent

        if(strcmp(args[0], "bg")!=0)
        {
          printf("[proc %d started]\n", pid);
          waitpid(pid, NULL, 0);
        }
        else
        {
          //check the limit [5]
          if(bgj_counter<4)
          {
            bg_jobs[bgj_counter].pid = pid;
            bg_jobs[bgj_counter++].command = args[1];

          }
          else
          {
            printf("\nSoryy, you can not run more than 5 jobs on the background..");
            //run the 6th on the fg
            waitpid(pid, NULL, 0);
          }
          printf("[BG proc %d started]\n", pid);

          if (WIFEXITED(status)) {
            printf("exited, status=%d\n", WEXITSTATUS(status));
          } else if (WIFSIGNALED(status)) {
            printf("killed by signal %d\n", WTERMSIG(status));
          } else if (WIFSTOPPED(status)) {
            printf("stopped by signal %d\n", WSTOPSIG(status));
          } else if (WIFCONTINUED(status)) {
            printf("continued\n");
          }
        }
      }
    }

    free (cmd);
    free (args);
  }
  free(bg_jobs);
}
