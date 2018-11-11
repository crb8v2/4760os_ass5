//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

int main(int argc, char* argv[]){

    // shared memory config
    sharedMemoryConfig();

    // clean shared mem
    shmdt(sysClockshmPtr);
    shmdt(RDPtr);
}

