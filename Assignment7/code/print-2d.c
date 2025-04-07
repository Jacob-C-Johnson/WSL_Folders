#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    const char *file_name = argv[1];
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
     
    Print_matrix(file_name);
    fclose(file);
    

    return 0;
}

