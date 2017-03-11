/*
Avery VanKirk 01025959
October 2016
CSCI 460 hw1: Processes

ProgramB:

1) Forks child process that reads shared memory from ProgramA
2) Child sends number to parent process over pipe
3) Parent process displays number on stdout

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX_LINE    1024
#define FALSE       0
#define TRUE        1

//handles forking stuff
int forknread(){

    int fd[2];
    pipe(fd);
    pid_t pid;
    char buf[MAX_LINE];

    if ((pid = fork()) < 0)
        perror("fork failed");
    //we are child
    if (pid == 0){
        //setup pipe
        close(fd[0]);
        //do shared memory recieve stuff from processA
        int usersnum = sharedMemRead();
        //Write to pipe to send to parent process
        FILE* fpw = fdopen(fd[1], "w");
        fprintf(fpw, "%d\n", usersnum % 26);
        exit(0);
    }
    //we are parent
    else{
        //setup pipe, receive from child
        close(fd[1]);
        FILE* fpr = fdopen(fd[0], "r");
        fgets(buf, MAX_LINE, fpr);

        printf("Result: %d\n",atoi(buf));
    }
return 0;
}

int sharedMemRead(){
    const int SIZE = MAX_LINE;
    const char *name = "hw1";

    //shared mem fd
    int shm_fd;
    int *ptr;

    shm_fd = shm_open(name, O_RDONLY, 0666);

    ptr = mmap(0,sizeof(int), PROT_READ, MAP_SHARED,shm_fd,0);
    //read
    if((int)ptr == -1){
      printf("Nothing available to read!\n");
    }
    else{
      printf("Reading from shared memory:\n");
    }

    int usersnum = *ptr;
    shm_unlink(name);

    return usersnum;
}

int main (int argc, char** argv) {
    forknread();
    return 0;
}
