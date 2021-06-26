#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <unistd.h>
#include "smpp.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int get_array_size(int size) {
	return sizeof(int) * size;
}

int get_buffer_size(int size) {
	return sizeof(int) * 5 + sizeof(char) * size;
}

void print_contents(struct circular_buffer *buffer)
{
		int count = 0;
		printf("Contents of the buffer: \n");
		for (int i = buffer->read; ; i++, count++) {
			if (i >= buffer->size)
				i = 0;

			if (count == buffer->full)
				break;

			printf("%c ", buffer->data[i]);
		}

}

/*
 * open_semaphore: create a semaphore in unlocked state (1) 
 * param: none
 * returns: semaphore id
 */
 int open_semaphore() {	
	sem_id = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 1);

	if(sem_id == (void*)-1){
		perror("sem_open failure");
		return EXIT_FAILURE;
	}
}

void close_semaphore() {
	sem_close(sem_id); 
}

/*
 * smpp_open: opens a shared memory region (creates one if it doesn't exist)
 * param: flags
 * returns: fd
 */
int smpp_open(int flags) {
	int fd;

	if (flags == 0)
		flags = O_RDONLY;

	fd = shm_open(SMP_NAME, flags, 0600);
	if (fd < 0) {
		perror("smpp_open: failed to open shared memory region");
		return EXIT_FAILURE;
	}

	open_semaphore();

	return fd;
}

/*
 * smpp_close: close the shared memory region and release semaphore
 * param: fd
 * returns: none
 */
void smpp_close(int fd) {
	close(fd);
	close_semaphore();
}

static int orig_size, enc_size;
static void *mmap_data;
static void *data;
static int enc = 0;

static unsigned char *key = (unsigned char *)"b6b16c1eff6efe9879f564f90787323c";
static unsigned char *iv = (unsigned char *)"c7d59e7d2980643f";

void smpp_enable_enc(off_t size) {
	enc = 1;
	orig_size = size;
	enc_size = (orig_size/EVP_MAX_IV_LENGTH + 1) * EVP_MAX_IV_LENGTH;
	data = malloc(orig_size);
	if (!data) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
}

/*
 * smpp_set_size: truncates shared memory region to SIZE specified in header file
 * param: fd, size
 * returns: success/failure
 */
int smpp_set_size(int fd, off_t size) {
	orig_size = size;

	// cipher block size calculation according to openssl docs
	enc_size = (orig_size/EVP_MAX_IV_LENGTH + 1) * EVP_MAX_IV_LENGTH;

	if (enc)
		size = enc_size;

	if (ftruncate(fd, size) != 0) {
		perror("ftruncate failed");
		return EXIT_FAILURE;
	}

	if (enc) {
		mmap_data = mmap(0, enc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		encrypt(data, orig_size, key, iv, mmap_data);

		//printf("Ciphertext is:\n");
		//BIO_dump_fp (stdout, (const char *)mmap_data, enc_size);

		munmap(mmap_data, enc_size);
	}

	return 0;
}

/*
 * smpp_read: maps shared memory to address space and also gets a semaphore lock
 * param: fd, prot : access to data being mapped (read : PROT_READ/ write : PROT_READ | PROT_WRITE)
 * returns: pointer to shared memory
 */
void *smpp_read(int fd, int SIZE, int prot) {
	mmap_data = mmap(0, enc ? enc_size : orig_size, prot, MAP_SHARED, fd, 0);

	// when we return here, we hold lock to the shared memory
	if(!sem_wait(sem_id)) {
		if (enc) {
			int ret = decrypt(mmap_data, enc_size, key, iv, data);
			if (ret != orig_size) {
				perror("decrypt failed");
				exit(EXIT_FAILURE);
			}
			return data;
		}
		return mmap_data;
	}
}

/*
 * smpp_write: removes mapping to shared memory and releases lock
 * param: *data, pointer to shared memory
 * returns: none
 */
void smpp_write(void *pdata, int SIZE) {
	if (enc)
		encrypt(data, orig_size, key, iv, mmap_data);
	munmap(mmap_data, enc ? enc_size : orig_size);

	// release lock on the shared memory region
	sem_post(sem_id);
}

