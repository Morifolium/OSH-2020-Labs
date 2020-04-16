#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int ready = 0;
int data;
#define LENs 4

void *worker(void *p)
{
    pthread_mutex_lock(&mutex);
    while (ready == 0)
    {
        pthread_cond_wait(&cv, &mutex);
    }
    ready = 0;
    sleep(1);
    printf("%d\n", data);
    ready = 1;
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main()
{
    pthread_t thread[LENs];
    for (int i = 0; i < LENs; i++)
    {
        pthread_create(&thread[i], NULL, worker, NULL);
        //pthread_detach(thread);
    }
    pthread_mutex_lock(&mutex);
    sleep(1);
    ready = 1;
    data = 1234;
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mutex);
    for (int i = 0; i < LENs; i++)
        pthread_join(thread[i], NULL);
    return 0;
}