#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "smp.h"

uuid_t uuid;
char uuid_string[60];

mqd_t serverq;
mqd_t clientq;

int main(int argc, char *argv) {
	uuid_generate(uuid);
	uuid_unparse(uuid, uuid_string);
	printf("Starting client: %s\n", uuid_string);

	serverq = mq_open("/smp_server", O_WRONLY);
	if (serverq == (mqd_t)-1) {
		printf("failed to open server mq: %s\n", strerror(errno));
		exit(-1);
	}

	if(mq_send(serverq, "hello server!", 13, 1) == -1) {
		printf("failed to send message: %s\n", strerror(errno));
		exit(-1);
	}

	mq_close(serverq);

	return 0;
}
