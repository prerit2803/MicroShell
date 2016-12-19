/******************************************************************************
 *
 *  File Name........: main.c
 *
 *  Description......: Simple driver program for ush's parser
 *
 *  Author...........: Vincent W. Freeh
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "parse.h"

char *end="end";
char *h,*builtIn[]={"where","cd","echo","logout","pwd","setenv","unsetenv","nice"}, WorkDir[1024];
extern char **environ;
char * findPath(char*);
void ExecBuiltIn(Cmd);
void checkExec(Cmd);
void commandExec(Cmd);

void checkExec(Cmd c){
  int i;
  for (i = 0; i < 8; ++i){
    if (strcmp(c->args[0],builtIn[i])==0)
    {
      ExecBuiltIn(c);
      return;
    }
  }
  commandExec(c);
}

void commandExec(Cmd c){
  pid_t pid;
  char *path=(char*)malloc(sizeof(char)*1000);
  //printf("cmd: %s\n", c->args[0]);
  if (c==NULL)
  {
    return;
  }
  if (strstr(c->args[0],"/")==NULL)
  {
    path=findPath(c->args[0]);
   // printf("in null part\n");
  }
  else if(c->args[0][0]=='/'){
    path=c->args[0];
    //printf("in abs part\n");
  }
  else{
    strcpy(path, WorkDir);
    strcat(path,"/");
    strcat(path,c->args[0]);
    //printf("in cat part\n");
  }

  
 // printf("Check for cmd: %s\n", path);
  if (path==NULL)
  {
    return;
  }
  switch(pid=fork())
  {
    case -1://error while forking
      return;
    break;
    case 0://child process
      if (execv(path,c->args)==-1)
      {
        if (errno==2)
          printf("command not found\n");
        else if(errno==13)
          printf("permission denied\n");
        else
          printf("Error while executing command\n");
        exit(EXIT_FAILURE);
      }
      else
        exit(EXIT_SUCCESS);
    default://parent process
      waitpid(pid, NULL, 0);
        return;
  }
}
char* findPath(char *c)
{
  char *path, env[1000], *res, *cmd;
  char delims[] = ":";
  struct stat st;
  //size allocation
  path=(char*)malloc(sizeof(char)*1000);
  res = malloc(1000);
  cmd = malloc(1000);
  //constructing the path
  strcpy(cmd,"/");
  strcpy(env,getenv("PATH"));
  strcat(cmd,c);
  //extracting each path
  res = strtok( env, delims );
  //iterating over each path from PATH variable
  while( res != NULL )
  {
    strcpy(path,res);
    strcat(path,cmd);
    //path received from result
    if(stat(path,&st)==0)
    {//found the correct path
      return path;
    }
    res = strtok( NULL, delims );
  }
  //not found
  return NULL;
}
void ExecWhere(Cmd c){
  int i;
  char *path;
  if (c->nargs > 2 || c->args[1]==NULL)
  {
    //if no argument to where is specified
    return;
  }
  for (i = 0; i < 8; ++i){
    if (strcmp(c->args[1],builtIn[i])==0)
    {
      printf("%s is a shell built-in\n",builtIn[i]);
    }
  }
  path=findPath(c->args[1]);
  printf("%s\n", path);
}

void ExecCd(Cmd c){
  if (c->nargs>3)
  {
    //if more than 3 arguments is passed to cd command it is an error
    return;
  }
  if (c->nargs<2)
  {
    //if no argument is specified to the command then also error
    return;
  }
  if (chdir(c->args[1])!=0)
  {// permission not granted to open
    printf("Permission Denied\n");
  }
  else{
    //after successful directory change, set the current directory variable
    getcwd(WorkDir,sizeof(char)*1024);
  }
}

void ExecEcho(Cmd c){
  int i;
  if (c->args[1]==NULL)
  {
    //no word is sepcified
    return;
  }
  for (i = 1; i < c->nargs; ++i)
  {
    //reading and printing the word
    printf("%s ", c->args[i]);
  }
  printf("\n");
}

void ExecLogout(Cmd c){
  //exiting
  exit(EXIT_SUCCESS);
}

void ExecPwd(Cmd c){
  //printing current working directory
  printf("%s\n", WorkDir);
}

void ExecSetenv(Cmd c){
  char **env;
  if (c->nargs==1)
  { //printing all the environment
    env=environ;
    while(*env!=NULL)
    {
      printf("%s\n", *env);
      env++;
    }
  }
  else
  {//setting the environment
    if(setenv(c->args[1],c->args[2],1)==-1){
      printf("Error while setting environment\n");
    }
  }
}

void ExecUnsetenv(Cmd c){
  if (c->args[1]==NULL)
  {
    //VAR is not specfied
    return;
  }
  unsetenv(c->args[1]);
}

void ExecNice(Cmd c){

}

void ExecBuiltIn(Cmd c){
  if (strcmp(c->args[0],"where")==0)
    ExecWhere(c);
  else if(strcmp(c->args[0],"cd")==0)
    ExecCd(c);
  else if(strcmp(c->args[0],"echo")==0)
    ExecEcho(c);
  else if(strcmp(c->args[0],"logout")==0)
    ExecLogout(c);
  else if(strcmp(c->args[0],"pwd")==0)
    ExecPwd(c);
  else if(strcmp(c->args[0],"setenv")==0)
    ExecSetenv(c);
  else if(strcmp(c->args[0],"unsetenv")==0)
    ExecUnsetenv(c);
  else if(strcmp(c->args[0],"nice")==0)
    ExecNice(c);
}
void outSetup(Cmd c,int aoCheck, int errCheck)
{
  int out,bkp_err;
  if (aoCheck==0)
  {
    out = open(c->outfile, O_WRONLY | O_CREAT | O_TRUNC,
      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  }
  else{
    out = open(c->outfile, O_WRONLY | O_APPEND | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  }
  if(out<0){
      printf("Error while opening output file\n");
      return;
  }
  else{
    dup2(out, STDOUT_FILENO);
  }
  if (errCheck==1)
  {
    bkp_err = dup( STDERR_FILENO);
    dup2(out, STDERR_FILENO);
  }
  checkExec(c);
  if (errCheck==1){
    dup2(bkp_err, STDERR_FILENO);
    close(bkp_err);
  }
}
void fileRedirection(Cmd c)
{
  int bkp_in, bkp_out, bkp_err, inputFile, outputFile;
  //for given input file
  if (c->in == Tin)
  {
    bkp_in=dup(STDIN_FILENO);
    inputFile=open(c->infile,O_RDONLY);
    if (inputFile<0)
    {
      printf("Error while opening input file: %s\n", c->infile);
      return;
    }
    dup2(inputFile,STDIN_FILENO);
  }
  //for output ao=0
  if (c->out==Tout)
  {
    outSetup(c,0,0);

  }
  else if (c->out==ToutErr)
  {
            outSetup(c,0,1);
  }
  else if (c->out==Tapp)
  {
      outSetup(c,1,0);
  }
  else if (c->out==TappErr)
  {
            outSetup(c,1,1);
  }
  if( c->in==Tin ){
        dup2(bkp_in, STDIN_FILENO);
        close(bkp_in); 
        close(inputFile);
    }
    
    dup2(bkp_out, STDOUT_FILENO);
    close(bkp_out);
            
    close(outputFile);
    fflush(stdout);
}

int commandProcessing(Cmd c){
  int pid, i, newFD[2], oldFD[2], old, inputFile, bkp_in;
  //When no pipe is associated
    /*if (c->in!=Tpipe && c->in!=TpipeErr &&  c->out!=Tpipe && c->out!=TpipeErr)
    {
      commandProcessing(c);
      continue;
    }
     //if there is a next cmd
        if( c->out==Tpipe || c->out==TpipeErr ){
                pipe(newFD);
        }
    */
  for (i = 0; i < 8; ++i){
    if (strcmp(c->args[0],builtIn[i])==0)
    {
      ExecBuiltIn(c);
      return 1;
    }
  }
  //forking
  /*pid=fork();
  if (pid<0)
  {
    printf("Error in creating Child\n");
    exit(EXIT_FAILURE);
  }
  if (pid==0){
    //in child
    if (old==1)
    {//if there is a previous command
      dup2(oldFD[0],STDIN_FILENO);
      close(oldFD[0]);
      close(oldFD[1]);
    }
    if( c->out==Tpipe || c->out==TpipeErr ){   //if there's a next cmd
      close(newFD[0]);
      dup2(newFD[1], STDOUT_FILENO);
      if(c->out==TpipeErr)
        dup2(newFD[1], STDERR_FILENO);
        close(newFD[1]);
        }*/

  if (c->out == Tout || c->out == Tapp || c->out == ToutErr || c->out == TappErr){
    fileRedirection(c);
  return;
  }
  else if( c->in==Tin ){
        int stdin_bkp = dup(STDIN_FILENO);
        int inFile = open(c->infile, O_RDONLY);
        
        dup2(inFile, STDIN_FILENO);
        commandExec(c);
        
        dup2(stdin_bkp, STDIN_FILENO);
        close(stdin_bkp); 
        close(inFile);
        
        fflush(stdout);
    }else{
    commandExec(c);
  exit(EXIT_SUCCESS);
  }/*
  else{
    //in parent
    if (old==1)
    {
      close(oldFD[0]);
      close(oldFD[1]);
    }
    if( c->out==Tpipe || c->out==TpipeErr ){   if there's a next cmd 
      oldFD[0] = newFD[0];
      oldFD[1] = newFD[1];
      old=1;
      }else{
        old=0;
        }
     wait(NULL);
  }*/
}

static void pipeProcessing(Pipe p){
  int i = 0;
  Cmd c;

  if ( p == NULL )
    return;
  for ( c = p->head; c != NULL; c = c->next ) {
    
    //Detected End
    if (strcmp(c->args[0],"end")==0)
    {
      //exit the ushell
      exit(EXIT_SUCCESS);
    }
    
    i = commandProcessing(c);
    if (i==1)
      continue;
   
  }
  pipeProcessing(p->next);
  return;
}

void init(Pipe p)
{
  int bkp_in,bkp_out;
  
  bkp_in=dup(STDIN_FILENO);

  getcwd(WorkDir,sizeof(char)*1024);

  h=getenv("HOME");
  strcat(h,"/.ushrc");
  int rcpath=open(h,O_RDONLY);
  if(rcpath<0)
    return;
  

  if(dup2(rcpath,STDIN_FILENO)<0)
    return;
  close(rcpath);
  p=parse();
  
  

  while(strcmp(p->head->args[0],end)!=0)
  {
    p=parse();
    if (p==NULL)
      return;
    pipeProcessing(p);
    freePipe(p);
  }
  if(dup2(bkp_in,STDIN_FILENO))
    return;
  close(bkp_in);

}
int main(int argc, char *argv[])
{
  Pipe p;

  //signal handling

  int bkp_in, bkp_out;
  char *host=malloc(sizeof(char)*256), *h;
  if (host==NULL)
  {
    return;
  }
  bkp_in=dup(STDIN_FILENO);
  bkp_out=dup(STDOUT_FILENO);
  //Shel init
  init(p);

  dup2(bkp_in,STDIN_FILENO);
  dup2(bkp_out,STDOUT_FILENO);

  gethostname(host,30);
  while ( 1 ) {
    if( isatty(STDIN_FILENO) )
      printf("%s%% ", host);
    fflush(stdout);
    p = parse();
    if (p==NULL)
      return;
    pipeProcessing(p);
    freePipe(p);
  }
  return 0;
}

/*........................ end of main.c ....................................*/
