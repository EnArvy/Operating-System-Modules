#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>

// Structure to represent a process
struct Process {
    char pid[32];                  // Process ID
    int arrival_time;         // Arrival time in microseconds
    int job_time;           // Burst time in microseconds
    int remaining_time;       // Remaining time in microseconds
    int turnaround_time;      // Turnaround time in microseconds
    int response_time;         // Waiting time in microseconds
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
void combineLogs(struct log*head){
    struct log *temp = head;
    while(temp->next != NULL){
        if(strcmp(temp->pid,temp->next->pid) == 0){
            temp->finish = temp->next->finish;
            free(temp->next);
            temp->next = temp->next->next;
            continue;
        }
        temp = temp->next;
    }
}
void printLog(struct log *head){
    while(head != NULL){
        printf("%s %d %d ",head->pid,head->start,head->finish);
        head = head->next;
    }
    printf("\n");
}


void printAvgTurnaroundResponse(struct Process processes[], int n){
    int sum = 0;
    for(int i=0;i<n;i++){
        sum += processes[i].turnaround_time;
    }
    printf("%.2f ",(float)sum/n);
    sum=0;
    for(int i=0;i<n;i++){
        sum += processes[i].response_time;
    }
    printf("%.2f\n",(float)sum/n);
}


struct log* fcfs(struct Process processes[], int n) {
    int current_time = 0;
    struct log* head=NULL;

    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time > current_time) {
            current_time = processes[i].arrival_time;
        }

        processes[i].turnaround_time = current_time + processes[i].job_time - processes[i].arrival_time;
        processes[i].response_time = current_time - processes[i].arrival_time;

        insertLog(&head,processes[i].pid,current_time,current_time + processes[i].job_time);

        current_time += processes[i].job_time;
    }
    return head;
}


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
            if(processes[current_process].response_time == -1){
                processes[current_process].response_time = current_time - processes[current_process].arrival_time;
            }
            current_time += execution_time;
            processes[current_process].remaining_time -= execution_time;

            insertLog(&head,processes[current_process].pid,current_time - execution_time,current_time);

            if (processes[current_process].remaining_time == 0) {
                remaining_processes--;
                processes[current_process].turnaround_time = current_time - processes[current_process].arrival_time;
            }
        }

        current_process = (current_process + 1) % n;
    }

    combineLogs(head);
    return head;
}


struct log* sjf(struct Process processes[], int n) {
    int current_time = 0;
    int completed = 0;
    bool executed[n]; // Array to track if a process has been executed
    struct log *head = NULL;

    // Initialize the executed array
    for (int i = 0; i < n; i++) {
        executed[i] = false;
    }

    while (completed < n) {
        int shortest_job_index = -1;
        int shortest_job_time = INT_MAX;

        // Find the shortest job that has arrived and not yet executed
        for (int i = 0; i < n; i++) {
            if (!executed[i] && processes[i].arrival_time <= current_time && processes[i].job_time < shortest_job_time) {
                shortest_job_time = processes[i].job_time;
                shortest_job_index = i;
            }
        }

        if (shortest_job_index != -1) {
            // Execute the shortest job
            int execution_time = processes[shortest_job_index].job_time;
            current_time += execution_time;

            // Update turnaround, and waiting times
            processes[shortest_job_index].turnaround_time = current_time - processes[shortest_job_index].arrival_time;
            processes[shortest_job_index].response_time = current_time - processes[shortest_job_index].arrival_time - execution_time;

            insertLog(&head,processes[shortest_job_index].pid,current_time - execution_time,current_time);

            // Mark the process as executed
            executed[shortest_job_index] = true;
            completed++;
        } else {
            // If no process is ready to execute, move time forward
            int soonest_job = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!executed[i] && processes[i].arrival_time < soonest_job) {
                    soonest_job = processes[i].arrival_time;
                }
            }
            current_time=soonest_job;
        }
    }

    return head;
}


struct log* shortestRemainingTimeFirst(struct Process processes[], int n) {
    int current_time = 0;
    int completed = 0;
    struct log* head = NULL;

    while (completed < n) {
        int shortest_job_index = -1;
        int shortest_job_time = INT_MAX;
        bool job_found = false;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].job_time < shortest_job_time) {
                    shortest_job_time = processes[i].job_time;
                    shortest_job_index = i;
                    job_found = true;
                }
            }
        }

        if (!job_found) {
            current_time++;
        } else {
            int execution_time = 1; // Execute one unit of time
            current_time += execution_time;
            processes[shortest_job_index].remaining_time -= execution_time;
            insertLog(&head,processes[shortest_job_index].pid,current_time - execution_time,current_time);
            if(processes[shortest_job_index].response_time == -1){
                processes[shortest_job_index].response_time = current_time - processes[shortest_job_index].arrival_time - execution_time;
            }

            if (processes[shortest_job_index].remaining_time == 0) {
                completed++;
                processes[shortest_job_index].turnaround_time = current_time - processes[shortest_job_index].arrival_time;
            }
        }
    }

    combineLogs(head);
    return head;
}



int main(int argc, char **argv){
	FILE *input = fopen(argv[1],"r");
	FILE *output = freopen(argv[2],"w",stdout);
	struct Process processes[32768],clonedProcesses[32768];
	int TsRR=atoi(argv[3]),TsMLFQ1=atoi(argv[4]),TsMLFQ2=atoi(argv[5]),TsMLFQ3=atoi(argv[6]),BMLFQ=atoi(argv[7]);
    struct log* head;

	char process[256];
	int numberProcesses=0;
	for(int i=0; fgets(process,sizeof(process),input);i++){
		strcpy(processes[i].pid,strtok(process," "));
		processes[i].arrival_time = atoi(strtok(NULL," "));
		processes[i].job_time = atoi(strtok(NULL," "));
        processes[i].remaining_time = processes[i].job_time;
        processes[i].response_time = -1;
		numberProcesses = i+1;
	}

    for(int i=0;i<numberProcesses;i++)clonedProcesses[i]=processes[i];
	head = fcfs(clonedProcesses,numberProcesses);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,numberProcesses);

	for(int i=0;i<numberProcesses;i++)clonedProcesses[i]=processes[i];
	head = roundRobin(clonedProcesses,numberProcesses,TsRR);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,numberProcesses);

    for(int i=0;i<numberProcesses;i++)clonedProcesses[i]=processes[i];
	head = sjf(clonedProcesses,numberProcesses);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,numberProcesses);

	for(int i=0;i<numberProcesses;i++)clonedProcesses[i]=processes[i];
	head = shortestRemainingTimeFirst(clonedProcesses,numberProcesses);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,numberProcesses);
	// MLFQ();

	return 0;
}