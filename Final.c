// closh.c - COSC 315, Winter 2020
// Zachary, Tanvir, Carson

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define TRUE 1
#define FALSE 0


// tokenize the command string into arguments - do not modify
void readCmdTokens(char* cmd, char** cmdTokens) {
    cmd[strlen(cmd) - 1] = '\0'; // drop trailing newline
    int i = 0;
    cmdTokens[i] = strtok(cmd, " "); // tokenize on spaces
    while (cmdTokens[i++] && i < sizeof(cmdTokens)) {
        cmdTokens[i] = strtok(NULL, " ");
    }
}

// read one character of input, then discard up to the newline - do not modify
char readChar() {
    char c = getchar();
    while (getchar() != '\n');
    return c;
}
void timeOut(int childId, int timeout) {
    if (timeout > 0) {
        sleep(timeout);
        printf("\nKilling process with id %d...\n", childId);
        kill (childId, SIGKILL);
        
    }
    else {
        waitpid(childId, 0, 0);
        }
}
// called when parallel == TRUE
void parallelExec(int counter, char** cmdTokens)
{
        if(counter > 0){
        int cid = fork();
        printf("Process ID %d",getpid());
        //if in the child process
        //if child from the fork, recursively call parallelExec, with decremented counter and args
            if (cid==0){
        // after all the children are pushed to the recursion stack, all of the execute the program individually at the same time.
            parallelExec(counter-1,cmdTokens);
            execvp(cmdTokens[0],cmdTokens);
        
        //if the parent from fork, do nothing
            }
            else { }
        }
    //if the count > 0 condition is no longer true, return to the main function
    else{ return; }
}

void sequentialExec(int count, char** cmdTokens, int timeout) {
	int parentId = getpid();
	printf("Parent process ID = %d\n", parentId);
	for (int i = 0; i<count; i++){ // For loop iterates for each process to start
		int cid = fork(); // Create child
		if (cid == 0) { // If I'm the child
			printf("Starting new process with ID = %d...\n",getpid());
			execvp(cmdTokens[0], cmdTokens); // Execute process
			printf("Failed.\n"); //Won't reach here unless failure
		}
		else { // If I'm the parent
		sleep (1); // Wait as parent for child to start
		timeOut(cid, timeout);
		printf("\nParent done waiting.\n"); 
		}
	}
	
}
// main method - program entry point
int main() {
    char cmd[81]; // array of chars (a string)
    char* cmdTokens[20]; // array of strings
    int count; // number of times to execute command
    int parallel; // whether to run in parallel or sequentially
    int timeout; // max seconds to run set of commands (parallel) or each command (sequentially)
    
    while (TRUE) { // main shell input loop
        
        // begin parsing code - do not modify
        printf("closh> ");
        fgets(cmd, sizeof(cmd), stdin);
        if (cmd[0] == '\n') continue;
        readCmdTokens(cmd, cmdTokens);
        do {
            printf("  count> ");
            count = readChar() - '0';
        } while (count <= 0 || count > 9);
        
        printf("  [p]arallel or [s]equential> ");
        parallel = (readChar() == 'p') ? TRUE : FALSE;
        do {
            printf("  timeout> ");
            timeout = readChar() - '0';
        } while (timeout < 0 || timeout > 9);
        // end parsing code
        
        if (parallel == FALSE)
		sequentialExec(count, cmdTokens, timeout);

        if(parallel==TRUE){
            int cid = fork();
            printf("Process ID %d",getpid());
            if (cid==0) {
                parallelExec(count,cmdTokens);
                wait(NULL);
                kill(cid,SIGKILL);
            }
            else {
                if(timeout==0){
                    waitpid(cid,0,0);
                }
                else{
                sleep(1);
                timeOut(cid,timeout);
                }}
                    }
        // doesn't return unless the calling failed
        // printf("Can't execute %s\n", cmdTokens[0]); // only reached if running the program failed
        exit(1);        
    }
}

