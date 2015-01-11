We can detect the potential race condition using valgrind --tool=helgrind program_name.
--------------------------------------------------------------------------------------
Suppose, two person A and B share the same account and try to deposit roughly at the same time. 
In the following example, they deposited 1$ million times. So, we expect the balance would be 
$2,000,000. However, it turns out that's not the case as we see from the output:

// global1.c
#include <stdio.h>
#include <pthread.h>

static volatile int balance = 0;

void *deposit(void *param)
{
    char *who = param;

    int i;
    printf("%s: begin\n", who);
    for (i = 0; i < 1000000; i++) {
        balance = balance + 1;
    }
    printf("%s: done\n", who);
    return NULL;
}

int main()
{
    pthread_t p1, p2;
    printf("main() starts depositing, balance = %d\n", balance);
    pthread_create(&p1, NULL, deposit, "A");
    pthread_create(&p2, NULL, deposit, "B");

    // join waits for the threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main() A and B finished, balance = %d\n", balance);
    return 0;
}


$ ./global
main() starts depositing, balance = 0
B: begin
A: begin
B: done
A: done
main() A and B finished, balance = 1270850
----
$ ./global
main() starts depositing, balance = 0
B: begin
A: begin
B: done
A: done
main() A and B finished, balance = 1423705

Note that both threads accessing the same function and share global variable balance.

Put simply, what could happened is this: the code to increment balance has been run 
twice by A and B, in both cases, however, the balance might started at 0 without 
noticing the other person was in the process of putting money into the same account.

 (1)
 // the variable balance is located at address 0x8049a1c
 // mov instruction is used first to get the memory value at the address 
 // and put it into register eax
 mov 0x8049a1c, %eax 

 (2)
 // the add is performed, adding 1 (0x1) to the contents of the eax register       
 add $0x1, %eax

 (3)
 // the contents of eax are stored back into memory at the same address.
 mov %eax, 0x8049a1c
 
 What is solution then
 =======================
 We need to make the balance updating code (critical section) as atomic:
 
 // global2.c
#include <stdio.h>
#include <pthread.h>

static volatile int balance = 0;
pthread_mutex_t myMutex;

void *deposit(void *param)
{
    char *who = param;

    int i;
    printf("%s: begin\n", who);
    for (i = 0; i < 1000000; i++) {

        // critical section
        pthread_mutex_lock(&myMutex);
        balance = balance + 1;
        pthread_mutex_unlock(&myMutex);

    }
    printf("%s: done\n", who);
    return NULL;
}

int main()
{
    pthread_t p1, p2;
    printf("main() starts depositing, balance = %d\n", balance);
    pthread_mutex_init(&myMutex,0);
    pthread_create(&p1, NULL, deposit, "A");
    pthread_create(&p2, NULL, deposit, "B");

    // join waits for the threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_mutex_destroy(&myMutex);
    printf("main() A and B finished, balance = %d\n", balance);
    return 0;
}



Technically, the deposit process may not be just one instruction, it may have three-instruction sequences as below:
