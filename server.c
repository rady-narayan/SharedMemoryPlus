#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "smp.h"

mqd_t queue_dsc;
char msg[MAX_MSG_SIZE];
struct mq_attr ma;

int main(int argc, char *argv) {
	printf("Starting server\n");
	ma.mq_flags = 0;
	ma.mq_maxmsg = MAX_MESSAGES;
	ma.mq_msgsize = MAX_MSG_SIZE;
	ma.mq_curmsgs = 0;

	queue_dsc = mq_open(SERVER_MQ_NAME, O_RDWR|O_CREAT, 0660, &ma);
	if (queue_dsc == (mqd_t)-1) {
		printf("mq open failed: %s\n", strerror(errno));
		exit(-1);
	}
	printf("mq opened %x\n", queue_dsc);

	size_t ret = mq_receive(queue_dsc, msg, MAX_MSG_SIZE, NULL);
	if (ret < 0) {
		printf("failed to receive msg: %s\n", strerror(errno));
		exit(-1);
	}
	msg[ret] = '\0';

	printf("received size: %lu msg: %s\n", ret, msg);

	mq_close(queue_dsc);
	mq_unlink(SERVER_MQ_NAME);
	return 0;
}
