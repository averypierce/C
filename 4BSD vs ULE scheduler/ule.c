/*

	Avery VanKirk  2016
	Operating Systems Assignment
	
 *  ULE scheduler
 *  Ready queue wait times and IO response times for each process are measured
 *	relative to to run time of other the other processes by keeping a count of
 *	total CPUtimeUsed values received from SchedSim.
 *]
		Since our scheduler comparisons are supposed to be relative, im just basing
		them all off of CPUtimeUsed, and an artifical 10msec context swtich to calculate
		overhead.

		1: CPUtimeUsed tells us how long each process waited in queue, assuming overhead isnt readablity
		2: In order to account for efficiency in different sized timeslices, there is a context switch counter.
		this way, a context switch results in penalties for small timeslices


 */

#include <stdio.h>
#include <stdlib.h>
#include "Dispatcher.h"
#include "vanQueue.h"
#include "SchedSim.h"

#define TRUE 		1
#define FALSE 	0

int verbose = FALSE;
int rounds = 100;
int timeslice = 100; //default

int processes_started;
int processes_completed;

int total_CPUtimeUsed;
int total_ready_queue_time = 0;
int total_io_response_time = 0;

int total_dispatches = 0; //Total dispatches == rounds
int total_iowaits = 0;

int min_time_in_ready_queue = 999; //prevent being stuck to 0
int max_time_in_ready_queue  = 0;
int avg_time_in_ready_queue = 0;

int min_time_io_response = 99999;
int max_time_io_response  = 0;
int avg_time_io_response = 0;

int context_switch = 5; //msec
int num_context_switches;
int overhead = 0;


char current = 'c';
char next = 'n';

void swap_queues(){
	if(current == 'c'){
		current = 'n';
		next = 'c';
	}
	else{
		current = 'c';
		next = 'n';
	}
}

void NewProcess(int pid) {
// Informs the student's code that a new process has been created, with process id = pid
// The new process should be added to the ready queue
	if(verbose)
		printf("new process %d created\n", pid);
	processes_started += 1;
	Ready(pid, 0);				// note: you need to invoke Ready()
}

void Dispatch(int *pid) {
// Requests the pid of the process to be changed to the running state
// The process should be removed from the ready queue
  	if(*pid = vanProcesses_ready()){
			*pid = vanProcess_pop();
			int prqtime = total_CPUtimeUsed - vanLastrun(*pid);
			total_ready_queue_time += prqtime;

			total_dispatches += 1;
			if(prqtime > 0){
				//This prevents initial processes from sticking queue time to zero, because that isnt interesting
				max_time_in_ready_queue = max_time_in_ready_queue > prqtime ? max_time_in_ready_queue : prqtime;
				min_time_in_ready_queue = min_time_in_ready_queue < prqtime ? min_time_in_ready_queue : prqtime;
			}

			int iowait;
			int waitingsince = vanGet_iowait(*pid);
			if(waitingsince){
				iowait = total_CPUtimeUsed - waitingsince;
				total_io_response_time += iowait;
				max_time_io_response = max_time_io_response > iowait ? max_time_io_response : iowait;
				min_time_io_response = min_time_io_response < iowait ? min_time_io_response : iowait;
					vanSet_waiting(*pid,0);
			}
			if(verbose)
				printf("process %d dispatched after %d ms\n", *pid,prqtime);
		}
		else{ //if next queue is empty, time to swap them
			if(verbose)
				printf("Swapping Queues!\n");
			swap_queues();
		}
	}


void Ready(int pid, int CPUtimeUsed) {

    total_CPUtimeUsed += CPUtimeUsed; //current time, ie 'lastrun' so that next

		//If process used entire timeslice, then we had to do a context switch!
		//Now, How do we represent that....
		if(CPUtimeUsed == timeslice){
			//total_CPUtimeUsed += context_switch; //because if ctswitch, itll be that many before next process starts
			num_context_switches += 1;
			overhead += context_switch;
		}//separate out processtime from other time!

    vanAdd_process(pid,total_CPUtimeUsed,CPUtimeUsed,timeslice);
		if(verbose)
			printf("process %d added to queues after using %d msec\n", pid, CPUtimeUsed);
	}


void Waiting(int pid) {
	// Informs the student's code that the process with id = pid has changed from the running state
	// to the waiting state
	//TimeStamps process with relative time
 	vanSet_waiting(pid,total_CPUtimeUsed);
	total_iowaits += 1;
	if(verbose)
		printf("process %d waiting\n", pid);
}


void Terminate(int pid) {
	//Informs the student's code that the process with id = pid has terminated
	//Total up stuff here? maybe? seems like maybe that
	processes_completed += 1;
	if(verbose)
		printf("process %d terminated\n", pid);
}

int main(int argc, char** argv) {

	if(argc > 1){
		timeslice = atoi(argv[1]);

		if(argc == 3)
			if(*argv[2] == 'v')
				verbose = TRUE;
	}

	Simulate(rounds, timeslice);

  printf("---------------------------------\n");
	printf("ULE Scheduler, timeslice = %d\n", timeslice);
  printf("---------------------------------\n");
	printf("1) Time spent in ready queue\n");
 	printf("Min: %d  msec\n",min_time_in_ready_queue);
 	printf("Max: %d  msec\n",max_time_in_ready_queue);
 	printf("Avg: %d msec\n",total_ready_queue_time/total_dispatches);
	printf("Total: %d msec\n",total_ready_queue_time);
	printf("---\n");
	//Time for process to RESPOND to IO, not time it spends waiting on IO
	printf("2) Response time to I/O operations\n");
 	printf("Min: %d msec\n",min_time_io_response);
 	printf("Max: %d msec\n",max_time_io_response);
 	printf("Avg: %d msec\n",total_io_response_time/(total_iowaits+1)); //no divide by zero
	printf("---\n");
  printf("3) Proportion of time spent on scheduler overheads:\n");
  printf("Total process execution time: %d msec\n",total_CPUtimeUsed);
	printf("Total scheduler overhead:     %d msecs\n",overhead );
	printf("Scheduler overhead:           %.2f%% of cpu\n",(double)overhead/(total_CPUtimeUsed + overhead )*100);//no divide by zero
	printf("---\n");
	printf("Procceses Started: %d\n",processes_started );
	printf("Procceses completed: %d\n",processes_completed );

}
