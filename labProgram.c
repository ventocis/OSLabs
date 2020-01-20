#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char** argv){
	char command[100];
	char** commandList = NULL;
	char* quit = "quit\n";
	int pid = -2;
	char** pCommandList;

	while (1) {
		printf("\nEnter your command or type quit to exit\n");
		fgets(command, 100, stdin);

		//exit if user types quit
		if (strcmp(command, quit) == 0)
			exit(0);

		//remove newline from string
		command[strlen(command) - 1] = 0;

		int cout = 0
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
				execvp(commandList[0], commandList);
				exit(0);
		}
		//parent runs this
		else{
		//Tried to make this work, but wasn't successful. Not sure if it's the right track or not
		/*
			char pCommand[100] = "grep ctxt /proc/";
			char strPid[50];
			sprintf(strPid, "%d", pid);
			strcat(pCommand, strPid);
			strcat(pCommand, "/status");

			wait(NULL);
			int check = 0;
			for (char * pP = strtok(pCommand, " "); pP != NULL; pP = strtok(NULL, " ")){
				check++;
				pCommandList = (char**)realloc(pCommandList, (check+1)*sizeof(*pCommandList));
				pCommandList[check-1] = (char*)malloc(sizeof(pP));
				strcpy(pCommandList[check-1], pP);
			}
			execvp(pCommandList[0], pCommandList);
			*/
			wait(NULL);
		}
	}
}
