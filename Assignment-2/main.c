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

//Linked list of log struct
struct log{
    char pid[32];
    int start;
    int finish;
    struct log *next;
};
//Function to create a new node
struct log* createLog(char *pid, int start, int finish){
    struct log *newNode = (struct log*)malloc(sizeof(struct log));
    strcpy(newNode->pid,pid);
    newNode->start = start;
    newNode->finish = finish;
    newNode->next = NULL;
    return newNode;
}
//Function to insert a node at the end of the linked list
void insertLog(struct log **head,char *pid, int start, int finish){
    struct log *newNode = createLog(pid,start,finish);
    if(*head == NULL){
        *head = newNode;
        return;
    }
    struct log *temp = *head;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = newNode;
}
void printLog(struct log *head){
    while(head != NULL){
        printf("%s %d %d ",head->pid,head->start,head->finish);
        head = head->next;
    }
    printf("\n");
}


// Function to simulate the Round Robin scheduling algorithm
struct log* roundRobin(struct Process processes[], int n, int time_slice) {
    int remaining_processes = n;
    int current_time = 0;
    int current_process = 0;

    struct log *head = NULL;

    while (remaining_processes > 0) {
        if (processes[current_process].remaining_time > 0) {
            int execution_time = (processes[current_process].remaining_time < time_slice)
                                    ? processes[current_process].remaining_time
                                    : time_slice;

            current_time += execution_time;
            processes[current_process].remaining_time -= execution_time;

            insertLog(&head,processes[current_process].pid,current_time - execution_time,current_time);

            if (processes[current_process].remaining_time == 0) {
                remaining_processes--;
                processes[current_process].turnaround_time = current_time - processes[current_process].arrival_time;
                processes[current_process].waiting_time = processes[current_process].turnaround_time - processes[current_process].job_time;
            }
        }

        current_process = (current_process + 1) % n;
    }

    //Combine continuouse execution of same process
    struct log *temp = head;
    while(temp->next != NULL){
        if(strcmp(temp->pid,temp->next->pid) == 0){
            temp->finish = temp->next->finish;
            temp->next = temp->next->next;
            continue;
        }
        temp = temp->next;
    }
    return head;
}


int main(int argc, char **argv){
	FILE *input = fopen(argv[1],"r");
	FILE *output = freopen(argv[2],"w",stdout);
	struct Process processes[32768],clonedProcesses[32768];
	int TsRR=atoi(argv[3]),TsMLFQ1=atoi(argv[4]),TsMLFQ2=atoi(argv[5]),TsMLFQ3=atoi(argv[6]),BMLFQ=atoi(argv[7]);

	char process[256];
	int numberProcesses=0;
	for(int i=0; fgets(process,sizeof(process),input);i++){
		strcpy(processes[i].pid,strtok(process," "));
		processes[i].arrival_time = atoi(strtok(NULL," "));
		processes[i].job_time = atoi(strtok(NULL," "));
        processes[i].remaining_time = processes[i].job_time;
		numberProcesses = i+1;
	}

    struct log* head;
	// // FCFS();
	for(int i=0;i<numberProcesses;i++)clonedProcesses[i]=processes[i];
	head = roundRobin(clonedProcesses,numberProcesses,TsRR);
    printLog(head);
	// ShortestJobFirst();
	// ShortestTimeRemaining();
	// MLFQ();

	return 0;
}