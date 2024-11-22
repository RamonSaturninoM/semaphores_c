/*
The semaphore mechanism ensures proper synchronization in several ways:

1) - Initialization:
   - The semaphore is initialized with value NUM_JOBS (3), limiting concurrent access

2) - Access Control:
   - sem_wait() decrements the semaphore counter and blocks if it reaches 0
   - This ensures only NUM_JOBS threads can access the critical section at once
   - sem_post() increments the counter when a thread is done, allowing others to proceed

3) - Critical Section Protection:
   - The semaphore protects access to the shared job_count variable
   - Only one thread can modify job_count at a time, preventing race conditions
   - This maintains consistency of the job queue between users and printer

*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_JOBS 3  // Maximum number of concurrent jobs allowed
#define SEM_USR "/user_sem"

sem_t *user_sem;  // Semaphore to control access to shared job_count

int job_count = 0;  // Shared variable tracking number of print jobs

// Thread function for users submitting print jobs
void* user(void* arg) {
    for (int i = 0; i < 3; i++) {
        sem_wait(user_sem);     // Wait to get access to job_count
        job_count++;            // Add a print job
        printf("User %ld added a print job. Total jobs: %d\n", (long)arg, job_count);
        sleep(1);               // Simulate some work
        sem_post(user_sem);     // Release access to job_count
    }
    return NULL;
}

// Thread function for printer processing jobs
void* printer(void* arg) {
    for (int i = 0; i < 9; i++) { // Process 9 jobs (3 users × 3 jobs each)
        sem_wait(user_sem);     // Wait to get access to job_count
        if (job_count > 0) {
            job_count--;        // Process a job
            printf("Printer processed a job. Remaining jobs: %d\n", job_count);
        } else {
            printf("Printer waiting for jobs.\n");
        }
        sleep(0.8);
        sem_post(user_sem);     // Release access to job_count
    }
    return NULL;
}

int main() {
    // Create/open the semaphore
    user_sem = sem_open(SEM_USR, O_CREAT | O_EXCL, 0644, NUM_JOBS); 

    if (user_sem == SEM_FAILED) {
        perror("Failed to create semaphores");
        return 1;
    }
    
    pthread_t users[3], printer_thread;  // Thread handles

    // Create 3 user threads
    for (long i = 0; i < 3; i++) {
        pthread_create(&users[i], NULL, user, (void*)i);
    }
    // Create 1 printer thread
    pthread_create(&printer_thread, NULL, printer, NULL);
    
    // Wait for all user threads to complete
    for (int i = 0; i < 3; i++) {
        pthread_join(users[i], NULL);
    }
    // Wait for printer thread to complete
    pthread_join(printer_thread, NULL);

    // Cleanup semaphore
    sem_close(user_sem);
    sem_unlink(SEM_USR);

    return 0;
}