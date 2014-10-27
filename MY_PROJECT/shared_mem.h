#include <sys/ipc.h>
#include <sys/shm.h> 

#define SIZE 8

/**
* Allocates a shared memory segment.
*
* @param n Size (in bytes) of chunk to allocate.
* @return Id of shared memory chunk.
*/

int AllocateSharedMemory(int key)
{
return shmget(key, SIZE, IPC_CREAT | 0666);
}

/**
* Maps a shared memory segment onto our address space.
*
* @param id Shared memory block to map.
* @return Address of mapped block.
*/

void* MapSharedMemory(int id)
{
void* addr;
addr = shmat(id, NULL, 0); /* Attach the segment... */
//shmctl(id, IPC_RMID, NULL); /* ...and mark it destroyed. */
return addr;
}

//to write in it:
// Put an empty string in the shared memory.
//sprintf(buf, "");

void WriteShMem(void *addr)
{
  sprintf(addr, "Start");
  
return;
}

//read
//just read string addr, from shmat
