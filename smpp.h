/*
 * Author: Radhika Narayana and Sushma Sakaleshpur Ravishankar
 * OS Project: Shared Memory ++
 *
 */

#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <semaphore.h>

#define SMP_NAME "/smp_object"

//for p1/p2
int get_array_size(int);

//for producer/consumer
int get_buffer_size(int);

struct circular_buffer {
    int size;
    int full;
    int empty;
    int read;
    int write;
    char data[];
};

void print_contents(struct circular_buffer *);

#define SEMAPHORE_NAME "/sem_object"
#define SEMAPHORE_NAME_READERS "/sem_object_readers"

// functions in smpp.c
int open_semaphore();
void close_semaphore();
int smpp_open(int flags);
void smpp_close(int fd);
int smpp_set_size(int fd, off_t size);
void *smpp_read(int fd, int size, int prot);
void smpp_write(void *data, int size);
void smpp_enable_enc(off_t);

// critical section semaphore
sem_t *sem_id;

/* encryption functions */
void handleErrors(void);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);
