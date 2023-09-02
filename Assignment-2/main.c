#include<stdio.h>
#include<string.h>
#include<stdlib.h>

// Structure to represent a process
struct Process {
    char pid[32];                  // Process ID
    int arrival_time;         // Arrival time in microseconds
    int job_time;           // Burst time in microseconds
    int remaining_time;       // Remaining time in microseconds
    int turnaround_time;      // Turnaround time in microseconds
    int waiting_time;         // Waiting time in microseconds
};

int main(int argc, char **argv){
	FILE *input = fopen(argv[1],"r");
	FILE *output = freopen(argv[2],"w",stdout);
	struct Process processes[32768];
	int TsRR=atoi(argv[2]),TsMLFQ1=atoi(argv[3]),TsMLFQ2=atoi(argv[4]),TsMLFQ3=atoi(argv[5]),BMLFQ=atoi(argv[6]);

	char process[256];
	int numberProcesses=0;
	for(int i=0; fgets(process,sizeof(process),input);i++){
		strcpy(processes[i].pid,strtok(process," "));
		processes[i].arrival_time = atoi(strtok(NULL," "));
		processes[i].job_time = atoi(strtok(NULL," "));
		numberProcesses = i+1;
		// printf("%s %d\n",processes[i].pid,processes[i].arrival_time,processes[i].job_time);
	}

	// FCFS();
	roundRobin(processes,numberProcesses,TsRR);
	// ShortestJobFirst();
	// ShortestTimeRemaining();
	// MLFQ();

	return 0;
}