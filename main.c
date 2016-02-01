#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>


struct Bg_job
{
  int pid;
  char *status;
  char *command;
};

typedef struct Bg_job BG_JOB;
int MAX_JOBS_NUMBER=5;

int parseInputString(char *inputString, char **args, char* delimiter)
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
  return i-1;
}

void change_dir(char **args, int params)
{
  int ret = -1;
  if(params==0 || strcmp(args[1],"~")==0)
  {
    ret = chdir(getenv("HOME"));
  }
  else
  {
    ret = chdir(args[1]);
  }
  if(ret==-1)
  {
    printf ("Sorry, directory [%s] does not exist\n", args[1] );
  }
}

char* get_cmd()
{
  char *buf=0;
  char *data = getcwd(buf, 256);
  strcat(data,":shell>>");
  return readline (data);
}

void bg_list(struct Bg_job *jobs_list, int counter)
{
  for(int i=0; i<MAX_JOBS_NUMBER; i++)
  {
    if(jobs_list[i].command!=NULL)
    {
      printf("%d[%s]:\t%s\n", i, jobs_list[i].status, jobs_list[i].command);
    }
  }
  printf("Total Background jobs: %d\n", counter);
}

void initialize_jobs(struct Bg_job *jobs_list)
{
  for(int i=0; i<MAX_JOBS_NUMBER; i++)
  {
    jobs_list[i].command=NULL;
  }
}

int kill_bg(struct Bg_job *jobs_list, char args, int bgj_counter)
{
  int kill_id = atoi(&args); 
  int kill_pid = jobs_list[kill_id].pid;
  int ret = kill(kill_pid, SIGTERM);

  if(ret==0)
  {
    jobs_list[kill_id].command =NULL;
    bgj_counter--;
    printf("Process %d was successfully terminated\n", kill_id);
  }
  else if(ret==-1)
  {
    printf("Error: the process could not be terminated\n");
  }
  return bgj_counter;
}

void stop_bg(struct Bg_job *jobs_list, char args)
{
  int job_id = atoi(&args);
  int job_pid = jobs_list[job_id].pid;
  int ret = kill(job_pid, SIGSTOP);

  if(ret==0)
  {
    jobs_list[job_id].status= "S";
    printf("\nProcess %d\n was stopped\n", job_id);
  }
  else
  {
    printf("\nError: process %d\n could not be stopped\n", job_id);
  }
}

void start_bg(struct Bg_job *jobs_list, char args)
{
  int job_id = atoi(&args);
  int job_pid = jobs_list[job_id].pid;
  int ret = kill(job_pid, SIGCONT);

  if(ret==0)
  {
    jobs_list[job_id].status= "R";
    printf("Process %d is now running\n", job_id);
  }
  else
  {
    printf("Error: process %d could not be resumed\n", job_id);
  }
}

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
              jobs_list[i].status = "R";
              strcpy(jobs_list[i].command, args[1]);
              bgj_counter++;
            } else
            {
              printf("\nSoryy, you can not run more than 5 jobs on the background..");
              //kill the 6th job
              kill(ch_pid, SIGSTOP);
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
