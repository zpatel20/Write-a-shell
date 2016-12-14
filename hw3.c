//include statements
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>    
#include <signal.h>
#include <sys/wait.h>

void sigHand(int signot)
{
  if(signot == SIGINT)
    printf(" SIGINT handled.\ncs361>");
  else if(signot == SIGTSTP)
    printf(" SIGSTOP handled.\ncs361>");
  fflush(stdout);
}

int TokenCount(char* usersinput)
{
  int count = 0;
  char* token;
  token = strtok(usersinput, " ");
  while(token != NULL){
    token = strtok(NULL, " ");
    count++;
  }
  return count;
}

//entire shell program for hw3
void startShell()
{
  printf("Shell for CS 361 started.\n");

  // handles SIGINT and SIGTSTP
  signal(SIGINT, sigHand);
  signal(SIGTSTP, sigHand);

  // begins the Shell.
  while(1)
    {
      // Display the required CS361 prompt.
      printf("cs361>");
      char input[100]; //input character maximum
      fgets(input, 100, stdin); //gets input
      char inputCopy[100];
      strcpy(inputCopy, input);

      char *newline = strchr(input, '\n');
      *newline = ' ';

      // Total amount of tokens, calculated from function above. 
      int numTokens = TokenCount(inputCopy);

      char* currentToken;
      char **tokens = (char **) malloc(sizeof(char *)
               *(numTokens+1));

      int redirection = 0;  // Redirection variable, 1 for yes, 0 or no.

      // Print the command for the user.
      currentToken = strtok(input, " ");
      int i;
      for(i = 0; i < numTokens; i++){
  tokens[i] = (char *) malloc(sizeof(char) *(strlen(currentToken) + 1));
  strcpy(tokens[i], currentToken);
  currentToken = strtok(NULL, " ");
  printf("cmdv[%d]: %s\n", i, tokens[i]);
      }

      tokens[numTokens] = (char *)0;

      // statement for exiting the condition
      if(strcmp(tokens[0], "exit") == 0){
  printf("Shell for CS 361 exiting now. \n");
  for(i = 0; i < numTokens + 1; i++)
    free(tokens[i]);
  free(tokens);
  exit(0);
      }

      // the Fork.
      pid_t pid = fork();

      if(pid < 0){
  printf("Failed to fork process!\n");
  exit(-1);
      }//
      else if(pid == 0){
  for(i = 1; i < numTokens - 1; i++){
    const char* filename = tokens[i+1];
    if(strcmp(tokens[i], ">>") == 0){
      printf("Appended to file successfully.\n");
      freopen(filename, "a", stdout);
      redirection = 1;
      break;
    }else if(strcmp(tokens[i], ">") == 0){
      printf("Wrote to file successfully.\n");
      freopen(filename, "w", stdout);
      redirection = 1;
      break;
    }else if(strcmp(tokens[i], "<") == 0){
      printf("Read from file successfully.\n");
      freopen(filename, "r", stdin);
      redirection = 1;
      break;
    }
  }

  i -= redirection;
  int numCleanTokens = i + 1;
  char** cleanToken = (char **) malloc(sizeof(char *) *(numCleanTokens + 1));
  int count;

  for(count = 0; count <= i; count++)
    cleanToken[count] = tokens[count];
  cleanToken[i+1] = (char *)0;

  execvp(cleanToken[0], cleanToken);

  char error[100];
  snprintf(error, 100, "%s", cleanToken[0]);
  perror(error);

  for(i = 0; i < numTokens + 1; i++)
    free(tokens);
  free(tokens);
  free(cleanToken);
  exit(0);
      }//
      else{
  printf("\n The Child PID: %d\n", pid);
  int st;
  wait(&st);
  if(WIFEXITED(st)){
    printf("The Child PID: %d has been exited with the status %d.\n", pid,
     WEXITSTATUS(st));
  }else if(WIFSIGNALED(st)){
    printf("The Child PID: %d was terminated with status %d.\n", pid, WTERMSIG(st));
  }
      }
      
      for(i = 0; i < numTokens; i++)
  free(tokens[i]);
      free(tokens);
    }
}

int main()
{ 
//runs entire shell
  startShell();
  return 0;
}

