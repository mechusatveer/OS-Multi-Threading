A message queue is a structure that can be shared between several processes. 
It's one of the methods that passing messages between processes. 
The messages can be placed in the queue and should be removed in the 
same order they were added.

To attach to a message queue, we need to call mq_open():

#include <mqueue.h>
mqd_t mq_open(const char *name, int oflag, ...);

This will return a handle to the message queue. It takes a minimum of the name 
of the message queue and flags. The name of the message queue must start with 
'/' up to 13 characters. The flag should be one of the following: O_RDONLY, 
O_WRONLY, O_CREAT, or O_RDWR. If the flag O_CREAT is passed, the call to 
mq_open() requires two 
additional parameters: a mode setting that is used to set the access and a
pointer to the attributes of the message.

The mq_close() takes the handle to the message queue. It closes the connection to the message queue.

#include <mqueue.h>
int mq_close(mqd_t mqdes);

But the message queue will continue to exist, and to remove it from the system,
we need to call mq_unlink() which takes the name of the message queue as its parameter.

#include <mqueue.h>
int mq_unlink(const char *name);

Messages are added to the queue using mq_send() and received from the queue via mq_receive().

#include <mqueue.h>
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);

The parameters to mq_send() are the message queue, a pointer to a buffer containg the message,
the size of the message, and a priority.

Th function mq_receive() takes the message queue, a pointer to a buffer where the message can 
be copied, the size of the buffer, and either a null pointer or a pointer to an unsigned int 
where the priority of the message will be written.

#include <mqueue.h>
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <mqueue.h>

#define PMODE 0655

int main()
{
   int status;
   struct mq_attr attr;
   attr.mq_maxmsg = 10;
   attr.mq_msgsize = 20;

   pid_t pid = fork();

   // child process - sending message
   if(pid == 0)
   {
      char message[20];
      strncpy(message, "Hello Parent!", 13);
      // write/create - attr needed because of O_CREAT
      mqd_t mqfd = mq_open("/test1", O_WRONLY|O_CREAT, PMODE, &attr);
      if(mqfd == -1)
      {
         perror("Child mq_open failure");
         exit(0);
      }
      status = mq_send(mqfd, message, strlen(message)+1, 0);
      if (status == -1)
      {
        perror("mq_send failure\n");
      }
      else {
        printf("Child is sending message: %s\n", message);
        printf("mq_send successful\n");
      }
      mq_close(mqfd);
      printf("Child process done\n");
   }

   // parent - receiving message
   else
   {
      // read only
      mqd_t mqfd = mq_open("/test1", O_RDONLY|O_CREAT, PMODE, &attr);
      if(mqfd == -1)
      {
         perror("Parent mq_open failure");
         exit(0);
      }

      // Parent is waiting for the child process to finish
      waitpid(pid, &status, 0);

      char buf[100];
      status = mq_receive(mqfd, buf, 100, 0);
      if (status == -1)
      {
        perror("mq_receive failure\n");
      }
      else
      {
        printf("mq_receive successful\n");
        printf("Parent received message: %s\n", buf);
      }

      mq_close(mqfd);
      mq_unlink("/test1");

      printf("Parent process done\n");
   }
   return 0;
}
