//Avery VanKirk 
//October 2016

/* vanHeap.c
	 A bunch of stuff has 'van' in front of it because some conflicts the c files
	 that we were given.

	 Modification of basic heap implementation for 4BSD scheduler simulation
	 Seemed like a good idea at the time.

	 Processes are stored as member of array of structs in oversight[]
	 process Queue is an array struct pointers pointing at memebers of oversight[]

	 Heap Notes:
   heap is implemented as arrayy
   heap indexing starts at array[1]
   array[0] holds size of array
   //See algorithm from CLRS chapter 6
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define MAX    200



struct vanNode {
	int pid;
	int priority;
	int lastrun;
	int iowait; 
}*bsdq[MAX],oversight[MAX];
//Hey, turns out we need to be able to find things from PID, like, whenever.
//In light of that, we are proud to anounce project oversight!
//It keeps track of all our nodes in an array so we can actually find them

void max_heapify(struct vanNode *A[], int i){
	int left = 2 * i;
	int right = left + 1;
	int largest = i;
	int size = A[0]->priority;
	struct vanNode *temp;

	if(left <= size && A[left]->priority > A[i]->priority)
		largest = left;
  if(right <= size && A[right]->priority > A[largest]->priority)
		largest = right;

	if(largest != i) {
		temp = A[i];
		A[i] = A[largest];
		A[largest] = temp;
		max_heapify(A,largest);
	}
}
//
void buildmaxheap(struct vanNode *A[]){
	int i = A[0]->priority;
	for(i = floor(A[0]->priority/2); i>=1; i--) {
		max_heapify(A,i);
	}
}

//To help readablity of heap functions and match CLRS
int parent(int i){
	return floor(i/2);
}

//i is heap size
int heap_increase_key(struct vanNode *A[],int i,struct vanNode * vproc,int key){
	struct vanNode *temp;
	if (key < A[i]->priority) {
		printf("heap increase error, %d, key %d\n", A[i]->priority , key);
		return -1;
	}
	A[i]->priority = key;
	//Swapping
	while(i > 1 && A[parent(i)]->priority < A[i]->priority) {
		temp = A[i];
		A[i] = A[parent(i)];
		A[parent(i)] = temp;
		i = parent(i);
	}
	return 0;
}

//Note: A[0]->priority is storing heapsize. Heap index starts at A[1]
void vanInsert(struct vanNode ** A, struct vanNode * vproc){
	int size = A[0]->priority += 1;
	int key = vproc->priority;
	//link pointer array to storage array
	A[size] = &oversight[vproc->pid-1000];
	//set last elements prioirty/key to -1 for heap_increase_key()
	A[size]->priority = -1;
	heap_increase_key(A,size,A[size],key);
}

//Currently returns a copy of node...Was there a reason for that?
int heap_pop(struct vanNode ** A){
	int size = A[0]->priority;

	if(A[0]->priority < 0) {
		printf("error heap underflow\n");
		exit(-1);
	}
  int nextpid = A[1]->pid;
	struct vanNode *max = (struct vanNode *)malloc(sizeof(struct vanNode *));
	max = A[1];
	A[1] = A[size];
	A[0]->priority -= 1;
	max_heapify(A,1);
	return nextpid;
}

//prqtime is process-ready-queue time
void vanAdd_process(int pid, int timestamp,int CPUtimeUsed,int timeslice){

  int index = pid-1000;
  int priority = oversight[index].priority; //will be 0 for a new process
  //Probably dont need both checks
  if(CPUtimeUsed == 0 && priority == 0){
    //printf("Starting new priority 8 process\n" );
    priority = 8;
  }
  else if(oversight[index].iowait > 0 && priority < 16){ //Waited on IO
    priority++;
    //oversight[index].iowait = 0; //This was Zeroing iowait too early, should wait until dispatch
  }
  else if(CPUtimeUsed == timeslice && priority > 0){ //Used its whole time slice before
    priority--;
  }

	oversight[index].pid = pid;
	oversight[index].priority = priority;
	oversight[index].lastrun = timestamp;
	struct vanNode *temp = &oversight[index]; //pointer to actual struct kept in oversight

	vanInsert(bsdq,temp);
}

//Returns number of processes that are in queue.
int vanProcesses_ready(){
  return bsdq[0]->priority;
}
//Used to printf process priority when it is dispatched
int vanGet_priority(int pid){
  return oversight[pid-1000].priority;
}
int vanProcess_pop(){
  int pid = heap_pop(bsdq);
  return pid;
}
void vanSet_waiting(int pid,int timestamp){
  oversight[pid-1000].iowait = timestamp;
}
int vanGet_iowait(int pid){
  return oversight[pid-1000].iowait;
}
int vanLastrun(int pid){
  return oversight[pid-1000].lastrun;
}

//Setup first node to store the heap size
//Only because thats how the original algorithm was
void vanInitialize(){
  bsdq[0] = malloc(sizeof(struct vanNode));
	bsdq[0]->priority = 0;
}
