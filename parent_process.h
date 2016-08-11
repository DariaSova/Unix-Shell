int parent_job(struct Bg_job *jobs_list, int bgj_counter, pid_t pid, pid_t ch_pid, char** args)
{
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
      jobs_list[i].pid = ch_pid;
      jobs_list[i].status = "R";
      strcpy(jobs_list[i].command, args[1]);
      bgj_counter++;
    } else
    {
      printf("\nSorry, you can not run more than 5 jobs on the background..\n");
      //kill the 6th job
      kill(ch_pid, SIGSTOP);
    }
  } else
  {
    printf("Process with pid %d started\n", pid);
    waitpid(ch_pid, NULL, 0);
  }
  return bgj_counter;
}
