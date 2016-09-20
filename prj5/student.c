/*
 * student.c
 * Multithreaded OS Simulation for CS 2200, Project 5
 * Spring 2016
 *
 * This file contains the CPU scheduler for the simulation.
 * Name: Mingjun Xie
 * GTID: 903074907
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"


/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;
static pthread_mutex_t queue_mutex;
static pthread_cond_t idle_cond;
static pcb_t* head;
static int quantum; // this is for time slice used by rr scheduler
static int cpus; //count for number of cpus
static int scheduler; // the type of scheduling algorigthm:1. fifo 2.rr 3.sp

pcb_t* dequeue();
void enqueue(pcb_t* pcb);
pcb_t* get_highest();
/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running
 *	process indexed by the cpu id. See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&queue_mutex);
    pcb_t* head_b;
    if (scheduler != 3) {
        head_b = dequeue();
    } else {
        head_b = get_highest();
    }
    pthread_mutex_unlock(&queue_mutex);
    if (head_b == NULL) {
        context_switch(cpu_id, NULL, quantum);
    } else {
        head_b -> state = PROCESS_RUNNING;
        pthread_mutex_lock(&current_mutex);
        current[cpu_id] = head_b;
        pthread_mutex_unlock(&current_mutex);
        context_switch(cpu_id, head_b, quantum);
    }
}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    /* FIX ME */
    //schedule(0);
    pthread_mutex_lock(&queue_mutex);
    while (head == NULL) {
        pthread_cond_wait(&idle_cond, &queue_mutex);
    }
    pthread_mutex_unlock(&queue_mutex);
    schedule(cpu_id);
    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    //mt_safe_usleep(1000000);
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */
    pcb_t* cur = current[cpu_id];
    cur -> state = PROCESS_READY;
    cur -> next = NULL;
    // if (head == NULL) {
    //     head = cur;
    // } else {
    //     pcb_t* temp = head;
    //     while (temp -> next != NULL) {
    //         temp = temp -> next;
    //     }
    //     temp -> next = cur;
    // }
    enqueue(cur);
    schedule(cpu_id);

}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&current_mutex);
    current[cpu_id] -> state = PROCESS_WAITING;
    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&current_mutex);
    current[cpu_id] -> state = PROCESS_TERMINATED;
    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is static priority, wake_up() may need
 *      to preempt the CPU with the lowest priority process to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a higher priority than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    /* FIX ME */
    process -> state = PROCESS_READY;
    enqueue(process);
    if (scheduler == 3) {
        pthread_mutex_lock(&current_mutex);
        int min = process -> static_priority;
        int end = 0;
        int sp;
        int id = -1;
        for (int i = 0; i < cpus && !end; i++) {
            pcb_t* cur = current[i];
            if (cur != NULL) {
                sp = cur -> static_priority;
                if (sp < min) {
                    min = sp;
                    id = i;
                }
            } else {
                end = 1;
            }
        }
        pthread_mutex_unlock(&current_mutex);
        if (id != -1) {
            force_preempt(id);
        }


    }


}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -p command-line parameters.
 */
int main(int argc, char *argv[])
{
    int cpu_count;

    /* Parse command-line arguments */
    if (argc < 2 || argc > 4)
    {
        fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n"
            "         -p : Static Priority Scheduler\n\n");
        return -1;
    }
    cpu_count = atoi(argv[1]);

    /* FIX ME - Add support for -r and -p parameters*/
    cpus = cpu_count;
    scheduler = 0;
    if (argc == 2) {
        scheduler = 1;
        quantum = -1;
    } else {
        if (argv[2][1] == 'r') {
            scheduler = 2;
            quantum = atoi(argv[3]);
        } else if (argv[2][1] == 'p') {
            scheduler = 3;
            quantum = -1;
        }
    }

    if (scheduler == 0) {
        return 0;
    }

    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}


/*
 * These functions are helper functions.
 */
void enqueue(pcb_t* process) {
    pcb_t* temp;
    process->next = NULL;
    if (head == NULL) {
        head = process;
    } else {
        temp = head;
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = process;
    }
    pthread_cond_signal(&idle_cond);
}

pcb_t* dequeue() {
    pcb_t* temp;

    if (head == NULL) {
        return NULL;
    } else {
        temp = head;
        head = head -> next;
    }

    return temp;
}

pcb_t* get_highest() {
    pcb_t* max;

    if (head == NULL) {
        return NULL;
    } else {
        max = head;
        pcb_t* temp = head;
        while (temp != NULL) {
            if (max -> static_priority < temp -> static_priority) {
                max = temp;
            }
            temp = temp -> next;
        }
    }
    pcb_t* atemp = head;
// need to take max out of the queue
    if (max -> pid == head -> pid) {
        head = head -> next;
    } else {
        while (atemp -> next -> pid != max -> pid) {
            atemp = atemp -> next;
        }
        atemp -> next = max -> next; // remove max out of the queue
    }

    return max;

}