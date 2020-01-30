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

//signal handler function declaration
void sigHandler2(int);

//variable to terminate loop
int isRunning = 1;

int main () {

    //sigaction struct declaration
    struct sigaction sa;
    sa.sa_handler = sigHandler2;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    //create the first child process
    if(fork() == 0){

      printf("spawnd child A PID# %d\n", getpid());
      while(isRunning){

	//sleep a random amt of time 1-5 secs
	sleep(rand() % 5 + 1);

	//randomize which signal to be sent
	if(rand() % 2){
	  kill(getppid(), SIGUSR1);
	  printf("singal SIGUSR1 from %d\n", getpid());
	}else{
	  kill(getppid(), SIGUSR2);
	  printf("singal SIGUSR2 from %d\n", getpid());
        }
	fflush(stdout);
      }
    }
    else{

      //create the second child
      if(fork() == 0){

        printf("spawnd child B PID# %d\n", getpid());
        while(isRunning){

  	  //sleep a random amt of time 1-5 secs
	  sleep(rand() % 5 + 1);

	  //randomize which signal to be sent
	  if(rand() % 2){
	    kill(getppid(), SIGUSR1);
	    printf("singal SIGUSR1 from %d\n", getpid());  
	  }else{
	    kill(getppid(), SIGUSR2);
	    printf("singal SIGUSR2 from %d\n", getpid());
	  }
	  fflush(stdout);
        }
      }
      
      //parent process
      else{

        while(isRunning){
	
          //wait for the child process to send a signal
	  printf("waiting...\t");
	  fflush(stdout);
          //re-install the handlers
          sigaction(SIGINT, &sa, NULL);
          sigaction(SIGUSR1, &sa, NULL);
          sigaction(SIGUSR2, &sa, NULL);
          pause();

        }
      }
    }

  return 0;
}

void sigHandler2 (int sigNum2) {
  if(sigNum2 == SIGUSR1)
    printf("Received: a SIGUSR1 signal\n");
  else if (sigNum2 == SIGUSR2)
   printf("Received: a SIGUSR2 signal\n");
  else{
    printf ("received an interrupt.\n");
    
    // terminate the loops in both processes 
    isRunning = 0;
    printf ("outta here.\n");
  }
}
