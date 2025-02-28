#include "Fraction.h"
#include <stdio.h>

// Prints gretting
void printGreeting() {
    printf("Welcome to the Fraction Tester.\n");
}

// Gets the user input instructs to enter negative to quit
void getUserInput(Fraction *frac) {
    printf("Please enter the numerator and denominator separated by a space (negative number to quit): ");
    scanf("%d %d", &frac->numerator, &frac->denominator);
}

// Find greatest common denomiator to simplify
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Simpilfies the fraction
Fraction reduceFraction(Fraction frac) {
    if (frac.denominator == 0) {
        printf("denominator cannot be zero.\n");
        frac.numerator = 0;
        frac.denominator = 1;
        return frac;
    }
    int divisor = gcd(frac.numerator, frac.denominator);
    frac.numerator /= divisor;
    frac.denominator /= divisor;
    return frac;
}

// Outputs the fraction
void printFraction(Fraction frac) {
    if (frac.denominator == 0) {
        printf("Invalid fraction.\n");
    } else if (gcd(frac.numerator, frac.denominator) == 1) {
        printf("The fraction is: %d/%d\n", frac.numerator, frac.denominator);
    } else {
        Fraction reduced = reduceFraction(frac);
        printf("The fraction %d/%d is simplified to: %d/%d\n",
               frac.numerator, frac.denominator, reduced.numerator, reduced.denominator);
    }
}

// Says goodbye
void printGoodbye() {
    printf("Thanks for using Fraction Tester, have a nice day!\n");
}


int main() {
    Fraction f;
        printGreeting();
    
        while (1) {
            getUserInput(&f);
    
            // If the user enters a negative numerator, we stop
            if (f.numerator < 0 || f.denominator <= 0) {
                printf("Thanks for using Fraction Tester, have a nice day!\n");
                break;
            }
    
            printFraction(f);
            f = reduceFraction(f);
    
            // Print the reduced fraction
            printf("The Fraction entered is reduced to: %d/%d\n\n", f.numerator, f.denominator);
        }
    
        return 0;
    }