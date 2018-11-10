//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

void sigint(int);

int main(int argc, char* argv[]) {

    //signal handling config - ctrl-c
    signal(SIGINT, sigint);

    return 0;
}


void sigint(int a) {
    printf("^C caught\n");

    // kill open forks

    // write to log

    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);


    exit(0);
}