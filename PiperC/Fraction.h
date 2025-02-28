#ifndef FRACTION_H
#define FRACTION_H

#include <stdbool.h>

typedef struct {
    int numerator;
    int denominator;
} Fraction;

void printGreeting();
void getUserInput(Fraction *frac); 
int gcd(int a, int b);
Fraction reduceFraction(Fraction frac);
void printFraction(Fraction frac);
void printGoodbye();

#endif