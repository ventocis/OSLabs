#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

int loopStatus = 1;
int numOfFiles = 0;
pthread_mutex_t mutex_totalTime;
int totalTime;

void *worker_thread(void *args){
  char* file = (char *) args;
  char buffer[100];
  int thread_time;
  snprintf(buffer, 100, "%s", file);

  int perc = (rand() % 10 + 1);

  if(perc < 9){
    sleep(1);
    thread_time = 1;
  }
  else{
    thread_time = (rand() % 4) + 7;
    sleep(thread_time);
  }

  pthread_mutex_lock(&mutex_totalTime);
  totalTime += thread_time;
  pthread_mutex_unlock(&mutex_totalTime);	

  printf("\nprocessed filename: %s", buffer);

  return NULL;
}

//signal handler function
void sigHandler2(int sig){
  printf("\nnum of files serviced: %d", numOfFiles);
  float avg = (float) totalTime / (float) numOfFiles;
  printf("\nAverage service time: %f seconds\n", avg);
  loopStatus = 0;
  exit(0);
}

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

    printf("Enter a file name: ");
    
    fgets(filename, 100, stdin);
    

    if((status = pthread_create (&tID, NULL, worker_thread, filename)) != 0){
      fprintf(stderr, "worker thread create error %d: %s\n", status, strerror(status));
      exit(1);
    }

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

  pthread_mutex_init(&mutex_totalTime, NULL);

  if((status = pthread_create (&dis_thread, NULL, dispatch_thread, NULL)) != 0){
    fprintf(stderr, "dispatch thread create error %d: %s\n", status, strerror(status));
    exit(1);
  }

  if ((status = pthread_join(dis_thread, &result)) != 0) {
    fprintf(stderr, "dispatch join error %d:%s\n", status, strerror(status));
    exit(1);
  }

  pthread_mutex_destroy(&mutex_totalTime);

  return 0;
}
