LIBS = -lrt -lpthread -lcrypto

all:
	gcc -o p1 p1.c smpp.c Evp-symmetric-encrypt.c ${LIBS}
	gcc -o p2 p2.c smpp.c Evp-symmetric-encrypt.c ${LIBS}
	gcc -o producer producer.c smpp.c Evp-symmetric-encrypt.c ${LIBS}
	gcc -o consumer consumer.c smpp.c Evp-symmetric-encrypt.c ${LIBS}
	rm -f /dev/shm/sem.sem_object /dev/shm/smp_object
