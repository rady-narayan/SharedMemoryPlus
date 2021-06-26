# SharedMemory++

Implementation of shared memory functionality with added features. Shared Memory is a form of IPC - Inter process communication, where the process starting the communication creates a memory space in its address space and shares it with other cooperating processes with which it wants to communicate.
Processes can then start exchanging information by reading and writing to this shared region.

But the problem is it lacks synchronization. synchronization is needed when processes need to execute concurrently. In this project, synchronization is implemented on top of this by using Semaphores which is a locking mechanism to read and write. Basically, it is a client server model, where client was the process initiating the communication (shm_open, mmap)

Along with this, encryption using AES 256 algorithm is implemented, so the content was encrypted when it was stored in the memory.

Implementation used was POSIX, Linux has both System V and POSIX implementation of shared memory.
