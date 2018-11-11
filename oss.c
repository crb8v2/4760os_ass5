//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

void sigint(int);
static void ALARMhandler();
void writeResultsToLog();

int main(int argc, char* argv[]) {

    int clockStop = 0;

    //signal handling config - ctrl-c
    signal(SIGINT, sigint);

    //alarm times out if forks all do not return in 2 seconds
    signal(SIGALRM, ALARMhandler);
    alarm(4);

    // shared memory config
    sharedMemoryConfig();

    //####################
    //####START CLOCK#####
    //####################
    // os clock
    while(clockStop == 0){

    }


    // clean shared memory
    shmdt(sysClockshmPtr);
    return 0;
}


void sigint(int a) {

    // kill open forks

    // write to log

    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(RDPtr);
    shmctl(RDshmid, IPC_RMID, NULL);

    writeResultsToLog();

    printf("^C caught\n");
    exit(0);
}

// alarm magic
static void ALARMhandler() {

    // kill forks

    //write to log

    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(RDPtr);
    shmctl(RDshmid, IPC_RMID, NULL);

    printf("Timed out after 4 seconds.\n");
    exit(EXIT_SUCCESS);
}

// if process completes, write data to log
void writeResultsToLog(){
            FILE *fp = fopen("log.txt", "a+");
            fprintf(fp, "writing to log\n");
            fclose(fp);
}