struct Bg_job
{
  int pid;
  char *status;
  char *command;
};
typedef struct Bg_job BG_JOB;

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
    printf("Error: the process %d could not be terminated\n", kill_id);
  }
  return bgj_counter;
}

void stop_bg(struct Bg_job *jobs_list, char args)
{
  int job_id = atoi(&args);
  int job_pid = jobs_list[job_id].pid;

  if(strcmp(jobs_list[job_id].status, "S") == 0)
  {
    printf("Error: process %d has been alsready stopped\n", job_id);
  }
  else
  {
    int ret = kill(job_pid, SIGSTOP);

    if(ret==0)
    {
      jobs_list[job_id].status= "S";
      printf("Process %d was stopped\n", job_id);
    }
    else
    {
      printf("Error: process %d could not be stopped\n", job_id);
    }
  }
}

void start_bg(struct Bg_job *jobs_list, char args)
{
  int job_id = atoi(&args);
  int job_pid = jobs_list[job_id].pid;

  if(strcmp(jobs_list[job_id].status, "R") == 0)
  {
    printf("Error: process %d is currently running\n", job_id);
  }
  else
  {
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
}
