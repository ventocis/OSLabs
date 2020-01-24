#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void exitHandler (int);
void sigHandler2(int);

int isRunning = 1;

int main () {
    signal (SIGINT, exitHandler);
	signal(SIGUSR1, sigHandler2);
	signal(SIGUSR2, sigHandler2);
    int forkNum = fork();

	//parent thread
	if(forkNum > 0){
		while(isRunning){
			pause();
    		signal (SIGINT, exitHandler);
			signal(SIGUSR1, sigHandler2);
			signal(SIGUSR2, sigHandler2);
		}
	}
	//child thread
	else{
		signal (SIGINT, exitHandler);
		while(isRunning){
			sleep(rand() % 5 + 1);
			
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
    printf (" received an interrupt.\n");
    // this is where shutdown code would be inserted 
	isRunning = 0;
    printf ("outta here.\n");
}

void sigHandler2 (int sigNum2) {
	printf("Received: %d    \n", sigNum2);
}
