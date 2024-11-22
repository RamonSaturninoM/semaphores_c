/*
Errors:
    1) - both threads were trying to book tickets at the same time, causing a race condition
    2) - because of the race condition, the tickets variable was being modified simultaneously, causing lost updates and incorrect ticket counts

Fixes:
    1) - used a mutex lock to synchronize access to the tickets variable, ensuring that only one thread could modify it at a time
    2) - the mutex lock acts as a critical section, preventing race conditions and ensuring that the tickets variable is updated correctly
*/


#include <stdio.h>
#include <pthread.h>

int tickets = 10; // Shared number of available tickets
pthread_mutex_t lock;

void* book_tickets(void* arg) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&lock);    // Start of critical section - acquire lock
        if (tickets > 0) {
            int temp = tickets;
            temp--;                   // Simulate some processing time
            tickets = temp;
            printf("Booked a ticket, remaining tickets: %d\n", tickets);
        } else {
            printf("No tickets available to book.\n");
        }
        pthread_mutex_unlock(&lock);  // End of critical section - release lock
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_mutex_init(&lock, NULL);  // Initialize mutex lock

    // Create two threads for concurrent ticket booking
    pthread_create(&t1, NULL, book_tickets, NULL);
    pthread_create(&t2, NULL, book_tickets, NULL);

    // Wait for both threads to complete
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Final tickets remaining: %d\n", tickets);
    return 0;
}