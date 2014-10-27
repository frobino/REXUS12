#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define LO		"/tmp/LO"
#define SOE		"/tmp/SOE"
#define MAX_BUF_SIZE	1

#define DEBUG

//----------------------------------------------------------------------

//returns a char because the real gpio returns char to be masked
char read_gpio_soe(int *fd_soe)
{
    int soeread;
    char buf_soe;
    
    soeread = read((*fd_soe), &buf_soe, MAX_BUF_SIZE);
    
    #ifdef DEBUG
        printf("%d\n", soeread);
    #endif
    
return(buf_soe);
}

//----------------------------------------------------------------------

char read_gpio_lo(int *fd_lo)
{
    int loread;
    char buf_lo;
    
    loread = read((*fd_lo), &buf_lo, MAX_BUF_SIZE);
    
    #ifdef DEBUG
        printf("%d\n", loread);
    #endif
    
return(buf_lo);
}

//----------------------------------------------------------------------

void open_gpio(int *fd_lo,int *fd_soe)
{

    /* Open the pipe for reading */
    (*fd_lo) = open(LO, O_RDONLY);
    (*fd_soe) = open(SOE, O_RDONLY);

return;
}

//----------------------------------------------------------------------


