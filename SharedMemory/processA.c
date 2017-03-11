/*
Avery VanKirk 01025959
October 2016
CSCI 460 hw1: Processes

ProgramA:

1) Reads a number from stdin
2) Sends number to child process over pipe
3) Child process writes number to shared memory for ProgramB to retrieve

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>


#define MAX_LINE    1024
#define FALSE       0
#define TRUE        1

//Forks process and calls a function to send data
int forknsend(int usersnum){

    pid_t pid;

    if ((pid = fork()) < 0)
        perror("fork failed");
    //we are child
    if (pid == 0){
        //Send to processB via shared memory
        sharedMemWrite(usersnum);
        exit(0);
    }
    //we are parent
    else{
        //nothin
    }
return 0;
}

//Writes usersnum to shared mem under name 'hw1'
int sharedMemWrite(int usersnum){

    const int SIZE = MAX_LINE;
    const char *name = "hw1";
    //shared mem fd
    int shm_fd;
    int *ptr;

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd,MAX_LINE);
    ptr = mmap(0,sizeof(int), PROT_WRITE, MAP_SHARED,shm_fd,0);

    //Write
    *ptr = usersnum;

    return 0;
}


void userPrompt(){
  char buf[MAX_LINE];
  int i;
  printf("$-.-> ");

  while (fgets(buf, MAX_LINE, stdin)) {

      int invalid = FALSE;

      for(i = 0; buf[i] !='\n';i++){
          if(buf[i] < 48 || buf[i] > 57){
              invalid = TRUE;
              break;
          }
      }

      if(invalid){
          printf("Only digits 0-9. No other characters allowed. Max length 9.\n");
          printf("$-.-> ");
      }

      else{
          //Limiting digits to what atoi understands
          buf[9] = '\n';
          int i = atoi(buf);
          printf("Your number is: %d, sending to processB.\n",i);
          forknsend(i);
          break;
      }
  }
}

//Launches user prompt function.
//Will accept input as command line arg for advanced users
int main (int argc, char** argv) {

    if(argc > 1) {
      forknsend(atoi(argv[1]));
    }

    else{
      userPrompt();
    }
    //generates newline for shell when we ctrl-d
    printf("\n");
    return 0;
}
