//All the struct values I have used throughout the project
struct msgq{
	int num_messages;
	char *messages[101]; //to store messages in the heap
	int position;
	int count;
	int condition;
} msgq;

struct msgq *msgq_init(int num_msgs);
int msgq_send(struct msgq *mq, char *msg);
char *msgq_recv(struct msgq *mq);
int msgq_len(struct msgq *mq);
void msgq_show(struct msgq *mq);
