//
// Created by crbaniak on 11/10/18.
//

#ifndef ASS5_HEADER_H
#define ASS5_HEADER_H

#include <stdio.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


//#include <time.h>
//#include <sys/wait.h>
//#include <sys/time.h>
//#include <sys/ipc.h>
//#include <fcntl.h>
//#include <semaphore.h>
//#include <sys/stat.h>
//#include <string.h>

//shared mem keys, probs not secure in a header, but its here to stay
#define CLOCK_SHMKEY 123123
#define RD_SHMKEY 123124

//#################################
//##### SHARED MEMORY SEGMENT #####
//#################################

// shared memory
//      rescources
//      clock

// struct for time
typedef struct {
    unsigned int seconds;
    unsigned int nanoseconds;
} systemClock_t;

//struct for rescource descriptor
typedef struct {
    int allocated;
    int max;
    int request;

} rescourceDescriptor_t[20];

// ##### GLOBALS #####
// globals for accessing pointers to shared memory
int sysClockshmid; //holds the shared memory segment id
systemClock_t *sysClockshmPtr; //points to the data structure
int RDshmid;
rescourceDescriptor_t *RDPtr;


// allocates shared mem
void sharedMemoryConfig() {

    //shared mem for sysClock
    sysClockshmid = shmget(CLOCK_SHMKEY, sizeof(systemClock_t), IPC_CREAT|0777);
    if(sysClockshmid < 0)
    {
        perror("sysClock shmget error in master \n");
        exit(errno);
    }
    sysClockshmPtr = shmat(sysClockshmid, NULL, 0);
    if(sysClockshmPtr < 0){
        perror("sysClock shmat error in oss\n");
        exit(errno);
    }

    //shared mem for Rescource Descriptor
    RDshmid = shmget(RD_SHMKEY, sizeof(rescourceDescriptor_t), IPC_CREAT|0777);
    if(RDshmid < 0)
    {
        perror("RD shmget error in master \n");
        exit(errno);
    }
    RDPtr = shmat(RDshmid, NULL, 0);
    if(RDPtr < 0){
        perror("sysClock shmat error in oss\n");
        exit(errno);
    }

    //tests for shared mem
//    sysClockshmPtr->nanoseconds = 3;
//    printf("%d\n", sysClockshmPtr->nanoseconds);
//    sysClockshmPtr->nanoseconds = 0;
//    sysClockshmPtr->seconds = 0;

}



#endif //ASS5_HEADER_H
