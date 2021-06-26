#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "smpp.h"

#define BUFFER_SIZE 8

int main() {
	struct circular_buffer *buffer;
	char input[256];
	int fd;
	int size = get_buffer_size(BUFFER_SIZE);

	smpp_enable_enc(size);
	fd = smpp_open(O_CREAT | O_RDWR);

	while(1) {
		// read from stdin
		printf("Consumer: enter 'c' to consume: ");
		if (fgets(input, 256, stdin) == NULL) {
			printf("Consumer: EXIT\n");
			smpp_close(fd);
			return EXIT_SUCCESS;
		}

		// one char + new line
		if (strlen(input) != 2) {
			printf("Consumer: invalid input\n");
			continue;
		}

		if (input[0] == '!') {
			smpp_close(fd);
			return EXIT_SUCCESS;
		}

		if (input[0] != 'c') {
			printf("Consumer: invalid input\n");
			continue;
		}

		buffer = (struct circular_buffer *) smpp_read(fd, size, PROT_READ | PROT_WRITE);
		if (buffer->empty == buffer->size) {
			printf("Consumer: buffer is empty\n");
			goto unmap;
		}

		print_contents(buffer);

		printf("\nConsumer: read (%c) from buffer\n", buffer->data[buffer->read++]);

		if (buffer->read >= buffer->size)
			buffer->read = 0;

		buffer->full--;
		buffer->empty++;
		printf("Consumer: done\n");
unmap:
		smpp_write(buffer, size);

	}

	smpp_close(fd);

	return EXIT_SUCCESS;
}


