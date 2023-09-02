#include<stdio.h>
#include<string.h>
#include<stdlib.h>

// Structure to represent a process
struct Process {
    char pid[32];                  // Process ID
    int arrival_time;         // Arrival time in microseconds
    int burst_time;           // Burst time in microseconds
    int remaining_time;       // Remaining time in microseconds
    int turnaround_time;      // Turnaround time in microseconds
    int waiting_time;         // Waiting time in microseconds
	int job_time;
};

int main(int argc, char **argv){
	FILE *input = fopen(argv[1],"r");
	FILE *output = fopen(argv[2],"w");
	struct Process processes[32768];

	char process[256];
	int numberProcesses=0;
	for(int i=0; fgets(process,sizeof(process),input);i++){
		strcpy(processes[i].pid,strtok(process," "));
		processes[i].arrival_time = atoi(strtok(NULL," "));
		processes[i].job_time = atoi(strtok(NULL," "));
		numberProcesses = i+1;
	}


	return 0;
}