/*
 * my_queue.h - prototype functions for a queue
 *
 * Author: clintf
 * Student Name: Piper Bliss
 * Date: 4/16/2025
 * Assignment:HW-Prog04
 * Course: CSCI 356
 * Version 1.0
 */


#ifndef MY_QUEUE_H_
#define MY_QUEUE_H_



struct q_elementS {
	void* contents;				// the queue item
	struct q_elementS* next; // pointer to the next item in queue
};


typedef struct q_elementS q_element;

struct queueS {
	q_element* front;
};

//Clint change stack to queue
typedef struct queueS* queue;	// a queue is a pointer


/*
 * creates a queue
 * returns: a pointer to a queue
 */
queue newqueue();

/*
 * checks the status of a queue
 * queue q: a queue to check for emptiness; q must not be NULL
 * returns: value is > 0 iff queue has no elements
 */
int isempty (const queue q);

/*
 * adds item to end of queue
 * queue q: 	a queue to append; q must not be NULL
 * void* item:	a pointer to an item to be enqueued onto queue
 * returns:		item appended to queue
 */
void enqueue (queue q, void* item);

/*
 * dequeues first item from queue
 * queue q: a queue to remove an item: q must not be NULL
 * returns: item returned was front of queue and next element
 * 			is new head of queue
 */
void* dequeue (queue q);


/*
 * allows fo first item from queue to be examined
 * queue q: a queue to check: q must not be NULL
 * returns: item returned is current front of queue and
 * 			queue is left unaltered
 */
void* peek (queue q);


#endif /* MY_QUEUE_H_ */

