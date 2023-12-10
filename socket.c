#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <unistd.h> 
#define BUFFER_SIZE 50
#define NUMITEMS 30

char message[BUFFER_SIZE] = "Child Thread";
int message_count = 0;

pthread_mutex_t insert_mutex;
pthread_mutex_t client_mutex;
pthread_mutex_t server_mutex;
pthread_cond_t client_broad;
pthread_cond_t server_sig;

receive_broadcast(int arg) {
    pthread_cond_wait(& client_broad, & client_mutex);

    printf("Thread %d Received : %s\n", arg, message);
}

void* server(void * arg) {
    while (1) {
        pthread_mutex_lock(& server_mutex);
        pthread_cond_wait(& server_sig, & server_mutex);
        pthread_cond_broadcast(& client_broad);

        message_count = 0;
        pthread_mutex_unlock(& server_mutex);
    }
}

void* client(void * arg) {
    int status;
    while (1) {
        if (message_count == 0) {
            message_count = 1;
            sleep(1);
            printf("Thread %d: \n", (int)arg);
            
            pthread_cond_signal(& server_sig);
        } else 
            receive_broadcast(arg);
        
        pthread_mutex_unlock(& client_mutex);
    }
}

int main(int argc, char *argv[]) {
    int status, cycle;
    void *result;

    pthread_t server_tid, client_tid[8];
    pthread_mutex_init(&client_mutex, NULL);
    pthread_mutex_init(&server_mutex, NULL);
    pthread_cond_init(&server_sig, NULL);
    pthread_cond_init(&client_broad, NULL);

    cycle = 5;
    for(int i = 0; i < cycle; i++) {
        status = pthread_create(&client_tid, NULL, client, i + 1);
        if(status != 0) perror("[ Create ] Client Thread");
    }

    status = pthread_create(&server_tid, NULL, server, NULL);
    if(status != 0) perror("[ Create ] Server Thread");

    status = pthread_join(server_tid, NULL);
    if(status != 0) perror("[ Join ] Server Thread");

    for(int i = 0; i < cycle; i++) {
        status = pthread_join(client_tid, NULL);
        if(status != 0) perror("[ Join ] Client Thread");
    }

    return 0;
}
