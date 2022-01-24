#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "msgq.h"
#include <string.h>
#include <semaphore.h> 


static int boolean = 1;
// initializes a message queue and returns a pointer to a struct msgq
struct msgq *msgq_init(int num_msgs){
	// malloc msgq struct
	struct msgq *mq = malloc(sizeof(msgq));
	// store parameter num into mq->number of messages
	mq->num_messages = num_msgs;
	return mq;	
}

// places msg on the message queue mq. mq is returned from msgq_init
int msgq_send(struct msgq *mq, char *msg){
	if(mq->condition == 2){//if condition is the do this
		char response[80];//initialize array
		printf("Do you want to add another element. If so type the name otherwise type 'n' ");//ask user a question
		scanf("%s", response);//store the data
		if(strcmp(response,"n") != 0 && strcmp(response, "N") != 0){//if the response is anything but n
			printf("Array is full\n");//print these
			printf("Blocked\n");
			exit(1);//exit the program
		}//end of inner if
	}//end of outer if

	if(boolean == 1){//if boolean is 1
		if(mq->count > 100){//check if the count is greater than 100
			boolean = 0;//if so change boolean value
			printf("Blocked\n");//print blocked
			return -1;//and return
		}//end of if
		mq->messages[mq->count] = msg;//else store the message in the queue
		mq->count++;//increment count
	}//end of if
	return 1;

}
//this function is use to delete the element and shift elements
void free_element(struct msgq *mq, int size, int position){
	for(int i = position; i < size - 1; i++){
		mq->messages[i] = mq->messages[i+1];
	}
	mq->num_messages--;
		
}

// returns a message from mq. 
char *msgq_recv(struct msgq *mq){
	// get a message from the queue
	char *string = mq->messages[mq->position];;
	if(mq->condition ==1){//if the condition is 1
		 string = mq->messages[mq->position];//do this
		 free_element(mq, mq->num_messages, mq->position);//delete the element

	}else if(mq->condition == 2){//if the condition is 2
		string = mq->messages[mq->position];
		mq->position++;//increment size
	}//end of else if

	return string;
}

// returns number of messages on mq
int msgq_len(struct msgq *mq){
	return mq->num_messages;
}
// displays all of the messages in mq to stdout
void msgq_show(struct msgq *mq){
	printf("I am going to display all the messages\n");
	for(int i = 0; i < msgq_len(mq); i++){
		printf("%d %s\n", i, mq->messages[i]);
	}

}
