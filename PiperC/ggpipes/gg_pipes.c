/***********************************************
 * gg_pipes.c - The parent process spawns      *
 *  children that randomly choose numbers until*
 *  signaled to stop/start                     *
 *                                             *
 * Name: Piper Bliss                           *
 * Assignment: HW-Prog02                       *
 * Email: pabliss@coastal.edu                  *
 * Date: 3/5/2025                              *
 ***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define MAX_ROUNDS 10
#define BUFFER_SIZE 10
#define MAX_GUESS 10
#define MIN_GUESS 1

/* Used to write operations */
int safe_write(int fd, const char *buffer, size_t size) {
    size_t bytes_written = write(fd, buffer, size);
    if (bytes_written != size) {
        perror("Failed");
        return -1;
    }
    return 0;
}

/* to read operations */
int safe_read(int fd, char *buffer, size_t size) {
    ssize_t bytes_read = read(fd, buffer, size);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            fprintf(stderr, "Pipe closed\n");
        } else {
            perror("Failed");
        }
        return -1;
    }
    return bytes_read;
}

int main(int argc, char *argv[]) {
    int rounds;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_rounds>\n", argv[0]);
        return 1;
    }
    
    rounds = atoi(argv[1]);
    if (rounds <= 0 || rounds > MAX_ROUNDS) {
        fprintf(stderr, "Number of rounds must be between 1 and %d\n", MAX_ROUNDS);
        return 1;
    }
    
    // Random seed for parent process
    srand(time(NULL));
    
    // Create pipes for communication
    int parent_to_child1[2]; // Parent writes Child 1 reads
    int child1_to_parent[2]; // Child 1 writes Parent reads
    int parent_to_child2[2]; // Parent writes Child 2 reads
    int child2_to_parent[2]; // Child 2 writes Parent reads
    
    if (pipe(parent_to_child1) < 0 || pipe(child1_to_parent) < 0 ||
        pipe(parent_to_child2) < 0 || pipe(child2_to_parent) < 0) {
        perror("Pipe creation failed");
        return 1;
    }
    
    printf("The Guessing Game\n");
    
    // First child process
    pid_t pid1 = fork();
    
    if (pid1 < 0) {
        perror("Fork failed");
        return 1;
    }
    
    if (pid1 == 0) {
        
        // Initialize a different random seed for child 1
        srand(time(NULL) ^ getpid());
        
        // Close unused pipe ends
        close(parent_to_child1[1]);
        close(child1_to_parent[0]);
        close(parent_to_child2[0]);
        close(parent_to_child2[1]);
        close(child2_to_parent[0]);
        close(child2_to_parent[1]);
        
        char buffer[BUFFER_SIZE];
        
        // Game loop for Child 1
        while (1) {
            // Wait for parent signal
            if (safe_read(parent_to_child1[0], buffer, BUFFER_SIZE) < 0) {
                break; // Exit if pipe closed or there is a problem
            }
            
            // Check if parent said to exit
            if (strcmp(buffer, "exit") == 0) {
                break;
            }
            
            // Make guess and send to parent
            int guess = (rand() % MAX_GUESS) + MIN_GUESS;
            sprintf(buffer, "%d", guess);
            if (safe_write(child1_to_parent[1], buffer, strlen(buffer) + 1) < 0) {
                break;
            }
        }
        
        // Close pipe ends
        close(parent_to_child1[0]);
        close(child1_to_parent[1]);
        
        return 0;
    }
    
    // Create second child
    pid_t pid2 = fork();
    
    if (pid2 < 0) {
        perror("Fork failed");
        kill(pid1, SIGTERM); // Terminate first child
        return 1;
    }
    
    if (pid2 == 0) {
        
        // Initialize a different random seed for child 2
        srand(time(NULL) ^ (getpid() + 1000));
        
        // Close unused pipe ends
        close(parent_to_child2[1]);
        close(child2_to_parent[0]);
        close(parent_to_child1[0]);
        close(parent_to_child1[1]);
        close(child1_to_parent[0]);
        close(child1_to_parent[1]);
        
        char buffer[BUFFER_SIZE];
        
        // Game loop for Child 2
        while (1) {
            // Wait for signal from parent
            if (safe_read(parent_to_child2[0], buffer, BUFFER_SIZE) < 0) {
                break; // Exit if pipe closed or there is a problem
            }
            
            // Check if parent sent exit command
            if (strcmp(buffer, "exit") == 0) {
                break;
            }
            
            // Make guess and send to parent
            int guess = (rand() % MAX_GUESS) + MIN_GUESS;
            sprintf(buffer, "%d", guess);
            if (safe_write(child2_to_parent[1], buffer, strlen(buffer) + 1) < 0) {
                break;
            }
        }
        
        // Close pipe ends
        close(parent_to_child2[0]);
        close(child2_to_parent[1]);
        
        return 0;
    }
    
    // Close unused pipe ends
    close(parent_to_child1[0]); // Close read end of parent to child1
    close(child1_to_parent[1]); // Close write end of child1 to parent
    close(parent_to_child2[0]); // Close read end of parent to child2
    close(child2_to_parent[1]); // Close write end of child2 to parent
    
    printf("Child 1 PID: %d\n", pid1);
    printf("Child 2 PID: %d\n", pid2);
    printf("Beginning %d Rounds...\n", rounds);
    printf("Guess!\n");
    
    int child1_score = 0;
    int child2_score = 0;
    char buffer[BUFFER_SIZE];
    
    // Play the number of rounds
    for (int round = 1; round <= rounds; round++) {
        int round_completed = 0;
        
        while (!round_completed) {
            printf("---------------------------\n");
            printf("Round %d:\n", round);
            
            // Signal both children to make a guess
            strcpy(buffer, "guess");
            if (safe_write(parent_to_child1[1], buffer, strlen(buffer) + 1) < 0 ||
                safe_write(parent_to_child2[1], buffer, strlen(buffer) + 1) < 0) {
                fprintf(stderr, "Error signaling children\n");
                goto cleanup;
            }
            
            // Get guess from Child 1
            if (safe_read(child1_to_parent[0], buffer, BUFFER_SIZE) < 0) {
                fprintf(stderr, "Error reading from Child 1\n");
                goto cleanup;
            }
            int guess1 = atoi(buffer);
            printf("Child 1 guesses %d!\n", guess1);
            
            // Get guess from Child 2
            if (safe_read(child2_to_parent[0], buffer, BUFFER_SIZE) < 0) {
                fprintf(stderr, "Error reading from Child 2\n");
                goto cleanup;
            }
            int guess2 = atoi(buffer);
            printf("Child 2 guesses %d!\n", guess2);
            
            // Generate parent's number
            int parent_number = (rand() % MAX_GUESS) + MIN_GUESS;
            printf("Parent's number: %d\n", parent_number);
            
            // Check difference
            int diff1 = abs(parent_number - guess1);
            int diff2 = abs(parent_number - guess2);
            
            // Determine winner
            if (diff1 < diff2) {
                printf("Child 1 Wins!\n");
                child1_score++;
                round_completed = 1;
            } else if (diff2 < diff1) {
                printf("Child 2 Wins!\n");
                child2_score++;
                round_completed = 1;
            } else {
                printf("Parent indicates a tie, replaying round...\n");
                // Round will be replayed
            }
        }
    }
    
    // Print final results
    printf("---------------------------\n");
    printf("---------------------------\n");
    printf("Results:\n");
    printf("Child 1: %d\n", child1_score);
    printf("Child 2: %d\n", child2_score);
    
    if (child1_score > child2_score) {
        printf("Child 1 Wins the Match!\n");
    } else if (child2_score > child1_score) {
        printf("Child 2 Wins the Match!\n");
    } else {
        printf("The Match is a Tie!\n");
    }
    
cleanup:
    // Signal children to exit
    strcpy(buffer, "exit");
    if (safe_write(parent_to_child1[1], buffer, strlen(buffer) + 1) < 0 ||
        safe_write(parent_to_child2[1], buffer, strlen(buffer) + 1) < 0) {
        fprintf(stderr, "Error signaling children to exit\n");
    }
    
    // Wait for children to exit
    if (waitpid(pid1, NULL, 0) < 0) {
        perror("Error waiting for Child 1");
    }
    
    if (waitpid(pid2, NULL, 0) < 0) {
        perror("Error waiting for Child 2");
    }
    
    // Close pipe ends
    close(parent_to_child1[1]);
    close(child1_to_parent[0]);
    close(parent_to_child2[1]);
    close(child2_to_parent[0]);
    
    return 0;
}