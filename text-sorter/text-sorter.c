/*
Joel Blanchard
Program 9
*/

/*
Santa Claus(parent) will get the data filename as a command-line argument. Santa Claus will then create the 9 pipes, 1 semaphore and 1 shared memory(initialized to 0). Santa Claus will then make sure the semaphore is free (unlocked). Santa Claus will then fork & exe the nine flying reindeer and pass on to them the data filename, the pipe identifiers, semaphore identifier, and the shared memoryidentifier as command-line arguments
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
	//get data filename as command line argument
	if (argc != 2) {
		printf("\nOne command line argument expected.\n");
		printf("\nFound %d command line arguments in program\n\nPlease include file name\n", argc - 1);
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

	char str1[100];
	char str2[100];
	char str3[100];
	char str4[100];
	char str5[100];
	char str6[100];
	char str7[100];
	char str8[100];
	char str9[100];
	strcpy(str1, "");
	strcpy(str2, "");
	strcpy(str3, "");
	strcpy(str4, "");
	strcpy(str5, "");
	strcpy(str6, "");
	strcpy(str7, "");
	strcpy(str8, "");
	strcpy(str9, "");
	
	pid_t pid1 = -1 ;
	pid_t pid2 = -1 ;
	pid_t pid3 = -1 ;
	pid_t pid4 = -1 ;
	pid_t pid5 = -1 ;
	pid_t pid6 = -1 ;
	pid_t pid7 = -1 ;
	pid_t pid8 = -1 ;
	pid_t pid9 = -1 ;

	
	
	int data;
	int id;
	int retval;
	char* fileName = argv[1];

	FILE *outFile;
	outFile = fopen("text-sorter.out", "w");

	//create 9 pipes
	int fd1[2];
	int fd2[2];
	int fd3[2];
	int fd4[2];
	int fd5[2];
	int fd6[2];
	int fd7[2];
	int fd8[2];
	int fd9[2];
	
	if (pipe(fd1) == -1) { 
		fprintf(stderr, "Pipe 1 Failed" ); 
		return 1; 
	} 
	if (pipe(fd2) == -1) { 
		fprintf(stderr, "Pipe 2 Failed" ); 
		return 1; 
	}
	if (pipe(fd3) == -1) { 
		fprintf(stderr, "Pipe 3 Failed" ); 
		return 1; 
	} 
	if (pipe(fd4) == -1) { 
		fprintf(stderr, "Pipe 4 Failed" ); 
		return 1; 
	}
	if (pipe(fd5) == -1) { 
		fprintf(stderr, "Pipe 5 Failed" ); 
		return 1; 
	} 
	if (pipe(fd6) == -1) { 
		fprintf(stderr, "Pipe 6 Failed" ); 
		return 1; 
	}
	if (pipe(fd7) == -1) { 
		fprintf(stderr, "Pipe 7 Failed" ); 
		return 1; 
	} 
	if (pipe(fd8) == -1) { 
		fprintf(stderr, "Pipe 8 Failed" ); 
		return 1; 
	}
	if (pipe(fd9) == -1) { 
		fprintf(stderr, "Pipe 9 Failed" ); 
		return 1; 
	}

	fcntl(fd1[0], F_SETFL, O_NONBLOCK);
	fcntl(fd2[0], F_SETFL, O_NONBLOCK);
	fcntl(fd3[0], F_SETFL, O_NONBLOCK);
	fcntl(fd4[0], F_SETFL, O_NONBLOCK);
	fcntl(fd5[0], F_SETFL, O_NONBLOCK);
	fcntl(fd6[0], F_SETFL, O_NONBLOCK);
	fcntl(fd7[0], F_SETFL, O_NONBLOCK);
	fcntl(fd8[0], F_SETFL, O_NONBLOCK);
	fcntl(fd9[0], F_SETFL, O_NONBLOCK);

	

	//create semaphore
	id = semget(KEY, 1, 0666 | IPC_CREAT);
	semctl(id, 0, SETVAL, argument);
	

	//create shared memory (initialized to 0)
	int shm_fd; 
	void* shmem; 
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, SIZE);
	shmem = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

	sprintf((char*)shmem, "%s", "0");
	
	//create sharedDone
	int shm_fd2; 
	void* sharedDone; 
	shm_fd2 = shm_open(name2, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd2, SIZE);
	sharedDone = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd2, 0);

	sprintf((char*)sharedDone, "%s", "false");
	//printf("\nShared Done: %s\n", (char*)sharedDone);

	//printf("\nCreated shared memory\n");

	//fork and exec 9 processes (pass filename and identifiers for pipe and shared memory as arguments)
	int i = 1;
	char arg1[50];
	char arg2[50];
	char arg3[50];
	char arg4[50];
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd1[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd1[1]);

	pid1 = fork();
	if(pid1 == 0) {
	
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG

		char *const argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}

	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd2[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd2[1]);

	pid2 = fork();
	if(pid2 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
	
		char *const argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}


	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd3[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd3[1]);
	
	pid3 = fork();
	if(pid3 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
			
		char *argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}
	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd4[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd4[1]);
	
	pid4 = fork();
	if(pid4 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
			
		char *argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}
	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd5[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd5[1]);
	
	pid5 = fork();
	if(pid5 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
			
		char *argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}
	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd6[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd6[1]);
	
	pid6 = fork();
	if(pid6 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
			
		char *argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}
	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd7[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd7[1]);
	
	pid7 = fork();
	if(pid7 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
			
		char *argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}
	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd8[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd8[1]);
	
	pid8 = fork();
	if(pid8 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
			
		char *argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}
	i++;
	
	snprintf(arg1, sizeof(arg1), "%d", i);
	snprintf(arg2, sizeof(arg2), "%d", fd9[0]);
	snprintf(arg3, sizeof(arg3), "%d", fd9[1]);
	
	pid9 = fork();
	if(pid9 == 0) {
			
		//printf("\nExecuting program %d\n", i); //USE FOR DEBUG
			
		char *argv[] = {fileName, arg1, arg2, arg3, NULL};
		execvp("./p", argv);
	}



	//Close write sides

	close(fd1[1]);
	close(fd2[1]);
	close(fd3[1]);
	close(fd4[1]);
	close(fd5[1]);
	close(fd6[1]);
	close(fd7[1]);
	close(fd8[1]);
	close(fd9[1]);
	




	int found = 0;
	while (strcmp((char*)sharedDone,"true") != 0) { //while EOF not reached, signal not sent
		//Read from pipes

		if (read(fd1[0], str1, 50)) {
			if (strlen(str1) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str1);
				found = 1;
				//printf("\nREAD %s FROM PIPE 1\n", str1);
				strcpy(str1, "");
			}
		}
		if (read(fd2[0], str2, 50)) {
			if (strlen(str2) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str2);
				found = 2;
				//printf("\nREAD FROM PIPE 2\n");
				strcpy(str2, "");
			}
		}
		if (read(fd3[0], str3, 50)) {
			if (strlen(str3) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str3);
				found = 3;
				//printf("\nREAD FROM PIPE 3\n");
				strcpy(str3, "");
			}
		}
		if (read(fd4[0], str4, 50)) {
			if (strlen(str4) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str4);
				found = 4;
				//printf("\nREAD FROM PIPE 4\n");
				strcpy(str4, "");
			}
		}
		if (read(fd5[0], str5, 50)) {
			if (strlen(str5) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str5);
				found = 5;
				//printf("\nREAD FROM PIPE 5\n");
				strcpy(str5, "");
			}
		}
		if (read(fd6[0], str6, 50)) {
			if (strlen(str6) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str6);
				found = 6;
				//printf("\nREAD FROM PIPE 6\n");
				strcpy(str6, "");
			}
		}
		if (read(fd7[0], str7, 50)) {
			if (strlen(str7) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str7);
				found = 7;
				//printf("\nREAD FROM PIPE 7\n");
				strcpy(str7, "");
			}
		}
		if (read(fd8[0], str8, 50)) {
			if (strlen(str8) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str8);
				found = 8;
				//printf("\nREAD FROM PIPE 8\n");
				strcpy(str8, "");
			}
		}
		if (read(fd9[0], str9, 50)) {
			//printf("\nTHIS IS THE STRING READ FROM PIPE 9: -%s-\n", str9);
			if (strlen(str9) > 3) {
				fflush(outFile);
				fprintf(outFile, "%s", str9);
				found = 9;
				//printf("\nREAD FROM PIPE 9\n");
				strcpy(str9, "");
			}
		}
		else {
			//sleep(1);
			//printf("\nFound no pipes to read\n");
		}
		if (found > 0) {
			//UNLOCK SEMAPHORE AFTER READING FROM PIPE
			operations[0].sem_num = 0;
			operations[0].sem_op = +1;
			operations[0].sem_flg = 0;
			retval = semop(id, operations, 1);
			
			//printf("\nMain process unlocked semaphore\n");
	
			//printf("\nMain process read from pipe %d\n", found);
			found = 0;
		}
	}

	//Kill each process after done signal
	kill(pid1,SIGKILL);
	kill(pid2,SIGKILL);
	kill(pid3,SIGKILL);
	kill(pid4,SIGKILL);
	kill(pid5,SIGKILL);
	kill(pid6,SIGKILL);
	kill(pid7,SIGKILL);
	kill(pid8,SIGKILL);
	kill(pid9,SIGKILL);

	
	fflush(outFile);
	fprintf(outFile, "%s", (char*)shmem);

	fflush(outFile);
	fclose(outFile);
	shm_unlink(name);
	shm_unlink(name2);
	semctl(id, 0, IPC_RMID, 0);

	return 0;
}

















