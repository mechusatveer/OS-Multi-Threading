MUTEX AND CONDITION VARIABLE
Write a multi threaded C code with one thread printing all even numbers and 
the other all odd numbers. The output should always be in sequence
ie. 0,1,2,3,4....etc.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int MAX = 10;
int count = 0;

void *even(void *arg)
{
    printf("This is even thread()\n");
    while(count < MAX)
        if(count % 2 == 0)
            printf("%d ", count++);
    pthread_exit(0);
}

void *odd(void *arg)
{
    printf("This is odd thread()\n");
    while(count < MAX)
        if(count % 2 == 1)
            printf("%d ", count++);
    pthread_exit(0);
}

int main()
{
    pthread_t t1;
    pthread_t t2;

    pthread_create(&t1, 0, &even, NULL);
    pthread_create(&t2, 0, &odd, NULL);

    pthread_join(t1, 0);
    pthread_join(t2, 0);

    return  0;
}

This solution will not work output be incosistant no ordering....

//do not signal condition variable from main
//count is 0 so even thread will execute and signal conditional variable.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int MAX = 10;
int count = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;

void *even(void *arg)
{
    while(count < MAX)
    {
        pthread_mutex_lock(&mutex);
        while(count % 2 != 0)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        printf("%d ", count++);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }
    pthread_exit(0);
}

void *odd(void *arg)
{
    while(count < MAX)
    {
        pthread_mutex_lock(&mutex);
        while(count % 2 != 1)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        printf("%d ", count++);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t t1;
    pthread_t t2;

    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&cond, 0);

    pthread_create(&t1, 0, &even, NULL);
    pthread_create(&t2, 0, &odd, NULL);

    pthread_join(t1, 0);
    pthread_join(t2, 0);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return  0;
}
