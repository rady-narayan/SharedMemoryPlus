#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <unistd.h>
#include "smpp.h"

#define ARRAY_SIZE 3

int main() {
	int *data;
	int size = get_array_size(ARRAY_SIZE);
	int fd = smpp_open(O_CREAT | O_RDWR);

	smpp_set_size(fd, get_array_size(ARRAY_SIZE));

	for (int j = 0; j < 10 ; ++j) {
		sleep(1);
		data = (int *) smpp_read(fd, size, PROT_READ | PROT_WRITE);
		printf("P2 mapped address: %p\n", data);
		printf("P2 enter critical region\n");

		printf("before: ");
		for (int i = 0; i < ARRAY_SIZE; ++i) 
			printf("%d ", data[i]);
		printf("\n");

		// update region
		for (int i = 0; i < ARRAY_SIZE; ++i) 
			data[i] += 1;

		printf("after: ");
		for (int i = 0; i < ARRAY_SIZE; ++i) 
			printf("%d ", data[i]);
		printf("\n");

		printf("P2 exit\n");

		smpp_write(data, size);
	}

	smpp_close(fd);

	return EXIT_SUCCESS;
}
