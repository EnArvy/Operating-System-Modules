#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <unistd.h> // For usleep function
#include <limits.h> 

// Function to generate an exponential random variable in microseconds
double generateExponentialRandom(double lambda) {
    return -log(1.0 - (double)rand() / (RAND_MAX + 1.0)) * 1e6 / lambda;
    // The multiplication by 1e6 converts seconds to microseconds
}

// Structure to represent a process
struct Process {
    int pid;                  // Process ID
    int arrival_time;         // Arrival time in microseconds
    int burst_time;           // Burst time in microseconds
    int remaining_time;       // Remaining time in microseconds
    int turnaround_time;      // Turnaround time in microseconds
    int waiting_time;         // Waiting time in microseconds
};

// Function to simulate the Round Robin scheduling algorithm
void roundRobin(struct Process processes[], int n, int time_slice) {
    int remaining_processes = n;
    int current_time = 0;
    int quantum = time_slice * 1e6; // Convert time slice to microseconds
    int current_process = 0;

    while (remaining_processes > 0) {
        if (processes[current_process].remaining_time > 0) {
            int execution_time = (processes[current_process].remaining_time < quantum)
                                    ? processes[current_process].remaining_time
                                    : quantum;

            current_time += execution_time;
            processes[current_process].remaining_time -= execution_time;

            if (processes[current_process].remaining_time == 0) {
                remaining_processes--;
                processes[current_process].turnaround_time = current_time - processes[current_process].arrival_time;
                processes[current_process].waiting_time = processes[current_process].turnaround_time - processes[current_process].burst_time;

                printf("Process %d (Round Robin): Arrival Time %d microseconds, Finished Time %d microseconds\n",
                       processes[current_process].pid,
                       processes[current_process].arrival_time,
                       current_time);
            }
        }

        current_process = (current_process + 1) % n;

        // Sleep for the time slice in microseconds
        // usleep(quantum);
    }
}

// Function to simulate the First Come First Serve scheduling algorithm
void fcfs(struct Process processes[], int n) {
    int current_time = 0;

    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time > current_time) {
            current_time = processes[i].arrival_time;
        }

        processes[i].turnaround_time = current_time + processes[i].burst_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;

        printf("Process %d (FCFS): Arrival Time %d microseconds, Finished Time %d microseconds\n",
               processes[i].pid,
               processes[i].arrival_time,
               current_time + processes[i].burst_time);

        current_time += processes[i].burst_time;
    }
}


// Function to simulate the Shortest Job First scheduling algorithm
void sjf(struct Process processes[], int n) {
    int current_time = 0;
    int completed = 0;

    while (completed < n) {
        int shortest_job_index = -1;
        int shortest_burst_time = INT_MAX;
        bool job_found = false;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].burst_time < shortest_burst_time) {
                    shortest_burst_time = processes[i].burst_time;
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
                processes[shortest_job_index].waiting_time = processes[shortest_job_index].turnaround_time - processes[shortest_job_index].burst_time;

                printf("Process %d (SJF): Arrival Time %d microseconds, Finished Time %d microseconds\n",
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

    struct Process processes[n];

    // Set the seed for the random number generator
    srand(time(NULL));

    printf("Enter the mean inter-arrival time (in microseconds): ");
    scanf("%lf", &meanInterArrivalTime);

    printf("Enter the mean job duration (in microseconds): ");
    scanf("%lf", &meanJobDuration);

    for (int i = 0; i < n; i++) {
        processes[i].arrival_time = (int)generateExponentialRandom(meanInterArrivalTime);
        processes[i].burst_time = (int)generateExponentialRandom(meanJobDuration);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].pid = i + 1;
    }

  // Sort processes by arrival time (ascending order)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                struct Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
    printf("Enter time slice for Round Robin (in microseconds): ");
    scanf("%d", &time_slice);

    printf("----- Round Robin Scheduling -----\n");
    roundRobin(processes, n, time_slice);

    printf("\n----- First Come First Serve Scheduling -----\n");
    fcfs(processes, n);

    printf("\n----- Shortest Job First Scheduling -----\n");
    sjf(processes, n);

    return 0;
}





