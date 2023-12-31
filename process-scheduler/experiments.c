#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <limits.h> 
#include<string.h>

// Function to generate an exponential random variable in microseconds
double generateExponentialRandom(double lambda) {
    return -log(1.0 - (double)rand() / (RAND_MAX + 1.0))/ lambda;
}

struct Process {
    char pid[32];                  // Process ID
    double arrival_time;         // Arrival time in microseconds
    double job_time;           // Burst time in microseconds
    double remaining_time;       // Remaining time in microseconds
    double turnaround_time;      // Turnaround time in microseconds
    double response_time;         // Waiting time in microseconds
};

//Linked list of log struct
struct log{
    char pid[32];
    double start;
    double finish;
    struct log *next;
};
struct log* createLog(char *pid, double start, double finish){
    struct log *newNode = (struct log*)malloc(sizeof(struct log));
    strcpy(newNode->pid,pid);
    newNode->start = start;
    newNode->finish = finish;
    newNode->next = NULL;
    return newNode;
}
void insertLog(struct log **head,char *pid, double start, double finish){
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
        printf("%s %.3lf %.3lf ",head->pid,head->start,head->finish);
        head = head->next;
    }
    printf("\n");
}

void printAvgTurnaroundResponse(struct Process processes[], int n){
    double sum = 0;
    for(int i=0;i<n;i++){
        sum += processes[i].turnaround_time;
    }
    printf("%.3f ",(float)sum/n);
    sum=0;
    // printf("\n");
    for(int i=0;i<n;i++){
        sum += processes[i].response_time;
        // printf("%.3f ",processes[i].response_time);
    }
    // printf("\n");
    printf("%.3f\n",(float)sum/n);
}

struct Queue {
    int front, rear, size;
    unsigned capacity;
    struct Process* array;
};
struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = capacity - 1;
    queue->array = (struct Process*)malloc(queue->capacity * sizeof(struct Process));
    return queue;
}
bool isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}
void enqueue(struct Queue* queue, struct Process process) {
    if (queue->size == queue->capacity)
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = process;
    queue->size += 1;
}
struct Process dequeue(struct Queue* queue) {
    struct Process process = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size -= 1;
    return process;
}


struct log* fcfs(struct Process processes[], int n) {
    double current_time = 0;
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


struct log* roundRobin(struct Process processes[], int n, double time_slice) {
    struct Queue* queue = createQueue(n);
    struct log* head = NULL;
    int queued[n];
    for(int i=0;i<n;i++) queued[i]=0;

    double current_time = 0;
    int completed = 0;

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && queued[i]==0) {
                if (processes[i].remaining_time > 0) {
                    enqueue(queue, processes[i]);
                    queued[i]=1;
                }
            }
        }

        if (!isEmpty(queue)) {
            struct Process process = dequeue(queue);
            double execution_time = (process.remaining_time < time_slice) ? process.remaining_time : time_slice;
            current_time += execution_time;
            process.remaining_time -= execution_time;

            insertLog(&head,process.pid,current_time - execution_time,current_time);

            if(process.response_time == -1){
                process.response_time = 1;
                for(int i=0;i<n;i++){
                    if(strcmp(process.pid,processes[i].pid) == 0){
                        processes[i].response_time = current_time - processes[i].arrival_time - execution_time;
                        break;
                    }
                }
                process.response_time = current_time - process.arrival_time - execution_time;
            }
            for (int i = 0; i < n; i++) {
                if (processes[i].arrival_time <= current_time && queued[i]==0) {
                    if (processes[i].remaining_time > 0) {
                        enqueue(queue, processes[i]);
                        queued[i]=1;
                    }
                }
            }

            if (process.remaining_time == 0) {
                completed++;
                process.turnaround_time = current_time - process.arrival_time;
                for(int i=0;i<n;i++){
                    if(strcmp(process.pid,processes[i].pid) == 0){
                        processes[i].turnaround_time = current_time - processes[i].arrival_time;
                        break;
                    }
                }
            } else {
                enqueue(queue, process);
            }
        } else {
            double temptime = __DBL_MAX__;
            for(int i=0;i<n;i++){
                if(processes[i].arrival_time > current_time && processes[i].arrival_time < temptime){
                    temptime = processes[i].arrival_time;
                }
            }
            current_time = temptime; 
        }
    }

    free(queue->array);
    free(queue);

    combineLogs(head);
    return head;
}


struct log* sjf(struct Process processes[], int n) {
    double current_time = 0;
    int completed = 0;
    bool executed[n]; // Array to track if a process has been executed
    struct log *head = NULL;

    // Initialize the executed array
    for (int i = 0; i < n; i++) {
        executed[i] = false;
    }

    while (completed < n) {
        int shortest_job_index = -1;
        double shortest_job_time = __DBL_MAX__;

        // Find the shortest job that has arrived and not yet executed
        for (int i = 0; i < n; i++) {
            if (!executed[i] && processes[i].arrival_time <= current_time && processes[i].job_time < shortest_job_time) {
                shortest_job_time = processes[i].job_time;
                shortest_job_index = i;
            }
        }

        if (shortest_job_index != -1) {
            // Execute the shortest job
            double execution_time = processes[shortest_job_index].job_time;
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
            double soonest_job = __DBL_MAX__;
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
    double current_time = 0;
    int completed = 0;
    struct log* head = NULL;

    while (completed < n) {
        int shortest_job_index = -1;
        double shortest_job_time = __DBL_MAX__;
        bool job_found = false;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time < shortest_job_time) {
                    shortest_job_time = processes[i].remaining_time;
                    shortest_job_index = i;
                    job_found = true;
                }
            }
        }

        if (!job_found) {
            double temptime = __DBL_MAX__;
            for(int i=0;i<n;i++){
                if(processes[i].arrival_time > current_time && processes[i].arrival_time < temptime){
                    temptime = processes[i].arrival_time;
                }
            }
            current_time = temptime;
        } else {
            double execution_time = __DBL_MAX__; // Execute one unit of time
            for(int i=0;i<n;i++){
                if(processes[i].arrival_time > current_time && processes[i].remaining_time > 0 && i != shortest_job_index){
                    if(processes[i].arrival_time - current_time < execution_time){
                        execution_time = processes[i].arrival_time - current_time;
                    }
                }
            }
            execution_time = (execution_time<processes[shortest_job_index].remaining_time)?execution_time:processes[shortest_job_index].remaining_time;
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


struct log* mlfq(struct Process processes[], int n, double time_slice_high, double time_slice_med, double time_slice_low, double boost) {
    struct Queue* queue_high = createQueue(n);
    struct Queue* queue_med = createQueue(n);
    struct Queue* queue_low = createQueue(n);
    struct log* head = NULL;
    int queued[n];
    for(int i=0;i<n;i++) queued[i]=0;

    double current_time = 0;
    int completed = 0;
    double boosttrack = 0;

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && queued[i]==0) {
                if (processes[i].remaining_time > 0) {
                    enqueue(queue_high, processes[i]);
                    queued[i]=1;
                }
            }
        }

        // Dequeue from the highest-priority queue first
        if (!isEmpty(queue_high)) {
            struct Process process = dequeue(queue_high);
            double execution_time = (process.remaining_time < time_slice_high) ? process.remaining_time : time_slice_high;
            if(boosttrack + execution_time >= boost){
                execution_time = boost - boosttrack; 
                boosttrack = 0;
            }else{
                boosttrack += execution_time;
            }
            current_time += execution_time;
            process.remaining_time -= execution_time;

            if(execution_time!=0)
                insertLog(&head,process.pid,current_time - execution_time,current_time);
            // printf("%s1 %.3lf %.3lf\n",process.pid,current_time - execution_time,current_time);

            if(process.response_time == -1){
                process.response_time = 1;
                for(int i=0;i<n;i++){
                    if(strcmp(process.pid,processes[i].pid) == 0){
                        processes[i].response_time = current_time - processes[i].arrival_time - execution_time;
                        break;
                    }
                }
                process.response_time = current_time - process.arrival_time - execution_time;
            }

            if (process.remaining_time == 0) {
                completed++;
                process.turnaround_time = current_time - process.arrival_time;
                for(int i=0;i<n;i++){
                    if(strcmp(process.pid,processes[i].pid) == 0){
                        processes[i].turnaround_time = current_time - processes[i].arrival_time;
                        break;
                    }
                }
            } else {
                enqueue(queue_med, process);
            }
        } else if (!isEmpty(queue_med)) {
            struct Process process = dequeue(queue_med);
            double execution_time = (process.remaining_time < time_slice_med) ? process.remaining_time : time_slice_med;
            if(boosttrack + execution_time >= boost){
                execution_time = boost - boosttrack; 
                boosttrack = 0;
            }else{
                boosttrack += execution_time;
            }
            current_time += execution_time;
            process.remaining_time -= execution_time;

            if(execution_time!=0)
                insertLog(&head,process.pid,current_time - execution_time,current_time);
            // printf("%s2 %.3lf %.3lf\n",process.pid,current_time - execution_time,current_time);

            if (process.remaining_time == 0) {
                completed++;
                process.turnaround_time = current_time - process.arrival_time;
                for(int i=0;i<n;i++){
                    if(strcmp(process.pid,processes[i].pid) == 0){
                        processes[i].turnaround_time = current_time - processes[i].arrival_time;
                        break;
                    }
                }
            } else {
                enqueue(queue_low, process);
            }
        } else if (!isEmpty(queue_low)) {
            struct Process process = dequeue(queue_low);
            double execution_time = (process.remaining_time < time_slice_low) ? process.remaining_time : time_slice_low;
            if(boosttrack + execution_time >= boost){
                execution_time = boost - boosttrack; 
                boosttrack = 0;
            }else{
                boosttrack += execution_time;
            }
            current_time += execution_time;
            process.remaining_time -= execution_time;

            if(execution_time!=0)
                insertLog(&head,process.pid,current_time - execution_time,current_time);
            // printf("%s3 %.3lf %.3lf\n",process.pid,current_time - execution_time,current_time);

            if (process.remaining_time == 0) {
                completed++;
                process.turnaround_time = current_time - process.arrival_time;
                for(int i=0;i<n;i++){
                    if(strcmp(process.pid,processes[i].pid) == 0){
                        processes[i].turnaround_time = current_time - processes[i].arrival_time;
                        break;
                    }
                }
            } else {
                enqueue(queue_low, process);
            }
        } else {
            double temptime = __DBL_MAX__;
            for(int i=0;i<n;i++){
                if(processes[i].arrival_time > current_time && processes[i].arrival_time < temptime){
                    temptime = processes[i].arrival_time;
                }
            }
            boosttrack += temptime - current_time;
            current_time = temptime; 
            while(boosttrack >= boost) boosttrack -= boost;
        }
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && queued[i]==0) {
                if (processes[i].remaining_time > 0) {
                    enqueue(queue_high, processes[i]);
                    queued[i]=1;
                }
            }
        }
        if(boosttrack == 0){
            while(!isEmpty(queue_med)){
                struct Process process = dequeue(queue_med);
                enqueue(queue_high,process);
            }
            while(!isEmpty(queue_low)){
                struct Process process = dequeue(queue_low);
                enqueue(queue_high,process);
            }
        }
    }

    free(queue_high->array);
    free(queue_med->array);
    free(queue_low->array);
    free(queue_high);
    free(queue_med);
    free(queue_low);

    combineLogs(head);
    return head;
}


int main() {
    int n;  // Number of processes
    double time_slice; // Time slice for Round Robin
    double time_slice_high, time_slice_med, time_slice_low, boost; // time slices for MLFQ
    double meanInterArrivalTime;
    double meanJobDuration;
    struct log* head = NULL;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    struct Process processes[n], clonedProcesses[n];

    // Set the seed for the random number generator
    srand(time(NULL));

    printf("Enter the mean inter-arrival time : ");
    scanf("%lf", &meanInterArrivalTime);

    printf("Enter the mean job duration : ");
    scanf("%lf", &meanJobDuration);

    processes[0].arrival_time = ceil(generateExponentialRandom(1.0/meanInterArrivalTime));
    processes[0].job_time = ceil(generateExponentialRandom(1.0/meanJobDuration));
    processes[0].remaining_time = processes[0].job_time;
    processes[0].response_time = -1;
    sprintf(processes[0].pid, "p%d", 1);
    for (int i = 1; i < n; i++) {
        processes[i].arrival_time = ceil(generateExponentialRandom(1.0/meanInterArrivalTime))+processes[i-1].arrival_time;
        processes[i].job_time = ceil(generateExponentialRandom(1.0/meanJobDuration));
        processes[i].remaining_time = processes[i].job_time;
        processes[i].response_time = -1;
        sprintf(processes[i].pid, "p%d", i+1);
    }

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("----- Round Robin Scheduling -----\n");
    printf("Enter time slice for Round Robin : ");
    scanf("%lf", &time_slice);
    head = roundRobin(clonedProcesses, n, time_slice);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,n);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("\n----- First Come First Serve Scheduling -----\n");
    head = fcfs(clonedProcesses, n);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,n);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("\n----- shortest Job First Scheduling -----\n");
    head = sjf(clonedProcesses, n);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,n);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("\n----- shortest Remaining Time First Scheduling -----\n");
    head = shortestRemainingTimeFirst(clonedProcesses, n);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,n);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("\n----- MLFQ Scheduling -----\n");
    printf("Enter time slice for high-priority queue : ");
    scanf("%lf", &time_slice_high);
    printf("Enter time slice for medium-priority queue : ");
    scanf("%lf", &time_slice_med);
    printf("Enter time slice for low-priority queue : ");
    scanf("%lf", &time_slice_low);
    printf("Enter boost time: ");
    scanf("%lf", &boost);
    head = mlfq(clonedProcesses, n, time_slice_high, time_slice_med, time_slice_low, boost);
    printLog(head);
    printAvgTurnaroundResponse(clonedProcesses,n);
    
    return 0;
}





