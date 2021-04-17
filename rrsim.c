#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;

void print_task(taskval_t *t, void *arg) {
    printf("task %03d: %5d %3.2f %3.2f\n",
        t->id,
        t->arrival_time,
        t->cpu_request,
        t->cpu_used
    );  
}


void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}

void print_tick(int tick){
	printf("[%05d] ",tick);
}

taskval_t* check_if_task_arrived(taskval_t *ready_q, int current_tick){
   	taskval_t *task_p;
	if(event_list == NULL){
		return ready_q;
	}

    //we use a while loop in case multiple tasks arrive at the same time
	while(current_tick == event_list->arrival_time){

		task_p = peek_front(event_list);

		event_list = remove_front(event_list);

		ready_q = add_end(ready_q, task_p);

		if(event_list == NULL){
			return ready_q;
		}
	}
	return ready_q;
}
taskval_t* remove_and_dispatch(taskval_t *ready_q, taskval_t *current_task){
	//printf("\n Ready Queue before remove_and_dis:\n");
	//apply(ready_q, print_task, NULL);
	
	if(current_task != NULL){
		ready_q = remove_front(ready_q);
		ready_q = add_end(ready_q, current_task);
	//	printf("%d\n",ready_q->id);
	}
	current_task = peek_front(ready_q);

	//printf("After:\n");
	//apply(ready_q,print_task,NULL);
	return ready_q;
}

taskval_t* task_exit(taskval_t* t_exit, int finish_tick){
	printf("id=%05d EXIT w=%.2f ta=%.2f\n",
			t_exit->id,
			finish_tick - t_exit->arrival_time - t_exit->cpu_request,
			(float)finish_tick - t_exit->arrival_time);
	end_task(t_exit);
	//printf("Task ended successfully\n");
	return NULL;
}

//Each tick we must:
//	1. Check if there is a task arriving we can add to the ready queue #DONE#
//	2. If no task is running, dispatch one from the ready queue
//	3. If no task to dispatch, idle until there is one
//	4. If a task is currently running, keep running it until:
//		a. It uses up its quantum
//		b. It completes 
//	5. If the ready queue, event queue are both empty, and no task is running, end
void run_simulation(int qlen, int dlen) {
    taskval_t *ready_q = NULL;		//our ready queue
    taskval_t *current_task = NULL;	//the task currently being processed, not in ready_queue
    int tick;				//the current tick
    int remaining_qtime = 0;		//the remaining time of a particular quantum
    int remaining_dtime = 0;		//the remaining dispatch time
    //char* output;			//a string which we print as output
    tick = 0;
	

    while(!(event_list == NULL && ready_q == NULL && current_task == NULL)){
	    print_tick(tick);
	    ready_q = check_if_task_arrived(ready_q, tick);
	    if(remaining_qtime == 0 && remaining_dtime == 0){
	    	if((ready_q = remove_and_dispatch(ready_q, current_task)) != NULL){
			current_task = peek_front(ready_q);
			//printf("Outside of remove_and_dis:\n");
			//apply(ready_q, print_task, NULL);
			remaining_qtime = qlen;
			remaining_dtime = dlen;
		} else {
			printf("IDLE\n");
		}
	    }

	    if (remaining_dtime > 0){
		    printf("DISPATCHING\n");
		    remaining_dtime--;
	    } else if(remaining_qtime >0){
		    //current_task->cpu_used++;
		    if(current_task->cpu_used >= current_task->cpu_request){
			    if(current_task->cpu_used == current_task->cpu_request){
				    //we have used the whole tick
				    ready_q = remove_front(ready_q);
				    current_task = task_exit(current_task, tick);
				    //printf("Exit 1\n");
				    remaining_qtime = 0;
				    remaining_dtime = 0;
			    } else {
				    ready_q = remove_front(ready_q);
				    current_task = task_exit(current_task,tick);
				    //printf("Exit 2\n");
				    //COPY the whole dispatch process. Maybe make a function of it
				    if((ready_q = remove_and_dispatch(ready_q, current_task)) != NULL){
					    current_task = peek_front(ready_q);
					    remaining_qtime = qlen;
					    if(dlen > 0){	    
					    	remaining_dtime = dlen - 1;
					    	print_tick(tick);
					    	printf("DISPATCHING \n");
					    }
				    }
			    }
		    } else {
			    printf("id=%05d req=%.2f used=%.2f\n",
					    current_task->id,
					    current_task->cpu_request,
					    current_task->cpu_used);
			    current_task->cpu_used++;
			    remaining_qtime--;
		    }
	    }
	  // printf("Ready q at the end of tick %d:\n",tick);
	  //apply(ready_q, print_task, NULL); 
	    tick++;
	    //if(tick == 300){
		//    apply(ready_q, print_task, NULL);
		  //  break;
	    //}
		
			

/*
	    //if we have no task, and there is a task ready, run that task
	    if(current_task == NULL && ready_q != NULL){
		    current_task = peek_front(ready_q);
		    ready_q = remove_front(ready_q);
		    current_task->next = NULL;
		    remaining_qlen = qlen + dlen;
	    }
	    
	    if(remaining_qlen > qlen){
		    output = "DISPATCHING\n";  
	    } else if (remaining_qlen > 0) {
		    printf("id=%05d req=%.2f used=%.2f",
				current_task->id,
	    			current_task->cpu_request,
				current_task->cpu_used);			
		    current_task->cpu_used += 1;
		    output = "\n";
		    //must fix later? Do something if we do not use whole tick
	    } else if ((remaining_qlen == 0) && (current_task != NULL)){
		if(current_task->cpu_used < current_task->cpu_request){
			ready_q = add_end(ready_q, current_task);
			current_task = NULL;
		}
	    } else {
		    output = "IDLE\n";
	    }
	    printf("%s", output);
	    remaining_qlen--;
	    tick++;
*/
    }
}


int main(int argc, char *argv[]) {
    char   input_line[MAX_BUFFER_LEN];
    int    i;
    int    task_num;
    int    task_arrival;
    float  task_cpu;
    int    quantum_length = -1;
    int    dispatch_length = -1;

    taskval_t *temp_task;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quantum") == 0 && i+1 < argc) {
            quantum_length = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--dispatch") == 0 && i+1 < argc) {
            dispatch_length = atoi(argv[i+1]);
        }
    }

    if (quantum_length == -1 || dispatch_length == -1) {
        fprintf(stderr, 
            "usage: %s --quantum <num> --dispatch <num>\n",
            argv[0]);
        exit(1);
    }


    while(fgets(input_line, MAX_BUFFER_LEN, stdin)) {
        sscanf(input_line, "%d %d %f", &task_num, &task_arrival,
            &task_cpu);
        temp_task = new_task();
        temp_task->id = task_num;
        temp_task->arrival_time = task_arrival;
        temp_task->cpu_request = task_cpu;
        temp_task->cpu_used = 0.0;
        event_list = add_end(event_list, temp_task);
    }

#ifdef DEBUG
    int num_events;
    apply(event_list, increment_count, &num_events);
    printf("DEBUG: # of events read into list -- %d\n", num_events);
    printf("DEBUG: value of quantum length -- %d\n", quantum_length);
    printf("DEBUG: value of dispatch length -- %d\n", dispatch_length);
#endif

    run_simulation(quantum_length, dispatch_length);

    return (0);
}
