//Avery VanKirk 
//October 2016

/* vanQueue.c
   ULE queue.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX         200
#define FALSE       0
#define TRUE        1

struct cq{
    int pid;
    int lastrun;
    int iowait;
    struct cq *next;
  }*cq_head,*cq_tail,*nq_head,*nq_tail,*vanHead,*vanTail,oversight[MAX];

//Sets vanHead and tail to current or next queue
int set(char which){
  if(which = 'c'){
    vanHead = cq_head;
    vanTail = cq_tail;
  }
  else if(which = 'n'){
    vanHead = nq_head;
    vanTail = nq_tail;
  }
  else{
    return -1;
  }
  return 0;
}
//set stuff back for us when done
int finish(char which){
  if(which = 'c'){
    cq_head = vanHead;
    cq_tail = vanTail;
  }
  else if(which = 'n'){
    nq_head = vanHead;
    nq_tail = vanTail;
  }
  return 0;
}

//adds to end of list
//vanhead,vantail
int add(struct cq * temp,char which){

    if(set(which) != 0){
      return -1;
    }

    if (vanHead == NULL){
        temp->next = vanHead;
        vanHead = temp; //makes vanHead POINT to actual instance temp.
        vanTail = temp; //Only one item right now, so make sure...this.
    }
    else{
        temp->next = NULL;
        vanTail->next = temp; //current tail node now pointing to new node
        vanTail = temp; //reassign tail ptr to new tail node
    }
    finish(which);
    return 0;
}

void vanAdd_process(int pid, int prqtime,int CPUtimeUsed,int timeslice){

  char queue;
  int index = pid-1000;
  struct cq *temp = &oversight[index];
	oversight[index].pid = pid;
	oversight[index].lastrun = prqtime;

  //new processes go on current
  if(CPUtimeUsed == 0){
    queue = 'c';
  }
  //processes waiting on io also go to current queue
  if(oversight[index].iowait > 0){
    //oversight[index].iowait = 0;
    queue = 'c';
  }
  //processes using their whole slice go on next queue
  else if(CPUtimeUsed == timeslice){
    queue = 'n';
  }
  //This should crash If my if statements are missing a case and not assigning
  add(temp,queue);
}

//Returns number of processes that are in queue.
int vanProcesses_ready(char which){
  set(which);
  if(vanHead == NULL)
    return FALSE;
  else
    return TRUE;
  finish(which);
}
//Used to printf process priority when it is dispatched

void vanSet_waiting(int pid,int timestamp){
  oversight[pid-1000].iowait = timestamp;
}
int vanGet_iowait(int pid){
  return oversight[pid-1000].iowait;
}
int vanLastrun(int pid){
  return oversight[pid-1000].lastrun;
}

//remove front of list and return its data. returns -1 if list empty
int pop(char which){

    if(set(which) != 0){
      return -2;
    }
    if (vanHead != NULL){
        int pid = vanHead->pid;
        vanHead = vanHead->next;
        //printf("popped %d\n",pid);
        finish(which);
        return pid;
    }
}

int vanProcess_pop(char which){
  return pop(which);
}

void printt(){
  vanHead = cq_head;
  while(vanHead->next != NULL){
    printf("node %d\n",vanHead->pid );
    vanHead = vanHead->next;
  }
  printf("node %d\n",vanHead->pid );
}
