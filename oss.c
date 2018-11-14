//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

#define MAX_FORKS 17
#define MAX_RAND 5

void sigint(int);
static void ALARMhandler();
void initTables();
void writeResultsToLog();
void ossClock();
void createProcess(int pidHolder[]);

int main(int argc, char* argv[]) {

    //signal handling config - ctrl-c and timeout
    signal(SIGINT, sigint);
    signal(SIGALRM, ALARMhandler);
    alarm(4);

    // shared memory config
    sharedMemoryConfig();

    // inits for max matrix and rescources table
    initTables();

    //####START CLOCK#####
    while(1){
        ossClock();         // increments clock
        createProcess(pidHolder);    // creates process every x amount of time
    }

    // clean shared memory
    shmdt(sysClockshmPtr);
    return 0;
}


void sigint(int a) {

    // write to log
    writeResultsToLog();

//    // reap children
//    int ii;
//    for(ii = 0; ii < 18; ii++) {
//        while ((pidHolder[ii] = waitpid(-1, NULL, WNOHANG)) > 0) {
//            //printf("child %d terminated\n", RDPtr->pids[ii]);
//        }
//    }

    // kill open forks
    for(int ii = 0; ii < 18; ii++){
        if(pidHolder[ii] != 0){
            signal(SIGQUIT, SIG_IGN);
            kill(pidHolder[ii], SIGQUIT);
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

//    // reap children
//    int ii;
//    for(ii = 0; ii < 18; ii++) {
//        while ((pidHolder[ii] = waitpid(-1, NULL, WNOHANG)) > 0) {
//            //printf("child %d terminated\n", RDPtr->pids[ii]);
//        }
//    }

    // kill open forks
    for(int ii = 0; ii < 18; ii++){
        if(pidHolder[ii] != 0){
            signal(SIGQUIT, SIG_IGN);
            kill(pidHolder[ii], SIGQUIT);
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

    // init max table
    fprintf(fp, "##### REQUEST TABLE #####\n");
    fprintf(fp, "     ");
    for(jj = 0; jj < 20; jj++){
        fprintf(fp, "R%02i ", jj);
    }

    fprintf(fp, "\n");

    for(ii = 0; ii < 18; ii++){
        fprintf(fp, "P%02i:", ii);
        for(jj = 0; jj < 20; jj++){
            fprintf(fp, "%4d", RDPtr->request[ii][jj]);
            totalLines++;
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");

    fprintf(fp, "##### JOBS #####\n");
    for(ii = 0; ii < MAX_FORKS; ii++){
        fprintf(fp, "%d    ", RDPtr->pidJob[ii]);
    }

    fprintf(fp, "\n");

    fprintf(fp, "##### TIME INTERVALS #####\n");
    for(ii = 0; ii < MAX_FORKS; ii++){
        fprintf(fp, "%d    ", randomClockTime[ii]);
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
            randNum = (rand() % MAX_RAND) + 1;
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

void createProcess(int pidHolder[]){
    int ii, jj;
    for(ii = 0; ii < MAX_FORKS; ii++){
        if(pidHolder[ii] == 0) {

            // creates random request table
            for(jj = 0; jj < 20; jj++){
                RDPtr->request[ii][jj] = (rand() % MAX_RAND) + 1;
            }

            // get job for process
            //      0 : Terminate
            //      1 : release one rescource
            //      2 : request one rescource
            int randPercent = (rand() % 100) + 1;

            if(randPercent >= 95){          // 5 percent chance terminate
                RDPtr->pidJob[ii] = 0;
            }else if (randPercent >= 60){   //35 percent chance release 1
                RDPtr->pidJob[ii] = 1;
            }else{                          // 60 percent chance request 1
                RDPtr->pidJob[ii] = 2;
            }

            // get clock time to make next request
            randomClockTime[ii] = (rand() % 500000000) + 1000000;

            // fork user
            if ((pidHolder[ii] = fork()) == 0) {
                execl("./user", "user", "connor", NULL);
            }



        }
    }
}
