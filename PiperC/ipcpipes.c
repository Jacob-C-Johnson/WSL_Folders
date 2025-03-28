#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>

#define MAX_GUESS 10
#define MIN_GUESS 1

typedef struct {
    int socket;
    int id;
} ThreadData;

void *child_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int signal, guess;

    while (1) {
        // Wait for a signal from the parent
        read(data->socket, &signal, sizeof(int));
        if (signal == -1) break; // Exit signal received, terminate the thread

        // Generate a random guess and send it back to the parent
        guess = (rand() % MAX_GUESS) + MIN_GUESS;
        write(data->socket, &guess, sizeof(int));
    }

    close(data->socket);
    free(data);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_rounds>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int rounds = atoi(argv[1]);
    if (rounds <= 0) {
        fprintf(stderr, "Invalid number of rounds.\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    // Create socket pairs for parent-child communication
    int sockets[2][2];
    for (int i = 0; i < 2; i++) {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets[i]) < 0) {
            perror("socketpair");
            exit(EXIT_FAILURE);
        }
    }

    // Create child threads
    pthread_t t1, t2;
    ThreadData *data1 = malloc(sizeof(ThreadData));
    ThreadData *data2 = malloc(sizeof(ThreadData));
    data1->socket = sockets[0][1]; // Child 1 side
    data1->id = 1;
    data2->socket = sockets[1][1]; // Child 2 side
    data2->id = 2;

    pthread_create(&t1, NULL, child_thread, data1);
    pthread_create(&t2, NULL, child_thread, data2);

    int score1 = 0, score2 = 0;
    int guess1, guess2, target;
    int signal = 0;

    printf("The Guessing Game\n");
    printf("Beginning %d Rounds...\n", rounds);
    printf("Child 1 (ID: %lu) is ready.\n", t1);
    printf("Child 2 (ID: %lu) is ready.\n", t2);

    for (int round = 1; round <= rounds; round++) {
        printf("Round %d:\n", round);
        target = (rand() % MAX_GUESS) + MIN_GUESS;
    
        // Signal both children to make a guess
        write(sockets[0][0], &signal, sizeof(int));
        write(sockets[1][0], &signal, sizeof(int));
    
        // Read guesses from both children
        read(sockets[0][0], &guess1, sizeof(int));
        read(sockets[1][0], &guess2, sizeof(int));
    
        printf("Child 1 guesses %d!\n", guess1);
        printf("Child 2 guesses %d!\n", guess2);
        printf("Parent’s number: %d\n", target);
    
        // Determine the winner
        if (abs(target - guess1) < abs(target - guess2)) {
            printf("Child 1 Wins!\n");
            score1++;
        } else if (abs(target - guess2) < abs(target - guess1)) {
            printf("Child 2 Wins!\n");
            score2++;
        } else {
            // If it's a tie, move to the next round without replaying
            printf("It's a tie! Moving to the next round...\n");
        }
    }

    // Signal children to exit
    signal = -1;
    write(sockets[0][0], &signal, sizeof(int));
    write(sockets[1][0], &signal, sizeof(int));

    // Wait for child threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nResults:\nChild 1: %d\nChild 2: %d\n", score1, score2);
    if (score1 > score2) printf("Child 1 Wins the Match!\n");
    else if (score2 > score1) printf("Child 2 Wins the Match!\n");
    else printf("It's a Tie!\n");

    return 0;
}