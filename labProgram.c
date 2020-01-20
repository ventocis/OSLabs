#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

/**********************************************
*This program is a simple shell that takes the
*user input and execute it in a child process.
*
*@Author: Samuel Ventocilla & Mazen Ashgar
**********************************************/
int main(int argc, char** argv){

  //declare and initialize variables
  char command[100];
  char** commandList = NULL;
  char* quit = "quit\n";
  int pid = -2;
  int status;
  struct rusage resources;

  //keep getting user input until "quit"
  while (1) {

    //clear the command list
    commandList = NULL;	

    //prompt user for input and store input
    printf("\nEnter your command or type quit to exit\n");
    fgets(command, 100, stdin);

    //exit if user types quit
    if (strcmp(command, quit) == 0){
      free(commandList);
      exit(0);
    }

    //remove newline from string
    command[strlen(command) - 1] = 0;

    int count = 0;

    //Tokenize string & store into dynamically allocated string
    for (char * p = strtok(command, " "); p != NULL; p = strtok(NULL, " ")){
      count++;
      commandList = (char**)realloc(commandList, (count + 1)*sizeof(*commandList));
      commandList[count-1] = (char*)malloc(sizeof(p));
      strcpy(commandList[count-1], p);
    }

    //saved the PID to a variable so that we can get info using it in commands later
    pid = fork();

    //child runs this
    if(pid == 0){
      if(execvp(commandList[0], commandList) == -1){
        printf("Incorrect command\n");
      }
      exit(0);
    }

    //parent runs this
    else{

      //wait for child process to end
      waitpid(-1 ,&status, 0);

      //get rusage from child process
      getrusage(RUSAGE_CHILDREN, &resources);

      //print the usage time to the user
      printf("CPU usage: %d ms", resources.ru_utime.tv_usec);
      }
    }
   return 0;
}
