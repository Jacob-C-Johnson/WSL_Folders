/*
 * utilities.c - Implementation of pi functions
 *
 * Author: Jacob Johnson
 * Date: 02/03/2025
 *
 * Assignment: Assignment 2
 * Course: CSCI 473
 * Version 1.0
 */

#include "utilities.h"
#include <stdio.h>
#include<time.h>
#include <sys/time.h>
#ifndef _TIMER_H_
#define _TIMER_H_

#define GET_TIME(now) { \
    struct timeval t; \
    gettimeofday(&t, NULL); \
    now = t.tv_sec + t.tv_usec/1000000.0; \
}
#endif

double compute_pi(long int n) {
    // varibles for calculation
    long int i;
    double sum = 0.0;
    double alternator = 1.0;
    // variables for timing
    double start, end;
    double diff;

    // start timer
    GET_TIME(start);

    // Estimate pi
    for (i = 0; i < n; i++) {
        sum += alternator / (2 * i + 1);
        alternator = -alternator;
    }
    double estimate_pi = 4.0 * sum;

    // end timer
    GET_TIME(end);

    diff = end - start;

    // print results
    printf("Estimate of pi is %lf\n", estimate_pi);
    printf("Time to compute pi: %lf\n", diff);

    return estimate_pi;
}
