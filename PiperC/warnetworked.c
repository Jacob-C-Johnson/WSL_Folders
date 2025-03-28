#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

// Struct to pass in the socket information
typedef struct {
    int thread_socket;
} thread_data_t;

// Card structure
typedef struct {
    int rank;   // 2-14 (2-10, Jack=11, Queen=12, King=13, Ace=14)
    char suit;  // 'S', 'H', 'D', 'C' (Spades, Hearts, Diamonds, Clubs)
} Card;

// Function to draw a random card
Card draw_card() {
    Card card;
    card.rank = (rand() % 13) + 2; // Generate rank between 2 and 14
    int suit_index = rand() % 4;   // Randomize suit index

    // Map suit_index to suit character
    switch (suit_index) {
        case 0: card.suit = 'S'; break; // Spades
        case 1: card.suit = 'H'; break; // Hearts
        case 2: card.suit = 'D'; break; // Diamonds
        case 3: card.suit = 'C'; break; // Clubs
    }

    return card;
}

// Suit precedence for tie resolution
int suit_precedence(char suit) {
    switch (suit) {
        case 'S': return 4; // Spades
        case 'H': return 3; // Hearts
        case 'D': return 2; // Diamonds
        case 'C': return 1; // Clubs
    }
    return 0;
}

// Thread function for child threads
void *child_thread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    int socket_fd = data->thread_socket;
    while (1) {
        char buffer[256];
        read(socket_fd, buffer, sizeof(buffer));
        if (strcmp(buffer, "QUIT") == 0) {
            break; // Exit the thread
        } else if (strcmp(buffer, "DRAW") == 0) {
            Card card = draw_card();
            write(socket_fd, &card, sizeof(Card)); // Send card to parent
        }
    }
    return NULL;
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_rounds>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int rounds = atoi(argv[1]);
    if (rounds <= 0) {
        fprintf(stderr, "Number of rounds must be greater than 0.");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    // Create sockets for parent-child communication
    int sockets[2][2]; // One socket pair per child
    for (int i = 0; i < 2; i++) {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets[i]) < 0) {
            perror("socketpair");
            exit(EXIT_FAILURE);
        }
    }

    // Create child threads
    pthread_t threads[2];
    thread_data_t thread_data[2];
    for (int i = 0; i < 2; i++) {
        thread_data[i].thread_socket = sockets[i][1]; // Child side
        if (pthread_create(&threads[i], NULL, child_thread, &thread_data[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    printf("Beginning %d Rounds…", rounds);

    int wins[2] = {0, 0}; // Track wins for each child

    for (int round = 1; round <= rounds; round++) {
        printf("Round %d:", round);

        // Signal both children to draw cards
        for (int i = 0; i < 2; i++) {
            write(sockets[i][0], "DRAW", strlen("DRAW") + 1);
        }

        // Receive cards from both children
        Card cards[2];
        for (int i = 0; i < 2; i++) {
            read(sockets[i][0], &cards[i], sizeof(Card));
        }

        // Print the draws (ranks only)
        printf("Child 1 draws %d", cards[0].rank);
        printf("Child 2 draws %d", cards[1].rank);

        // Determine the winner
        if (cards[0].rank > cards[1].rank) {
            printf("Child 1 Wins!");
            wins[0]++;
        } else if (cards[1].rank > cards[0].rank) {
            printf("Child 2 Wins!");
            wins[1]++;
        } else { // Handle tie (show suits)
            printf("Tie! Checking suits…");
            printf("Child 1 has suit %c", cards[0].suit);
            printf("Child 2 has suit %c", cards[1].suit);
            if (suit_precedence(cards[0].suit) > suit_precedence(cards[1].suit)) {
                printf("Child 1 Wins!");
                wins[0]++;
            } else {
                printf("Child 2 Wins!");
                wins[1]++;
            }
        }
        printf("---------------------------");
    }

    // Check for sudden death if tied
    if (wins[0] == wins[1]) {
        printf("Sudden Death Round!");

        // Signal both children to draw cards
        for (int i = 0; i < 2; i++) {
            write(sockets[i][0], "DRAW", strlen("DRAW") + 1);
        }

        // Receive cards from both children
        Card cards[2];
        for (int i = 0; i < 2; i++) {
            read(sockets[i][0], &cards[i], sizeof(Card));
        }

        // Print the sudden death results (ranks only unless tied)
        printf("Child 1 draws %d", cards[0].rank);
        printf("Child 2 draws %d", cards[1].rank);
        if (cards[0].rank > cards[1].rank || 
            (cards[0].rank == cards[1].rank && suit_precedence(cards[0].suit) > suit_precedence(cards[1].suit))) {
            wins[0]++;
            printf("Child 1 Wins the Tournament!");
        } else {
            wins[1]++;
            printf("Child 2 Wins the Tournament!");
        }
    } else {
        printf("Results: Child 1: %d Child 2: %d", wins[0], wins[1]);
        if (wins[0] > wins[1]) {
            printf("Child 1 Wins the Tournament!");
        } else {
            printf("Child 2 Wins the Tournament!");
        }
    }

    // Signal children to quit
    for (int i = 0; i < 2; i++) {
        write(sockets[i][0], "QUIT", strlen("QUIT") + 1);
    }

    // Wait for threads to exit
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
