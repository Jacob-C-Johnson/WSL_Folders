/*
 * customer.h - the main function to call the queue and print information
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
#include <string.h>
#include "my_queue.h"
#include "customer.h"

#define MAX_INPUT 100

int main() {
    //Ask the user for input
    printf("Enter name and credit rating separated by a space.\n");
    printf("Enter on a blank line to finish input.\n");

    queue q = newqueue();// Create a new queue
    char input[MAX_INPUT];
    int sum = 0;
    int count = 0;
    int min_rating = 1000000;

    // Read user input
    while (1) {
        printf("> ");
        if (!fgets(input, MAX_INPUT, stdin)) break;
        if (strcmp(input, "\n") == 0) break;

        char name[50];
        int rating;
        if (sscanf(input, "%s %d", name, &rating) != 2) {
            printf("Invalid input. Format: Name Rating\n");
            continue;
        }

        Customer* c = create_customer(name, rating);
        enqueue(q, c);
        sum += rating;
        count++;

        if (rating < min_rating) {
            min_rating = rating;
        }
    }

    if (count == 0) {
        printf("No customer data entered.\n");
        return 0;
    }

    double average = (double)sum / count;

    printf("\n%-10s %-10s %-10s\n", "Name", "Data", "Distance");

    // Process each customer
    while (!isempty(q)) {
        Customer* c = (Customer*)dequeue(q);
        print_customer_info(c, (int)average);
        free(c);
    }

    printf("\nAverage: %.0f\n", average);
    printf("Minimum rating entered: %d\n", min_rating);

    return 0;
}

// Create a new customer with the given name and credit rating
Customer* create_customer(const char* name, int rating) {
    Customer* new_customer = (Customer*)malloc(sizeof(Customer));
    if (!new_customer) {
        fprintf(stderr, "Memory allocation failed for customer.\n");
        exit(1);
    }
    strncpy(new_customer->name, name, sizeof(new_customer->name) - 1);
    new_customer->name[sizeof(new_customer->name) - 1] = '\0'; // Ensure null termination
    new_customer->credit_rating = rating;
    return new_customer;
}

// Print customer information prints the name, credit rating, and distance from the average
void print_customer_info(Customer* customer, int average) {
    int distance = customer->credit_rating - average;
    printf("%-10s %-10d %-10d\n", customer->name, customer->credit_rating, distance);
}