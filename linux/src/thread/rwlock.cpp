#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_rwlock_t rwlock;
int shared_resource = 0;

void* reader_thread(void* arg) {
    while (1) {
        // Acquire read lock
        pthread_rwlock_rdlock(&rwlock);

        // Read the shared resource
        printf("Reader %ld read shared resource: %d\n", (long)arg, shared_resource);

        // Release read lock
        pthread_rwlock_unlock(&rwlock);

        // Sleep for a short interval
        usleep(500000);
    }

    return NULL;
}

void* writer_thread(void* arg) {
    while (1) {
        // Acquire write lock
        pthread_rwlock_wrlock(&rwlock);

        // Update the shared resource
        shared_resource++;
        printf("Writer %ld incremented shared resource to: %d\n", (long)arg, shared_resource);

        // Release write lock
        pthread_rwlock_unlock(&rwlock);

        // Sleep for a short interval
        usleep(1000000);
    }

    return NULL;
}

int pthread_rwlock() {
    // Initialize read-write lock
    pthread_rwlock_init(&rwlock, NULL);

    // Create reader threads
    pthread_t readers[2];
    for (long i = 0; i < 2; ++i) {
        pthread_create(&readers[i], NULL, reader_thread, (void*)i);
    }

    // Create writer thread
    pthread_t writer;
    pthread_create(&writer, NULL, writer_thread, NULL);

    // Join reader threads
    for (int i = 0; i < 2; ++i) {
        pthread_join(readers[i], NULL);
    }

    // Join writer thread
    pthread_join(writer, NULL);

    // Destroy read-write lock
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
