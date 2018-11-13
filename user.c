//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

int main(int argc, char* argv[]){

    // shared memory config
    sharedMemoryConfig();

    printf("We in this. %d", getpid());

    printf("my name is %s", argv[0]);
    printf("my name is %s", argv[1]);

    // clean shared mem
    shmdt(sysClockshmPtr);
    shmdt(RDPtr);

//    raise(SIGQUIT);
//
    exit(0);
}

