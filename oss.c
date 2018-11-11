//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

void sigint(int);
static void ALARMhandler();
void writeResultsToLog();
void ossClock();

int main(int argc, char* argv[]) {

    int clockStop = 0;

    //signal handling config - ctrl-c
    signal(SIGINT, sigint);

    //alarm times out if forks all do not return in 2 seconds
    signal(SIGALRM, ALARMhandler);
    alarm(4);

    // shared memory config
    sharedMemoryConfig();


    //####START CLOCK#####
    while(clockStop == 0){
        ossClock();
        printf("time seconds:nanoseconds -> %d:%d\n", sysClockshmPtr->seconds, sysClockshmPtr->nanoseconds);

    }


    // clean shared memory
    shmdt(sysClockshmPtr);
    return 0;
}


void sigint(int a) {

    // kill open forks

    // write to log
    writeResultsToLog();

    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(RDPtr);
    shmctl(RDshmid, IPC_RMID, NULL);

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
    int tester = 1;

    int i, j;
    fprintf(fp, "     ");
    for(j = 0; j < 20; j++){
        fprintf(fp, "R%02i ", j);
    }

    fprintf(fp, "\n");

    for(i = 0; i < 18; i++){
        fprintf(fp, "P%02i:", i);
        for(j = 0; j < 20; j++){
            fprintf(fp, "%4d", RDPtr->allocated[1][1]);
            totalLines++;
        }
        fprintf(fp, "\n");
    }

//    fprintf(stderr, "Proc: ");
//    for(i = 0; i < 18; i++){
//        fprintf(stderr, "%i ", (*shared).pidArr[i]);
//    }
//
//    fprintf(stderr, "\nblocked: ");
//    for(i = 0; i < 18; i++){
//        fprintf(stderr, "%i ", blocked[i]);
//    }
    fprintf(stderr, "\n");

            fclose(fp);
}

void ossClock(){

    int clockIncrement = 100000; // increment clock by 100,000 nanoseconds (1 microsecond)
    int rollover;

    if ((sysClockshmPtr->nanoseconds + clockIncrement) > 999999999){
        rollover = (sysClockshmPtr->nanoseconds + clockIncrement) - 999999999;
        sysClockshmPtr->seconds += 1;
        sysClockshmPtr->nanoseconds = rollover;
    } else {
        sysClockshmPtr->nanoseconds += clockIncrement;
    }

    //sleep here for easy managing
//    usleep(200000);

}