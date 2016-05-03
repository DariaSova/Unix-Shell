void freeStruct (struct Bg_job *list)
{
  for(int i=0 ; i<MAX_JOBS_NUMBER; i++ )
  {
    free(list[i].status);
    free(list[i].command);
  }
  free(list);
}

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
  free(token);
  return i-1;
}

char* get_cmd()
{
  char *buf=0;
  char *data = getcwd(buf, 256);
  strcat(data,":shell>>");
  free(buf);
  return readline (data);
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
