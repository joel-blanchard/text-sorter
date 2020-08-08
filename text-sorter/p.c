/*
Joel Blanchard
Program 9
*/

/*
Whenever a flying reindeer attempts to run,it will first check the semaphore and if it is free (unlocked) it will lock the semaphore and continue; otherwise, it must wait until the semaphore is unlocked. A flying reindeer that gets to runwill then generate a random number N between 1 and 10 (inclusive) and read N characters from thefile, pass its name (program1, etc) followed by those characters to Santa Claus, and addthe value of N to the value in the shared memory
*/

#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h> 
#include <sys/mman.h>
#include <signal.h>

#define KEY (7685)

int main(int argc, char *argv[]) {
	//check for the correct number of command line arguments
	if (argc != 4) {
		printf("\nThree command line argument expected.\n");
		printf("\nFound %d command line arguments in program\n", argc - 1);
		return 1;
	}
	
	union semun {
		int val;
		struct semid_ds *buf;
		ushort * array;
	} argument;
	
	argument.val = 1;

	const int SIZE = 4096; 
	const char* name = "sharedMemory";
	const char* name2 = "sharedDone";
	
	struct sembuf operations[1];

	char str[50];
	char chars[15];
	char *buf;
	char *finalBuf;
	int c;
	int retval;
	int id;
	int finalChars;
	int value;
	char *data;
	srand(time(NULL));
	int filePosition;



	int fdRead;
	int fdWrite;
	
	
	
	
	
	char* fileName = argv[0];
	FILE *inFile;
	inFile = fopen(fileName, "r");

	int i = atoi(argv[1]);
	fdRead = atoi(argv[2]);
	fdWrite = atoi(argv[3]);
	
	
	id = semget(KEY, 1, 0666);

	//prepare shared memory
	int shm_fd; 
	void* shmem; 
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, SIZE);
	shmem = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
	
	//prepare shared memory for done
	int shm_fd2; 
	void* sharedDone; 
	shm_fd2 = shm_open(name2, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd2, SIZE);
	sharedDone = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd2, 0);
	

	char processName[] = "_p0";
	processName[2] = *argv[1];
	//printf("\nName: %s\n", processName);
	
	//printf("\nENTERED PROCESS %s\n", processName);



	//while loop
	bool done = false;
	int k;
	while (done == false && strcmp((char*)sharedDone,"true") != 0) {
		
		//SEM WAIT
		operations[0].sem_num = 0;
		operations[0].sem_op = -1;
		operations[0].sem_flg = 0;
		retval = semop(id, operations, 1);
		
		
		//generate a random number N between 1 and 10 (inclusive)
		int r = rand() % 10 + 1;
		//printf("\nNumber: %d\n", r);

		//read N characters from the file
		k = 0;
		buf = (char*)malloc(r + 1 * sizeof(char));
		filePosition = atoi((char*)shmem);
		fseek(inFile, filePosition, SEEK_SET);
		while (k < r && (c = fgetc(inFile)) != EOF){
			//read single char at a time
			if (c != '\n') {
				buf[k] = c; 	//add char to chars array
				k++;
			}
		}
		if (k != r && done == false) {
			finalChars = k;
			done = true;
			finalBuf = (char*)malloc(k * sizeof(char));
			int j = 0;
			while (j < k) {
				finalBuf[j] = buf[j];
				j++;
			}

			strcpy(str, processName);
			strcat(str, finalBuf);
			//printf("\nWill send this String: %s\n", str);
			write(fdWrite, str, strlen(str) + 1); //pass str to main process
			free(buf);
			str[0] = '\0'; 		//Empty str
			free(finalBuf);
		}
		else  if (done == false){
			strcpy(str, processName);
			strcat(str, buf);
			if (strcmp((char*)sharedDone,"true") != 0)
			{
				write(fdWrite, str, strlen(str) + 1); //pass str to main process
			}

			free(buf);
			str[0] = '\0'; 		//Empty str
		}
		else {
			//End process
		}

		//add the value of N to the value in the shared memory

		value = atoi((char*)shmem);
		value = value + k;
		sprintf((char*)shmem, "%d", value);
		
		if (done == true) {
			sprintf((char*)sharedDone, "%s", "true");
		}

	//end while loop
	}

	shm_unlink(name);
	shm_unlink(name2);

	return 0;

}




