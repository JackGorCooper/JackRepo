/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */
int count;
pthread_mutex_t join_mutex;
pthread_cond_t EntryQueue;
pthread_cond_t MeetupQueue;
int generation;
int meetup_size;
int F_or_L;
int need_to_leave;
resource_t codeword;

void initialize_meetup(int n, int mf) {
    char label[100];
    int i;
    int status = 0;
    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
    count = 0;
    need_to_leave =0;
    generation = 1;
    meetup_size = n;
    F_or_L = mf;

    if(pthread_cond_init(&MeetupQueue, NULL)) {
	    fprintf(stderr, "Could not initialize MeetupQueue\n");
	    exit(1);
    }

    if(pthread_mutex_init(&join_mutex, NULL)) {
	    fprintf(stderr, "Could not initialize join_mutex\n");
	    exit(1);
    }
    if(pthread_cond_init(&EntryQueue, NULL)) {
	    fprintf(stderr, "Could not initialize EntryQueue\n");
	    exit(1);
    }

    init_resource(&codeword, label);
}

//join_meetup was created by following peudo-code provided on page 53 of the LittleBookOfSemaphores.pdf
//about reusable barrriers, and following code provided on slide 73 of the 05-synchronization slides 
void join_meetup(char *value, int len) {

	pthread_mutex_lock(&join_mutex);
	while(need_to_leave != 0){
		pthread_cond_wait(&EntryQueue, &join_mutex);
	}
	if((F_or_L == MEET_FIRST && count == 0) || (F_or_L == MEET_LAST && count == meetup_size-1)){
		write_resource(&codeword, value, len);
	}
        count++;	
       	if (count < meetup_size) {
		int my_generation = generation;
		while (my_generation == generation){
			pthread_cond_wait(&MeetupQueue, &join_mutex);
		}
	} else {
		count = 0;
		need_to_leave = meetup_size;
		generation++;
		pthread_cond_broadcast(&MeetupQueue);
	}
	read_resource(&codeword, value, len);
	need_to_leave--;
	if(need_to_leave==0){
		pthread_cond_broadcast(&EntryQueue);
	}
	pthread_mutex_unlock(&join_mutex);
}

/* Writing code from lecture to aid with understanding and for ease of referral.
 * barrier code from lectures:
 *
 * int 	count;
 * int 	n=3; Three threads
 * pthread_mutex_t m;
 * pthread_cond_t BarrierQueue;
 * int 	generation;
 *
 * void barrier() {
 * 	pthread_mutex_lock(&m);
 * 	count++;
 * 	if (count < n) {
 * 		int my_gneration = generation;
 * 		while (my_generation == generation) {
 * 			pthread_cond_wait(&BarrierQueue, &m);
 * 		}
 * 	} else {
 * 		count = 0;
 * 		generation++;
 * 		pthread_cond_broadcast(&BarrierQueue);
 * 	}
 * 	pthread_mutex_unlock(&m);
 * }
 * We have a mutex and condition variable, along with a counter and generation counter
 * A thread calls barier, and attempts to lock the mutex
 * When it succeeds, it increments the count, and if there are still more threads to block, it sets 
 * its generation as the current one, and then waits inside a loop (in case of spontaneous wakeup)
 *
 * If the end of the generation has indeed been reached, count it reset, generation incremented, and all
 * threads blocked by the barrier are allowed through using a broadcast. The thread then unlocks the
 * mutex
 * */
/* Reader writer soln using no semaphores from lecture
 *
 * reader() {
 * 	pthread_mutex_lock(&m);
 * 	while(!(writers == 0)) {
 * 		pthread_cond_wait(&readersQ, &m)
 * 	}
 * 	readers++;
 * 	pthread_mutex_unlock(&m)l
 *
 *	do reading
 *
 *	pthread_mutex_lock(&m);
 *	if(--readers==0){
 *		pthread_cond_signal(&writersQ);
 *	}
 *	pthread_mutex_unlock(&m);
 *}

  writer() {
  	pthread_mutex_lock(&m);
	while(!((readers==0)&&(writers==0))){
		pthread_cond_wait(&writersQ, &m);
	}
	writers++;
	pthread_mutex_unlock(&m);

	do writing

	pthread_mutex_lock(&m);
	writers--;
	pthread_cond_signal(&writersQ);
	pthread_cond_broadcast(&readersQ);
	pthread_mutex_unlock(&m);
}
*/
