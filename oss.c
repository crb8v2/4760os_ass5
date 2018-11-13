//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

void sigint(int);
static void ALARMhandler();
void initTables();
void writeResultsToLog();
void ossClock();

int main(int argc, char* argv[]) {

    int clockStop = 0;

    //signal handling config - ctrl-c and timeout
    signal(SIGINT, sigint);
    signal(SIGALRM, ALARMhandler);
    alarm(4);

    // shared memory config
    sharedMemoryConfig();

    // inits for max matrix and rescources table
    initTables();

    char holder[20] = "Connor";


    // ##### FORKS #####
    if ((RDPtr->pids[0] = fork()) == 0){
        execl("./user", "user", "connor", holder, NULL);
    }

    if ((RDPtr->pids[1] = fork()) == 0){
        execl("./user", "user", "connor", holder,  NULL);
    }

    //####START CLOCK#####
    while(clockStop == 0){
        ossClock();

    }

    // clean shared memory
    shmdt(sysClockshmPtr);
    return 0;
}


void sigint(int a) {

    // write to log
    writeResultsToLog();

    // reap children
    int ii;
    for(ii = 0; ii < 18; ii++) {
        while ((RDPtr->pids[ii] = waitpid(-1, NULL, WNOHANG)) > 0) {
            //printf("child %d terminated\n", RDPtr->pids[ii]);
        }
    }

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

    // write to log
    writeResultsToLog();

    // reap children
    int ii;
    for(ii = 0; ii < 18; ii++) {
        while ((RDPtr->pids[ii] = waitpid(-1, NULL, WNOHANG)) > 0) {
            //printf("child %d terminated\n", RDPtr->pids[ii]);
        }
    }

    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(RDPtr);
    shmctl(RDshmid, IPC_RMID, NULL);

    printf("Timed out after 4 seconds.\n");
    exit(EXIT_SUCCESS);
}

// if ctrl-c, write data to log
void writeResultsToLog(){

    FILE *fp = fopen("log.txt", "a+");

    int ii, jj;

    // init max table
    fprintf(fp, "##### MAX TABLE #####\n");
    fprintf(fp, "     ");
    for(jj = 0; jj < 20; jj++){
        fprintf(fp, "R%02i ", jj);
    }

    fprintf(fp, "\n");

    for(ii = 0; ii < 18; ii++){
        fprintf(fp, "P%02i:", ii);
        for(jj = 0; jj < 20; jj++){
            fprintf(fp, "%4d", RDPtr->max[ii][jj]);
            totalLines++;
        }
        fprintf(fp, "\n");
    }

    // for initial rescources
    fprintf(fp, "\n##### RESCOURCES #####\n");
    fprintf(fp, "     ");
    for(jj = 0; jj < 20; jj++){
        fprintf(fp, "R%02i ", jj);
    }

    fprintf(fp, "\n    ");

    for(jj = 0; jj < 20; jj++){
        fprintf(fp, "%4d", RDPtr->rescources[jj]);
    }

    fprintf(fp, "\n");

            fclose(fp);
}

void initTables(){

    // init clock to random
    time_t t;
    srand((unsigned) time(&t));

    int randNum;

    // for max table
    int ii, jj;
    for(ii = 0; ii < 18; ii++){
        for(jj = 0; jj < 20; jj++){
            randNum = (rand() % 5) + 1;
            RDPtr->max[ii][jj] = randNum;
        }
    }

    // for initial rescources
    for(ii = 0; ii < 20; ii++){
        randNum = (rand() % 10) + 1;
        RDPtr->rescources[ii] = randNum;
    }
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
//    printf("time seconds:nanoseconds -> %d:%d\n", sysClockshmPtr->seconds, sysClockshmPtr->nanoseconds);

}