//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

int main(int argc, char* argv[]){

    // shared memory config
    sharedMemoryConfig();

    // variables
    int timeIncrement, rollover;
    int seconds = sysClockshmPtr->seconds;
    int nanoseconds = sysClockshmPtr->nanoseconds;
    int requestTimeReached = 0;

    // get argv values for randClockTime sent in
    timeIncrement = atoi(argv[0]);

    // adjust request timer

    if ((nanoseconds + timeIncrement) > 999999999){
        rollover = (nanoseconds + timeIncrement) - 999999999;
        seconds += 1;
        nanoseconds = rollover;
    } else {
        nanoseconds += timeIncrement;
    }

    // case statement of operation to perform

    printf("We in this. %d     %d\n", getpid(), timeIncrement);

    while(requestTimeReached == 0){

        if(sysClockshmPtr->seconds >= seconds && sysClockshmPtr->nanoseconds >= nanoseconds){
            requestTimeReached = 1;
        }

    }

    printf("We out this. %d\n", getpid());

    // clean shared mem
    shmdt(sysClockshmPtr);
    shmdt(RDPtr);

    exit(0);
}
