#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

/********************************************************************
* This program is a multi threaded program that simulates a file
* server & displays stats about program usage. Once the user presses
* CTRL+C the program will exit cleanly
*
* @Author: Mazen Ashgar & Samuel Ventocilla
********************************************************************/


//variable to terminate loop
int loopStatus = 1;

//keep track of number of files accessed
int numOfFiles = 0;

//mutex is used to ensure that we can lock this variable while running other threads
//This helps to guard against race conditions
pthread_mutex_t mutex_totalTime;

//keep track of total time
int totalTime;

//this thread simulates accessing a file
void *worker_thread(void *args){
  char* file = (char *) args;
  char buffer[100];
  int thread_time;
  snprintf(buffer, 100, "%s", file);

  int perc = (rand() % 10 + 1);

  //have the thread sleep for a variable amount of time to simulate accessing a file
  if(perc < 9){
    sleep(1);
    thread_time = 1;
  }
  else{
    thread_time = (rand() % 4) + 7;
    sleep(thread_time);
  }

  //lock the total time variable, increment it by thread_time, and then unlock it
  pthread_mutex_lock(&mutex_totalTime);
  totalTime += thread_time;
  pthread_mutex_unlock(&mutex_totalTime);	

  printf("\nprocessed filename: %s", buffer);

  return NULL;
}

//signal handler function
void sigHandler2(int sig){
  //Display information to user
  printf("\nnum of files serviced: %d", numOfFiles);
  float avg = (float) totalTime / (float) numOfFiles;
  printf("\nAverage service time: %f seconds\n", avg);
  
  //set loopStatus to zero so other loops will exit
  loopStatus = 0;
  exit(0);
}

//Manages how the thread gets the user input and creates the worker thread
void *dispatch_thread(void *args){

  pthread_t tID;
  char filename[100];
  int status;
  
  //sigaction struct declaration
  struct sigaction sa; 
  sa.sa_handler = sigHandler2;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  while(loopStatus){
    //re-install the handlers
    sigaction(SIGINT, &sa, NULL);

	//get the file name from the user
    printf("Enter a file name: ");
    fgets(filename, 100, stdin);
    
	//create the worker thread
    if((status = pthread_create (&tID, NULL, worker_thread, filename)) != 0){
      fprintf(stderr, "worker thread create error %d: %s\n", status, strerror(status));
      exit(1);
    }

	//detach the worker thread from the parent thread so that the parent can continue listening for input
	//& doesn't have to wait to join the worker thread
    if ((status = pthread_detach(tID)) != 0) {
      fprintf(stderr, "worker detach error %d:%s\n", status, strerror(status));
      exit(1);
    }

    numOfFiles++;
  }

  return NULL;
}

int main(){

  pthread_t dis_thread;
  int status;
  void *result;

  //Create a mutex variable so that we can lock the variable while changing the value
  pthread_mutex_init(&mutex_totalTime, NULL);

  //create a thread to handle user input and dispatch worker threads
  if((status = pthread_create (&dis_thread, NULL, dispatch_thread, NULL)) != 0){
    fprintf(stderr, "dispatch thread create error %d: %s\n", status, strerror(status));
    exit(1);
  }

  //wait for thread to exit
  if ((status = pthread_join(dis_thread, &result)) != 0) {
    fprintf(stderr, "dispatch join error %d:%s\n", status, strerror(status));
    exit(1);
  }

  //destroy our mutex variable
  pthread_mutex_destroy(&mutex_totalTime);

  return 0;
}
