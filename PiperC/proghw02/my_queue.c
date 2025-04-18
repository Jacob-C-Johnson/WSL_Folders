/*
 * my_queue.c - the finished functions for the queue
 *
 * Author: clintf
 * Student Name: Piper Bliss
 * Date: 4/16/2025
 * Assignment:HW-Prog04
 * Course: CSCI 356
 * Version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "my_queue.h"

// create a new queue
queue newqueue() {
  queue q = (queue)malloc(sizeof(struct queueS));
  if (q == NULL) {
    fprintf(stderr, "malloc failed in newqueue\n");
    exit(1);
  }
  q->front = NULL;
  return q;
}

//check if the queue is empty
int isempty (const queue q){
    if (q == NULL) {
        fprintf(stderr, "queue is NULL in isempty\n");
        exit(1);
    }
    return (q->front == NULL);
}

// add an item to the end of the queue
void enqueue (queue q, void* item){
    if (q == NULL) {
        fprintf(stderr, "queue is NULL in enqueue\n");
        exit(1);
    }
    q_element* new_element = (q_element*)malloc(sizeof(q_element));
    if (new_element == NULL) {
        fprintf(stderr, "malloc failed in enqueue\n");
        exit(1);
    }
    new_element->contents = item;
    new_element->next = NULL;

    if (isempty(q)) {
        q->front = new_element;
    } else {
        q_element* current = q->front;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_element;
    }
}

// remove the first item from the queue
void* dequeue (queue q){
    if (q == NULL) {
        fprintf(stderr, "queue is NULL in dequeue\n");
        exit(1);
    }
    if (isempty(q)) {
        fprintf(stderr, "queue is empty in dequeue\n");
        return NULL;
    }
    q_element* front_element = q->front;
    void* item = front_element->contents;
    q->front = front_element->next;
    free(front_element);
    return item;
}

// peek at the first item in the queue without removing it
void* peek (queue q){
    if (q == NULL) {
        fprintf(stderr, "queue is NULL in peek\n");
        exit(1);
    }
    if (isempty(q)) {
        fprintf(stderr, "queue is empty in peek\n");
        return NULL;
    }
    return q->front->contents;
}