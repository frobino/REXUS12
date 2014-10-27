#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h> //signals between processes (kill)


#include "gpio.h"
#include "shared_mem.h"

#define DEBUG

#define KEY_MEM 1037 //must be the same as rsrouter2.c

#define CPU2_STATE_IDLE 1
#define CPU2_STATE_TEST_MODE 2
#define CPU2_STATE_WAIT_SOE 3
#define CPU2_STATE_START_SYS 4

//-----EXT DEFINED FUNCTIONS--------------------------------------------

void open_gpio(int *fd_lo,int *fd_soe);
char read_gpio_soe(int *fd_soe);
char read_gpio_lo(int *fd_lo);

//----------------------------------------------------------------------

//-----INT DEFINED FUNCTIONS--------------------------------------------

void state_machine(char buf_lo,char buf_soe,void *addr)
{
    if ((buf_lo == '1') && (buf_soe == '1'))
         {
            //write "start" in shared memory
            WriteShMem(addr);
	    kill(getpid(), SIGTERM); //...or pause();
	 }
    else if ((buf_lo == '0') && (buf_soe == '1'))
         {
            #ifdef DEBUG  
            printf("Test\n");
            #endif
         }
    
return;
}

//----------------------------------------------------------------------


int main(int argc, char *argv[])
{
    int fd_lo, fd_soe;
    char buf_lo,buf_soe,gpio;
    
    int sh_mem_id;
    void *sh_mem; //pointer to shared memory. Used to WR and RD 

    //int current_state; - Not necessary now

    /* Open the pipe for reading */
    open_gpio(&fd_lo,&fd_soe);
    
    
    //current_state = CPU2_STATE_IDLE; - Not necessary now
    
    sh_mem_id = AllocateSharedMemory(KEY_MEM);
    printf("%d",sh_mem_id);
    sh_mem = MapSharedMemory(sh_mem_id);
    
    while (1)
    {
        /* Read from the pipe LO*/
        buf_lo = read_gpio_soe(&fd_lo);
        buf_soe = read_gpio_soe(&fd_soe);
  
        #ifdef DEBUG  
            printf("LO : Read From the pipe : %c\n", buf_lo);
            printf("SOE : Read From the pipe : %c\n", buf_soe);
        #endif
        
        state_machine(buf_lo,buf_soe,sh_mem);
    
    sleep(10);
    }
        
return (0);
} 
