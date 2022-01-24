#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "zemaphore.h"
#include "msgq.h"
#include <time.h>
#include <semaphore.h>

// SEE Labs/GdbLldbLab for more information on lldb - lowlevel debugger

struct msgq *mq;
static int track = 0;//to keep track of which index producer function is at.
static int see2 = 0;//to keep track of which index consumefunction is at
pthread_mutex_t mutex1;//lock to give thread full resource without interruption
sem_t semEmpty;//to make sure no extra memory is accessed
sem_t semFull;//helper to semFull
#define size 100//size is 100
//
// Main threads
//
char *messages[] = { "msg1", "msg2", "hellomsg", "gustymsg"};//messages I am going to duplicate
char *consume_one[100];//messages that will be consumed by consumer1
char *consume_two[100];//messages that will be consumed by comsumer2
char *consume_three[100];//messages that will be consumed by consumer3

//Gusty's code Used it for testing
void *promtAndSend(void *arg) {
	mq->condition = 2;
	for (int i = 0; i < sizeof(messages)/sizeof(char*); i++) {
		char response[80];
		printf("Send? ");
		scanf("%s", response);
		if (response[0] == 'y' || response[0] == 'Y') { 
			printf("sending: %s\n", messages[i]); 
			msgq_send(mq, messages[i]);
		}
	}
	return NULL;
}
//Producer function to produce item
void *producer(void *arg){
	int count = 0;//initialize count at 0
	while(count < 51){//go till 50 since we each producer has to produce atleast 50 items
		int min = 0;//range 1 for random
		int max = 3;//range 2 for random
		int random = rand() % (max - min + 1) + min;//get the random number between the ranges 
		sem_wait(&semEmpty);//initiate semwait to see if resources are still available it will decrement by 1
		pthread_mutex_lock(&mutex1);//give the lock to the thread
		int x = msgq_send(mq, messages[random]);//start sending the message to be stored in the queue
		if(x == -1){//if x is 0
			sleep(5);//sleep for 5
		}//end of if
		printf("Producer %d: Insert %s at %d\n", (int)arg, messages[random], track);//print the result
		track++;//increment track
		count++;//increment count
		pthread_mutex_unlock(&mutex1);//release the lock
		sem_post(&semFull);//increment semfulll
	}//end of for
	sleep(3);//sleep for 3 after each thread
	return NULL;
}

void *consume(void *arg){
	int total_consume_per_thread = 34;//total items that can be consumed by each thread
	for(int i = 0; i < total_consume_per_thread; i++){//loop to add the items
		if(see2 > 100){//if see2 which is the index greater than 100 return null
			return NULL;
		}//end of if
		char *string = msgq_recv(mq);//to get the element from message recieve
		printf("Consumer %d: consumes %s at %d\n", (int)arg, string, mq->position + 1);//print the results
		sem_wait(&semFull);//decrement semfull
		pthread_mutex_lock(&mutex1);//give the lock
		if((int)arg == 3){//if consumer1
			consume_one[i] = string;//store in this array
		}//end of if
		if((int)arg == 4){//if consumer2
			consume_two[i] = string;//store in this array
		}//end of if
		if((int)arg == 5){//if consumer3
			consume_three[i] = string;//store in this array
		}//end of if
		mq->position = see2;//set the index in the struct
		see2++;//increment index
		pthread_mutex_unlock(&mutex1);//release the lock
		sem_post(&semEmpty);//increment empty
	}//end of for
	sleep(2);//sleep for 2
	return NULL;

}

// consume messges in msgq
// Gusty's code used it for testing
void *recvMsgs(void *arg) {
	//sleep(5);
	mq->condition = 2;
	printf("mq msg_count: %d\n", msgq_len(mq));
	for (int i = 0; i < msgq_len(mq); i++) {
		char *m = msgq_recv(mq);
		printf("recvMsgs: %s\n", m);
		//free(m);
	}
	return NULL;
}
//Gusty's code used it for testing
void *passiton(void *arg) {
	mq->condition = 1;
	int me = (int) arg;
	printf("This is me: %d\n", me);
	while (msgq_len(mq) > 1) {
		sleep(1);
		printf("passiton%d initial msgq_len: %d\n", me, msgq_len(mq));
		char *m = msgq_recv(mq);
		printf("passiton%d: %p %p %s\n", me, &m, m, m);
		printf("passiton%d after recv msgq_len: %d\n", me, msgq_len(mq));
		msgq_send(mq, m);
		printf("passiton%d after send msgq_len: %d\n", me, msgq_len(mq));
	}
	return NULL;
}

#define MSGQLEN 4

int main(int argc, char *argv[]) {
	pthread_t p1, p2, produce1, produce2, consumer1, consumer2, consumer3;//all the threads
	pthread_mutex_init(&mutex1, NULL);//initalize the mutex
	mq = msgq_init(MSGQLEN);//initialize the struct
	sem_init(&semEmpty, 0, 100);//initialize the sem
	sem_init(&semFull, 0, 0);//initialize the other sem
	char test = '1';//default
	srand(time(NULL));//so that not the same random numbers are generated
	if (argc == 2)
		test = argv[1][0];
	switch (test) {
		case '1':
			printf("test fill and empty msgq\n");
			printf("\n");

			printf("Time to create messages\n");
			pthread_create(&produce1, NULL, producer, (void *)1);//producer1
			pthread_create(&produce2, NULL, producer, (void *)2);//producer2
			//printf("After creating producers\n");

			//ALLL THE CONSUMERS WAITING PATIENTLY
			pthread_create(&consumer1, NULL, consume, (void *)3);//consumer1
			pthread_create(&consumer2, NULL, consume, (void *)4);//consumer2
			pthread_create(&consumer3, NULL, consume, (void *)5);//consumer3
			//
			//time to join
			pthread_join(produce1, NULL);//let producer1 finish
			pthread_join(produce2, NULL);//let producer2 finish
			pthread_join(consumer1, NULL);//let consumer1 finish
			pthread_join(consumer2, NULL);//let consumer2 finish
			pthread_join(consumer3, NULL);//let consumer3 finish
			break;
		case '2': //gusty's case used it for testing
			printf("test fill msgs and pass it on\n");
			pthread_create(&p1, NULL, promtAndSend, NULL);
			pthread_join(p1, NULL);
			printf("msgq_show() after filling for test 2:\n");
			msgq_show(mq);
			pthread_create(&p1, NULL, passiton, (void *)1);
			pthread_create(&p2, NULL, passiton, (void *)2);
			pthread_join(p1, NULL);
			pthread_join(p2, NULL);
			break;
		case '3': //gusty's case used it for testing
			printf("test fill and empty msgq\n");
			pthread_create(&p1, NULL, promtAndSend, NULL); 
		 	pthread_join(p1, NULL);
			printf("msgq_show() after filling for test 1:\n");
			msgq_show(mq);
			pthread_create(&p2, NULL, recvMsgs, NULL);
			pthread_join(p2, NULL);
			printf("msgq_show() after all consumed by test 1:\n");
			msgq_show(mq);
			break;

		default: //default case
			printf("invalid test selection!\n");
			break;

	}//end of switch-case
	pthread_mutex_destroy(&mutex1);//destroyed the mutex since no longer needed
	sem_destroy(&semEmpty);//destroyed the sem
	sem_destroy(&semFull);//destroyed the other sem
	return 0;
}//end of main
