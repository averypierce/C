//vanHeap.h
//Avery VanKirk 
//October 2016

/*	Header file vanHeap.h
 *  heaps implemented as array
 *
 */

//A[0] is heap size. Heap starts at A[1]
//internal stuffs
//void buildmaxheap(int A[]);
//void vanInsert(int A[], int key);
//int heap_pop(int A[]);


//Interface for schedulers
void vanInitialize();
void vanAdd_process(int pid,int ttime,int CPUtimeUsed,int timeslice);
int vanProcesses_ready();
int vanProcess_pop();
int vanLastrun(int pid);
int vanGet_priority(int pid);
void vanSet_waiting(int pid,int timestamp);
int vanGet_iowait(int pid);
