#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define SIZE 16

int main (int argc, char* argv[]) {
	int status;
    long int i, loop, temp, *shmPtr;
    int shmId;
	int semId;
	int check;
	struct sembuf sops;
    pid_t pid;
	printf("argument: %s\n", argv[1]);
	
	//Use IPC_PRIVATE to request a new semaphore
	semId = semget(IPC_PRIVATE, 1, IPC_CREAT | S_IRUSR | S_IWUSR);
	if(semId == -1){
		perror("main: semget");
		exit(1);
	}

	check = semctl(semId, 0, SETVAL, 1);
	if(check == -1){
		perror("main: semctl");
		exit(1);
	}

	loop = atoi(argv[1]);
    
	if ((shmId = shmget (IPC_PRIVATE, SIZE,
                         IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) {
        perror ("i can't get no..\n");
        exit (1);
    }
    if ((shmPtr = shmat (shmId, 0, 0)) == (void *) -1) {
        perror ("can't attach\n");
        exit (1);
    }

    shmPtr[0] = 0;
    shmPtr[1] = 1;
    if (!(pid = fork ())) {
        for (i = 0; i < loop; i++) {
			sops.sem_num = 0;
			sops.sem_op = -1;
			sops.sem_flg = 0;
			if(semop(semId, &sops, 1) == -1)
				exit(EXIT_FAILURE);
			printf("in1\n");
			temp = shmPtr[1];
			shmPtr[1] = shmPtr[0];
			shmPtr[0] = temp;
			sops.sem_num = 0;
			sops.sem_op = 1;
			sops.sem_flg = 0;
			if(semop(semId, &sops, 1) == -1)
				exit(EXIT_FAILURE);
        }
        if (shmdt (shmPtr) < 0) {
            perror ("just can 't let go\n");
            exit (1);
        }
        exit (0);
    }
    else {
        for (i = 0; i < loop; i++) {
			sops.sem_num = 0;
			sops.sem_op = -1;
			sops.sem_flg = 0;
			if(semop(semId, &sops, 1) == -1)
				exit(EXIT_FAILURE);
			printf("in2");
			temp = shmPtr[1];
			shmPtr[1] = shmPtr[0];
			shmPtr[0] = temp;
			sops.sem_num = 0;
			sops.sem_op = 1;
			sops.sem_flg = 0;
			if(semop(semId, &sops, 1) == -1)
				exit(EXIT_FAILURE);
        }
    }

    wait (&status);
   
	printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);
	

	semctl(semId, 0, IPC_RMID);

    if (shmdt (shmPtr) < 0) {
        perror ("just can't let go\n");
        exit (1);
    }
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("can't deallocate\n");
        exit (1);
    }
    return 0;
}
