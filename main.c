#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "constants.h"
#include "bg_job.h"
#include "common.h"
#include "parent_process.h"

int main ( void )
{
  BG_JOB jobs_list[MAX_JOBS_NUMBER];
  //counter for gb jobs
  int bgj_counter = 0;
  initialize_jobs(jobs_list);

  for (;;)
  {

    int status=0;
    pid_t pid;

    while((pid = waitpid(-1, &status, WNOHANG))>0)
    {
      if (WIFSIGNALED(status)) {
        printf("The process was killed by signal %d\n", WTERMSIG(status));
      }
      else
      {
        printf ("\nChild process %d is now terminated\n", pid);
        bgj_counter--;
      }

      int i=0;
      while(jobs_list[i].pid!=pid)
      {
        i++;
      }
      jobs_list[i].command=NULL;
    }

    int params_number =0;
    //prompt - get user's input
    char *cmd = get_cmd();
    while(strcmp(cmd, "")==0)
    {
      printf("No command was entered\n");
      cmd = get_cmd();
    }

    char** args = calloc(100, sizeof(char*));
    params_number = parseInputString(cmd, args, " \n");

    if(strcmp(args[0], "cd")==0)
    {
      change_dir(args, params_number);
    }
    else if(strcmp(args[0], "bglist")==0)
    {
      //print current bg jobs
      bg_list(jobs_list, bgj_counter);
    }
    else if (strcmp(args[0], "bgkill")==0)
    {
      //kill a bg job
      bgj_counter = kill_bg(jobs_list, *args[1], bgj_counter);
    } else if(strcmp(args[0], "stop")==0)
    {
      stop_bg(jobs_list, *args[1]);

    } else if(strcmp(args[0], "start")==0)
    {
      start_bg(jobs_list, *args[1]);
    }
    else {
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
            printf( "Child process could not do execute\n");
            exit(1);
          }
        } else 
        {
          //Parent
          bgj_counter = parent_job(jobs_list, bgj_counter, pid, ch_pid, args);
        }
      } else //ch_pid<0
      {
        printf( "Forking failed...\n");
      }
    }

    free (cmd);
    free (args);
  }
  freeStruct(jobs_list);
}
