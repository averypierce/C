//Avery VanKirk 
//October 2016

/*	Header file vanQueue.h
 * Queue implemented as linked list
 *
 */
//internal functions
 //adds to end of list
 void add(int pid, char which);
 //remove front of list and return its data. returns -1 if list empty
 int pop(char which);

//Interface for schedulers
void vanAdd_process(int pid,int ttime,int CPUtimeUsed,int timeslice);
int vanProcesses_ready();
int vanProcess_pop();
int vanLastrun(int pid);
void vanSet_waiting(int pid,int total_CPUtimeUsed);
int vanGet_iowait(int pid);
