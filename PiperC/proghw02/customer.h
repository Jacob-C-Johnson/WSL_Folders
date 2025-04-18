/*
 * customer.h - prototype functions and struct for a customer
 *
 * Author: clintf
 * Student Name: Piper Bliss
 * Date: 4/16/2025
 * Assignment:HW-Prog04
 * Course: CSCI 356
 * Version 1.0
 */
#ifndef CUSTOMER_H_
#define CUSTOMER_H_

//struct to hold customer information
typedef struct {
    char name[50];
    int credit_rating;
} Customer;

//function to create a customer with the name and rating
Customer* create_customer(const char* name, int rating);

//function to print the customer information and the distance from the average
void print_customer_info(Customer* customer, int average);

#endif