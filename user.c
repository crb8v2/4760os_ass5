//
// Created by crbaniak on 11/10/18.
//

#include "header.h"

int main(int argc, char* argv[]){

    // init clock to random
    time_t t;
    srand((unsigned) time(&t));

    // shared memory config
    sharedMemoryConfig();

    // random amount of time to request to OS
    // rand between 500m and 50m nanos
    int randTime = (rand() % 500000000) + 50000000;

    // generate users job
    // will request, release, or terminate.
    int randPercent = (rand() % 100) + 1;

    if(randPercent >= 95){          // 5 percent chance
        //terminate
    }else if (randPercent >= 60){   //35 percent chance
        //release
    }else{                          // 60 percent chance
        //request +1
    }

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