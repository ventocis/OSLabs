#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

/******************************************
*This program will communicate between
*a parent and a child process sending
*signals and having them handled until
*the user press CTRL+C
*
*@Author: Samuel Ventocilla & Mazen Ashgar
*******************************************/

//signal handlers function declaration
void exitHandler (int);
void sigHandler2(int);

//variable to terminate loop
int isRunning = 1;

int main () {

    //install handlers to the parent process
    signal (SIGINT, exitHandler);
    signal(SIGUSR1, sigHandler2);
    signal(SIGUSR2, sigHandler2);
    
    //create the child process
    int forkNum = fork();

    //print child pid
    if(forkNum > 0)
      printf("spawnd child PID# %d\n", forkNum);

    //parent process
    if(forkNum > 0){
      while(isRunning){
	
        //wait for the child process to send a signal
	printf("waiting...\t");
        pause();

        //re-install the handlers
        signal(SIGINT, exitHandler);
        signal(SIGUSR1, sigHandler2);
        signal(SIGUSR2, sigHandler2);
      }
    }

    //child process
    else{
	
      //install handler
      signal (SIGINT, exitHandler);
      while(isRunning){

	//sleep a random amt of time 1-5 secs
	sleep(rand() % 5 + 1);

	//randomize which signal to be sent
	if(rand() % 2)
	  kill(getppid(), SIGUSR1);
	else
	  kill(getppid(), SIGUSR2);
      }
      printf("child thread closing\n");
   }

  return 0;
}

void exitHandler (int sigNum) {

  printf ("received an interrupt.\n");

  // terminate the loops in both processes 
  isRunning = 0;
  printf ("outta here.\n");
}

void sigHandler2 (int sigNum2) {
  if(sigNum2 == 30)
    printf("Received: a SIGUSR1 signal\n");
  else
    printf("Received: a SIGUSR2 signal\n");
}
