#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for WCOREDUMP() */
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

//buffer size and limit for input length
#define BUFFER_SIZE 513

//structure containing all the necessary information about a program that will be executed
typedef struct {
	char** arguments;
	size_t argCount;
	char* inputFile;
	char* outputFile;
	bool onBackground;
} programDesc;

//global buffer for read/parse communication between threads, protected by the monitor
char buf[BUFFER_SIZE];
//global variable defining whether the program should continue running, protected by the monitor
bool isRunning = true;
//global mutex, condition, thread
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bufferReady = PTHREAD_COND_INITIALIZER;
pthread_t readThread;
pthread_t execThread;
//global PID of executed process that is running in the foreground
pid_t foregroundPid = -1;
//global PID of executed process that is running in the background
pid_t childOnBg;

//monitor - wait on condition
void threadWait()
{
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&bufferReady, &mutex);
	pthread_mutex_unlock(&mutex);
}

//monitor - send condition signal
void threadSignal()
{
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&bufferReady);
	pthread_mutex_unlock(&mutex);
}

//SIGINT handler
void sigintHandler() {
	putchar('\n');
	//if anything is running on the foreground, kill it 
	if (foregroundPid != -1) {
		kill(foregroundPid, SIGINT);
		foregroundPid = -1;
	} 
	//else ignore SIGINT, do not exit yourself like a proper SHELL
	else {
		putchar('$');
		putchar(' ');
	}	
	fflush(stdout);	
}

//SIGCHLD handler
void sigchldHandler()
{
	int status;
	//wait just for process in the background - skip others, those are being waited elsewhere!
	pid_t p = waitpid(childOnBg, &status, WNOHANG);
	if (p == -1 || p != childOnBg)
		return;
	//defining how the background process exited
	if (WIFEXITED(status))
	{
		int es = WEXITSTATUS(status);
		printf("Background process ended - normal termination (exit code = %d)\n", es);
	}
	#ifdef WCOREDUMP
	else if (WCOREDUMP(status))
	{
		int es = WTERMSIG(status);
		printf("Background process ended - signal termination %s(signal = %d)\n","with core dump ", es);
	}
	#endif
	else if (WIFSIGNALED(status))
	{
		int es = WTERMSIG(status);
		printf("Background process ended - signal termination %s(signal = %d)\n","", es);
	}
	else
	{
		printf("Background process ended - unknown type of termination\n");
	}
	putchar('$');
	putchar(' ');
	fflush(stdout);	
}

//free the allocated memory from the program description
void freeMemory(programDesc* program) {
	if (program->arguments != NULL)
	{
		for (char **p = program->arguments; *p; ++p)
			free(*p);
		free(program->arguments);
	}
	if (program->outputFile != NULL)
		free(program->outputFile);
	if (program->inputFile != NULL)
		free(program->inputFile);
	return;
}

//parse the buffer and create structured description of a program that is about to be executed
programDesc parseInfo() {
	programDesc program;
	program.outputFile = program.inputFile = NULL;
	program.onBackground = false;
	char* arr;
	int j = 0;
	bool isInput;
	//tokenize the input with space
	arr = strtok(buf, " ");
	size_t wordLength;
	program.arguments = malloc(sizeof(char*));
	size_t n = 1;
	while (arr != NULL)
 	{
 		//if the first word of user's input is exit, the program will end
 		if (!strcmp(arr, "exit") && j == 0)
 		{
 			isRunning = false;
 			break;
 		}
 		j++;
 		//check the word for the background run symbol, output symbol and input symbol
		for (int i = 0; i < strlen(arr); i++) {
			if (arr[i] == '&') {
				program.onBackground = true;
				arr = strtok(NULL, " ");
				break;
			}
			//input and output found
			else if (arr[i] == '>' || arr[i] == '<') {
				isInput = arr[i] == '<';
				//if the symbol was found at the end of a word, we need to get the next word as the specified file
				if (i + 1 == strlen(arr)) {
					arr = strtok (NULL, " ");
					if (arr != NULL) {
						wordLength = strlen(arr);
						if (!isInput) {
							program.outputFile = (char*)malloc((wordLength+1)*sizeof(char));
							memcpy(program.outputFile, arr, wordLength);
							program.outputFile[wordLength] = '\0';
						}
						else {
							program.inputFile = (char*)malloc((wordLength+1)*sizeof(char));
							memcpy(program.inputFile, arr, wordLength);
							program.inputFile[wordLength] = '\0';
						}
						break;
					}
					//nor input nor output specified
					else {
						fprintf(stderr, "The output file is missing.\n");
						break;
					}
					
				}
				//the input/output symbol was found in the middle of a word, so everything in front of the symbol as a valid program argument and everything after is a file name 
				else {
					char **tmp = realloc(program.arguments, (n+1)*sizeof(char*));
					program.arguments = tmp;
					++n;
					program.arguments[ n - 2 ] = malloc(i+1);
					//copy the argument before the symbol
					memcpy(program.arguments[n-2], arr, i);
					program.arguments[n-2][i] = '\0';
					i++;
					//file name length
					wordLength = strlen(arr) - i;
					if (arr[i-1] == '<') {
						program.inputFile = (char*)malloc((wordLength+1)*sizeof(char));
						memcpy(program.inputFile, &arr[i], wordLength);
						program.inputFile[wordLength] = '\0';
					}
					else {
						program.outputFile = (char*)malloc((wordLength+1)*sizeof(char));
						memcpy(program.outputFile, &arr[i], wordLength);
						program.outputFile[wordLength] = '\0';
					}					
					break;
				}
			}
			//no symbol found, this means currently scanned word is a valid argument -> copy to arguments
			else if (i + 1 == strlen(arr)) {
				char **tmp = realloc(program.arguments, (n+1)*sizeof(char*));
				program.arguments = tmp;
				++n;
				program.arguments[n-2] = malloc(strlen(arr) + 1);
				strcpy(program.arguments[n-2], arr);
				program.arguments[n-2][strlen(arr)] = '\0';
			}
		}
		arr = strtok(NULL, " ");
	}
	program.arguments[n-1] = NULL;
	program.argCount = n-1;
	return program;
}

//thread that does the parsing and the executing
void* parseExec() {
	while (isRunning) {
		//wait for the reading thread to fill the buffer
		threadWait();
		programDesc program = parseInfo();
		//if the global variable of running changed while reading, stop and wake up the other thread
		if (!isRunning) {
			freeMemory(&program);
			threadSignal();
			return NULL;
		}
		//ignore SIGINT handler for the background run process
		struct sigaction sigIgn;
		sigIgn.sa_handler = SIG_IGN;
		sigIgn.sa_flags = 0;
		sigemptyset(&sigIgn.sa_mask);
		pid_t pid = fork();
		if (pid > 0) {
			//parent process either waits for its child which will be executing specified program, or continues running if the specified program should run in background
			if (!program.onBackground) {
				foregroundPid = pid;
				waitpid(pid, NULL, 0);
			}
			//set the child PID so SIGINT can kill the child
			else {
				childOnBg = pid;
			}	
		}
		else if (pid == 0) {
			int inputFile, outputFile;
			//redirect stdin to specified file
			if (program.inputFile) {
				inputFile = open(program.inputFile, O_RDONLY);
				if (inputFile == -1) {
					fprintf(stderr, "Failed to open input file\n");
					exit(1);
				}
				if (dup2(inputFile, 1) == -1) {
					fprintf(stderr, "Failed to redirect the input file to stdin\n");
					exit(1);
				}
			}
			//redirect stdout to specified file
			if (program.outputFile) {
				outputFile = open(program.outputFile, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				if (outputFile == -1) {
					fprintf(stderr, "Failed to open output file\n");
					exit(1);
				}
				if (dup2(outputFile, 1) == -1) {
					fprintf(stderr, "Failed to redirect the stdout to output file\n");
					exit(1);
				}
			}
			//ignore SIGINT on the account of running on background
			if (program.onBackground == true) {
				sigaction(SIGINT, &sigIgn, NULL);
			}
			execvp(program.arguments[0], program.arguments);
			//the program returns here only if an error while execvp occurs
			perror("Error");
			if (program.inputFile)
				close(inputFile);
			if (program.outputFile)
				close(outputFile);
			exit(1);
		}
		else {
			fprintf(stderr, "Failed to fork.\n");
			isRunning = false;
			break;
		}
		foregroundPid = -1;
		//send signal to reading thread, free memory
		threadSignal();
		freeMemory(&program);
	}	
	return NULL;
}

//reading thread actions
void* listen() {
	ssize_t bytes;
	while (isRunning) {
		putchar('$');
		putchar(' ');
		fflush(stdout);
		bytes = read(0, buf, sizeof(buf));
		//if the input is too long, throw an error
		if (bytes >= BUFFER_SIZE) {
			fprintf(stderr, "The input is too long.\n");
			//need to empty the stdin
			while ((bytes = read(0, buf, sizeof(buf))) > 0) {
				if (buf[bytes-1] == '\n')
					break;
			}
		}
		else {
			buf[bytes - 1] = '\0';
			//buffer is ready, wake up the parsing thread and wait for it to read again
			threadSignal();
			threadWait();
		}
	}
	return NULL;
}


int main(int argc, char *argv[])
{
	//create SIGCHLD handler
	struct sigaction sa, sigInt;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigchldHandler;
	sigaction(SIGCHLD, &sa, NULL);

	//create SIGINT handler
	sigInt.sa_handler = sigintHandler;
	sigInt.sa_flags = 0;
	sigemptyset(&sigInt.sa_mask);
	sigaction(SIGINT, &sigInt, NULL);
	//create threads, then join
	while (pthread_create(&readThread, NULL, listen, NULL) != 0) {
		if (errno == EAGAIN)
			continue;
		fprintf(stderr, "Cannot create parsing thread: %d\n", errno);
		return 1;
	}

	while (pthread_create(&execThread, NULL, parseExec, NULL) != 0)	{
		if (errno == EAGAIN)
			continue;
		fprintf(stderr, "Cannot create executing thread: %d\n", errno);
		return 1;
	}
	int err;
	if ((err = pthread_join(execThread, NULL)) != 0) {
		fprintf(stderr, "Cannot join parsing thread: %d\n", err);
	}
	if ((err = pthread_join(readThread, NULL)) != 0) {
		fprintf(stderr, "Cannot join executing thread: %d\n", err);
	}
	return 0;
}
