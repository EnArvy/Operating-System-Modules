#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <unistd.h> // For usleep function
#include <limits.h> 

// Function to generate an exponential random variable in microseconds
double generateExponentialRandom(double lambda) {
    return -log(1.0 - (double)rand() / (RAND_MAX + 1.0))/ lambda;
    // The multiplication by 1e6 converts seconds to microseconds
}

// Structure to represent a process
struct Process {
    int pid;                  // Process ID
    int arrival_time;         // Arrival time in microseconds
    int job_time;             // Job time in microseconds
    int remaining_time;       // Remaining time in microseconds
    int turnaround_time;      // Turnaround time in microseconds
    int waiting_time;         // Waiting time in microseconds
};

// Function to simulate the Round Robin scheduling algorithm
void roundRobin(struct Process processes[], int n, int time_slice) {
    int remaining_processes = n;
    int current_time = 0;
    int current_process = 0;

    while (remaining_processes > 0) {
        if (processes[current_process].remaining_time > 0) {
            int execution_time = (processes[current_process].remaining_time < time_slice)
                                    ? processes[current_process].remaining_time
                                    : time_slice;

            current_time += execution_time;
            processes[current_process].remaining_time -= execution_time;

            printf("%d Start %d, Finish %d\n",
                       processes[current_process].pid,
                       current_time - execution_time,
                       current_time);

            if (processes[current_process].remaining_time == 0) {
                remaining_processes--;
                processes[current_process].turnaround_time = current_time - processes[current_process].arrival_time;
                processes[current_process].waiting_time = processes[current_process].turnaround_time - processes[current_process].job_time;
            }
        }

        current_process = (current_process + 1) % n;
    }
}

// Function to simulate the First Come First Serve scheduling algorithm
void fcfs(struct Process processes[], int n) {
    int current_time = 0;

    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time > current_time) {
            current_time = processes[i].arrival_time;
        }

        processes[i].turnaround_time = current_time + processes[i].job_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].job_time;

        printf("%d Start %d, Finish %d\n",
               processes[i].pid,
               processes[i].arrival_time,
               current_time + processes[i].job_time);

        current_time += processes[i].job_time;
    }
}

// Function to perform non-preemptive SJF scheduling
void sjfNonPreemptive(struct Process processes[], int n) {
    int current_time = 0;
    int completed = 0;
    bool executed[n]; // Array to track if a process has been executed

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
            processes[shortest_job_index].waiting_time = processes[shortest_job_index].turnaround_time - processes[shortest_job_index].job_time;

            // Print process details
            printf("%d Start %d, Finish %d\n",
                   processes[shortest_job_index].pid,
                   processes[shortest_job_index].arrival_time,
                   current_time);

            // Mark the process as executed
            executed[shortest_job_index] = true;
            completed++;
        } else {
            // If no process is ready to execute, move time forward
            current_time++;
        }
    }
}


// Function to simulate the Shortest Jobtime remaining First scheduling algorithm
void shortestRemainingTimeFirst(struct Process processes[], int n) {
    int current_time = 0;
    int completed = 0;

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

            if (processes[shortest_job_index].remaining_time == 0) {
                completed++;
                processes[shortest_job_index].turnaround_time = current_time - processes[shortest_job_index].arrival_time;
                processes[shortest_job_index].waiting_time = processes[shortest_job_index].turnaround_time - processes[shortest_job_index].job_time;

                printf("%d Start %d, Finish %d\n",
                       processes[shortest_job_index].pid,
                       processes[shortest_job_index].arrival_time,
                       current_time);
            }
        }
    }
}


int main() {
    int n;  // Number of processes
    int time_slice; // Time slice for Round Robin
    double meanInterArrivalTime;
    double meanJobDuration;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    struct Process processes[n], clonedProcesses[n];

    // Set the seed for the random number generator
    srand(time(NULL));

    printf("Enter the mean inter-arrival time (in microseconds): ");
    scanf("%lf", &meanInterArrivalTime);

    printf("Enter the mean job duration (in microseconds): ");
    scanf("%lf", &meanJobDuration);

    processes[0].arrival_time = ceil(generateExponentialRandom(1.0/meanInterArrivalTime));
    processes[0].job_time = ceil(generateExponentialRandom(1.0/meanJobDuration));
    processes[0].remaining_time = processes[0].job_time;
    processes[0].pid = 1;
    for (int i = 1; i < n; i++) {
        processes[i].arrival_time = ceil(generateExponentialRandom(1.0/meanInterArrivalTime))+processes[i-1].arrival_time;
        processes[i].job_time = ceil(generateExponentialRandom(1.0/meanJobDuration));
        processes[i].remaining_time = processes[i].job_time;
        processes[i].pid = i + 1;
        // printf("%d %d %d\n",processes[i].pid,processes[i].job_time,processes[i].arrival_time);
    }

    printf("Enter time slice for Round Robin (in microseconds): ");
    scanf("%d", &time_slice);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("----- Round Robin Scheduling -----\n");
    roundRobin(clonedProcesses, n, time_slice);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("\n----- First Come First Serve Scheduling -----\n");
    fcfs(clonedProcesses, n);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("\n----- shortest Job First Scheduling -----\n");
    sjfNonPreemptive(processes, n);

    // Copy elements from the original processes array to the cloned array
    for (int i = 0; i < n; i++) clonedProcesses[i] = processes[i];
    printf("\n----- shortest Remaining Time First Scheduling -----\n");
    shortestRemainingTimeFirst(processes, n);

    return 0;
}





