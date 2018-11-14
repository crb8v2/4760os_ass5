//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

int main(int argc, char* argv[]){

    // shared memory config
    sharedMemoryConfig();

    // get argv values for randClockTime sent in

    // case statement of operation to perform

    printf("We in this. %d\n", getpid());

    while(1);

    // clean shared mem
    shmdt(sysClockshmPtr);
    shmdt(RDPtr);

//    raise(SIGQUIT);
//
    exit(0);
}








//tests

//printf("We in this. %d\n", getpid());
//printf("my name is %s\n", argv[0]);
//printf("my name is %s\n", argv[1]);