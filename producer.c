#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "smpp.h"
#include <time.h>

#define BUFFER_SIZE 8

void initialize_buffer(int fd, int size) {
	struct circular_buffer *buffer = (struct circular_buffer *) smpp_read(fd, size, PROT_READ | PROT_WRITE);
	buffer->size = BUFFER_SIZE;
	buffer->read = 0;
	buffer->write = 0;
	buffer->full = 0;
	buffer->empty = BUFFER_SIZE;
	smpp_write(buffer, size);
}

int main() {
	struct circular_buffer *buffer;
	char input[256];
	int fd;
	int size = get_buffer_size(BUFFER_SIZE);
	srand(time(0));

	smpp_enable_enc(size);
	fd = smpp_open(O_CREAT | O_RDWR);

	smpp_set_size(fd, size);
	initialize_buffer(fd, size);

	while(1) {
		// read from stdin
		printf("Producer: enter input: ");
		if (fgets(input, 256, stdin) == NULL) {
			printf("Producer: EXIT\n");
			smpp_close(fd);
			return EXIT_SUCCESS;
		}

		// one char + new line
		if (strlen(input) != 2) {
			printf("Producer: invalid input\n");
			continue;
		}

		if (input[0] == '!') {
			smpp_close(fd);
			return EXIT_SUCCESS;
		}

		if (input[0] == 'p') {
			input[0] = rand() % 26 + 65;
			printf("Producer: random char generated: %c\n", input[0]);
		} else {
			printf("Producer: invalid input\n");
			continue;
		}

		buffer = (struct circular_buffer *) smpp_read(fd, size, PROT_READ | PROT_WRITE);		

		if (buffer->empty == 0) {
			printf("Producer: buffer is full\n");
			goto unmap;
		}

		buffer->data[buffer->write++] = input[0];

		if(buffer->write >= buffer->size)
			buffer->write = 0;

		buffer->full++;
		buffer->empty--;

		print_contents(buffer);

		printf("\nProducer: done\n");
unmap:
		smpp_write(buffer, size);
	}

	smpp_close(fd);

	return EXIT_SUCCESS;
}
