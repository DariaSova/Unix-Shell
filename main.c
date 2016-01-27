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

//void bg_list(struct bg_job* jobs_list, int n)
//{
//  for(int i=0; i<n; i++)
//  {
//    if(jobs_list[i]!=NULL)
//    {
//      printf("%d:\t%s", i, jobs_list[i].command);
//    }
//  }
//}

//void check_ch_process()
//{
//}

int main ( void )
{
  int MAX_JOBS_NUMBER=5;
  struct bg_job
  {
    int pid;
    int terminated;
    char *command;
  };

  typedef struct bg_job BG_JOB;
  BG_JOB jobs_list[MAX_JOBS_NUMBER];
  //counter for gb jobs
  int bgj_counter = 0;

  for(int i=0; i<MAX_JOBS_NUMBER; i++)
  {
    jobs_list[i].command=NULL;
  }

  for (;;)
  {

    int status=0;
    pid_t pid;

    if((pid = waitpid(-1, &status, WNOHANG))>0)
    {
      printf ("\nChild is DONE");
      printf("[proc %d exited with code %d]\n", pid, WEXITSTATUS(status));

      int i=0;
      while(jobs_list[i].pid!=pid)
      {
        i++;
      }
      jobs_list[i].command=NULL;
      bgj_counter--;
    }

    //printf ("\nStatus pid %d", waitpid(-1, &status, WNOHANG));
    //printf ("\nPID is %d\n",pid);
    //check_ch_process();
    char *cmd = get_cmd();
    printf ("Got: [%s]\n", cmd);

    char** args = calloc(100, sizeof(char*));
    parseInputString(cmd, args, " \n");

    if(strcmp(args[0], "cd")==0)
    {
      change_dir(args);
    }
    else if(strcmp(args[0], "bglist")==0)
    {
      //print current bg jobs
      for(int i=0; i<MAX_JOBS_NUMBER; i++)
      {
        if(jobs_list[i].command!=NULL)
        {
          printf("%d:\t%s\n", i, jobs_list[i].command);
        }
      }
      printf("Total Background jobs: %d\n", bgj_counter);
    }
    else {
      printf("Forking!!!!");
      int ch_pid= fork();              //fork child
      if(ch_pid >=0)
      {
        if(ch_pid==0)
        {               //Child
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
        } else 
        {                    //Parent
          if(strcmp(args[0], "bg")==0)
          {
            //check the limit of gb jobs [5]
            if(bgj_counter < MAX_JOBS_NUMBER)
            {
              int i=0;
              while(jobs_list[i].command!=NULL&&i<MAX_JOBS_NUMBER)
              {
                i++;
              }
              //allocate when initialize
              jobs_list[i].command = calloc(100, sizeof(char*));
              //find an empty slot in the array
              //jobs_list[bgj_counter].terminated=0;
              jobs_list[i].pid = ch_pid;
              strcpy(jobs_list[i].command, args[1]);
              bgj_counter++;
            } else
            {
              printf("\nSoryy, you can not run more than 5 jobs on the background..");
              //run the 6th on the fg
              waitpid(pid, NULL, 0);
            }
            // printf("[BG proc %d started]\n", pid);
            if (WIFEXITED(status)) {
              printf("exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
              printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
              printf("stopped by signal %d\n", WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
              printf("continued\n");
            }
          } else
          {
            printf("[proc %d started]\n", pid);
            waitpid(ch_pid, NULL, 0);
          }
        }
      } else //ch_pid<0
      {
        printf( "Forking failed...\n");
      }
    }

    free (cmd);
    free (args);
  }
  free(jobs_list);
}
