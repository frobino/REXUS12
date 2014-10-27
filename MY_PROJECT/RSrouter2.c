#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include "Serial_chad.h"
#include "tcp_chad.h"
#include <string.h> //strcmp
#include <pthread.h> //manage threads

#define SHMSZ     8
#define KEY_MEM 1037 //must be the same as state_machine.c

main()
{
    int shmid;
    key_t key;
    char *shm, *s;
    
    //threads variables
    pthread_t th_serial_to_flash, th_tcp_to_flash;
    pthread_attr_t *tattr=0;

    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = KEY_MEM;

    /*
     * Locate the segment and wait until something appears
     * Later I can put additional check (ex: start only on Start...)
     */
    while ((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {sleep(1);}
    
    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }


    while(1) 
    {
    /*
     * Now read what the server put in the memory.
     */
    printf("read from shared mem 1) %s",shm);
    
    if (!strcmp(shm,"Start")) //(strcmp(shm,"Start\n"))
        {
        
        printf("stcmp res %d",strcmp(shm,"Start\n"));
        
        //task_ch_to_flash();
        pthread_create(&th_serial_to_flash, tattr, fun_serial_to_flash, 0);
        pthread_create(&th_tcp_to_flash, tattr, fun_tcp_to_flash, 0);
        
        pthread_join(th_serial_to_flash,0);
        pthread_join(th_tcp_to_flash,0);
        pthread_exit(0);
    
        exit(0);
        
        }
    else
        printf("No Start received: %s\n",shm);
        
    /*pthread_join(th_serial_to_flash,0);
    pthread_join(th_tcp_to_flash,0);
    pthread_exit(0);
    
    exit(0);*/
    }
} 
