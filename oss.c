//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

#define MAX_FORKS 18
#define MAX_RAND 5

void sigint(int);
static void ALARMhandler();
void initTables();
void writeResultsToLog();
void ossClock();
void createProcess(int pidHolder[]);
void checkMsgQ();
void processJob(int);
void logProcDetected(int, int);
void logBlocked(int, int);
void logAllocated(int, int);
void logAllocatedMatrix();
void logForceAllocatedMatrix();

int main(int argc, char* argv[]) {

    // shared memory config
    sharedMemoryConfig();

    // msgQueue config
    messageQueueConfig();

    //signal handling config - ctrl-c and timeout
    signal(SIGINT, sigint);
    signal(SIGALRM, ALARMhandler);
    alarm(2);

    // inits for max matrix and rescources table
    initTables();

    //####START CLOCK#####
    while(1){
        ossClock();                 // increments clock
        createProcess(pidHolder);   // creates process every x amount of time
        checkMsgQ();                // check for message q for request

        // if rescources changed
            // run bakers alg

        // print allocated table
        logAllocatedMatrix();
    }

    // clean shared memory (never gets here)
    shmdt(sysClockshmPtr);
    return 0;
}


void sigint(int a) {

    // write to log
//    writeResultsToLog();
    logForceAllocatedMatrix();

//    // reap children
//    int ii;
//    for(ii = 0; ii < 18; ii++) {
//        while ((pidHolder[ii] = waitpid(-1, NULL, WNOHANG)) > 0) {
//            //printf("child %d terminated\n", RDPtr->pids[ii]);
//        }
//    }

    // kill open forks
    int ii;
    for(ii = 0; ii < 18; ii++){
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
    // to destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    printf("^C caught\n");
    exit(0);
}

static void ALARMhandler() {

    // write to log
//    writeResultsToLog();
    logForceAllocatedMatrix();

//    // reap children
//    int ii;
//    for(ii = 0; ii < 18; ii++) {
//        while ((pidHolder[ii] = waitpid(-1, NULL, WNOHANG)) > 0) {
//            //printf("child %d terminated\n", RDPtr->pids[ii]);
//        }
//    }

    // kill open forks
    int ii;
    for(ii = 0; ii < 18; ii++){
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
    // to destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    printf("Timed out after 2 seconds.\n");
    exit(EXIT_SUCCESS);
}

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

    // init max table
    fprintf(fp, "##### ALLOCATED #####\n");
    fprintf(fp, "     ");
    for(jj = 0; jj < 20; jj++){
        fprintf(fp, "R%02i ", jj);
    }

    fprintf(fp, "\n");

    for(ii = 0; ii < 18; ii++){
        fprintf(fp, "P%02i:", ii);
        for(jj = 0; jj < 20; jj++){
            fprintf(fp, "%4d", RDPtr->allocated[ii][jj]);
            totalLines++;
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");

    fprintf(fp, "##### BLOCKED QUEUE #####\n");
    for(ii = 0; ii < MAX_FORKS; ii++){
        fprintf(fp, "%d    ", blockedQueue[ii]);
    }



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
//    usleep(200);
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

            int randPercent = (rand() % 100) + 1;

            if(randPercent >= 91){          // 5 percent chance terminate
                RDPtr->pidJob[ii] = 0;
            }else if (randPercent >= 60){   //35 percent chance release 1
                RDPtr->pidJob[ii] = 1;
            }else{                          // 60 percent chance request 1
                RDPtr->pidJob[ii] = 2;
            }

            // get clock time to make next request
            randomClockTime[ii] = (rand() % 500000000) + 1000000;

            char stashbox[10];

            sprintf(stashbox, "%d", randomClockTime[ii]);

            // fork user
            if ((pidHolder[ii] = fork()) == 0) {
                execl("./user", stashbox, NULL);
            }

        }
    }
}

void checkMsgQ(){
    int pidPass;

    // msgrcv to receive message
    msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT);

    // display the message
    if(message.mesg_text[0] != '0') {
        pidPass = atoi(message.mesg_text);
//        printf("Data Received is : %s \n", message.mesg_text);
        // do the work of the process
        processJob(pidPass);
    }

    strcpy(message.mesg_text, "0");
}

// process detected, do the work
void processJob(int pid){

    int jobNumber;
    int procNumber;
    int rescourceRequestNumber;

    //get job number for pid
    int ii;
    for(ii = 0; ii < 18; ii++){
        if(pidHolder[ii] == pid){
            jobNumber = RDPtr->pidJob[ii];
            procNumber = ii;
            rescourceRequestNumber = (rand() % 20);
            // write to log file
            logProcDetected(procNumber, rescourceRequestNumber);
        }
    }

    //case statement on jobNumber
//    printf("job right here!!!!!!!!!!!!!!!!!     %d\n", jobNumber);
    if(jobNumber == 1 || jobNumber == 2){   // allocate

        //allocate if rescources are avail, if not send to blocked queue
        if(RDPtr->request[procNumber][rescourceRequestNumber] <= RDPtr->rescources[rescourceRequestNumber]){
            //update the allocated table
            RDPtr->allocated[procNumber][rescourceRequestNumber] = RDPtr->request[procNumber][rescourceRequestNumber];
            //update rescources
            RDPtr->rescources[rescourceRequestNumber] -= RDPtr->request[procNumber][rescourceRequestNumber];
            logAllocated(procNumber, rescourceRequestNumber);
        } else {
            //assign to blocked queue
            int ii;
            int posted = 0;
            for(ii = 0; ii < 18; ii++){
                if(blockedQueue[ii] == 0){
                    blockedQueue[ii] = pid;
                    posted = 1;
                    // write blocked to log
                    logBlocked(procNumber, rescourceRequestNumber);
                }
                if(posted == 1){
                    ii = 18;
                }
            }
        }

    } else if(jobNumber == 0) {             // terminate
        // kill process
//        kill(pid, SIGKILL);
        requestTimeReached = 1;
        // write empty to pid block
        pidHolder[procNumber] = 0;
    }


}

void logProcDetected(int procNumber, int reqNum){

    FILE *fp = fopen("log.txt", "a+");
    fprintf(fp, "OS has detected Process P%d requesting R%d at time %d:%d\n",
            procNumber, reqNum, sysClockshmPtr->seconds, sysClockshmPtr->nanoseconds);
    fclose(fp);

}

void logAllocated(int procNumber, int reqNum){

    FILE *fp = fopen("log.txt", "a+");
    fprintf(fp, "OS granting P%d request R%d at time %d:%d\n",
            procNumber, reqNum, sysClockshmPtr->seconds, sysClockshmPtr->nanoseconds);
    fclose(fp);

}

void logBlocked(int procNumber, int reqNum){

    FILE *fp = fopen("log.txt", "a+");

    fprintf(fp, "OS blocking P%d for requesting R%d at time %d:%d\n",
            procNumber, reqNum, sysClockshmPtr->seconds, sysClockshmPtr->nanoseconds);

    fclose(fp);

}

void logAllocatedMatrix(){

    int lines;
    int ch = 0;

    FILE *fp = fopen("log.txt", "a+");

    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }

    // print table every 20 lines
    if(lines % 20 == 0) {

    fprintf(fp, "\n");

    // init max table
    fprintf(fp, "##### ALLOCATED #####\n");
    fprintf(fp, "     ");
    int ii, jj;
    for(jj = 0; jj < 20; jj++){
        fprintf(fp, "R%02i ", jj);
    }

    fprintf(fp, "\n");

    for(ii = 0; ii < 18; ii++){
        fprintf(fp, "P%02i:", ii);
        for(jj = 0; jj < 20; jj++){
            fprintf(fp, "%4d", RDPtr->allocated[ii][jj]);
            totalLines++;
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");
    }

    fclose(fp);

}

void logForceAllocatedMatrix() {

    int ii,jj;

    FILE *fp = fopen("log.txt", "a+");

    fprintf(fp, "\n");

    // init max table
    fprintf(fp, "##### ALLOCATED #####\n");
    fprintf(fp, "     ");
    for(jj = 0; jj < 20; jj++){
        fprintf(fp, "R%02i ", jj);
    }

    fprintf(fp, "\n");

    for(ii = 0; ii < 18; ii++){
        fprintf(fp, "P%02i:", ii);
        for(jj = 0; jj < 20; jj++){
            fprintf(fp, "%4d", RDPtr->allocated[ii][jj]);
            totalLines++;
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");

    fclose(fp);


}