#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
//RS
#include <termios.h>
//time
#include <time.h>
//signals
#include <signal.h>

#define BAUDRATE1 B115200
#define BAUDRATE2 B115200

//----------------------------------------------------------------------

int max_fd(int a, int b)
{
int max;
max=a>b?a:b;
return max;

}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

int open_rs(int *fd_rs1,char * dev1){
        struct termios newtio;
	*fd_rs1 = open(dev1, O_RDWR | O_NOCTTY |O_NONBLOCK); 
        if (*fd_rs1 <0) {perror(dev1); 
		printf("Cannot open rs port %s\n",dev1);
		return -1; }

        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE1 | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;

        // set input mode (non-canonical, no echo,...) 
        newtio.c_lflag = 0;

        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

        tcflush(*fd_rs1, TCIFLUSH);
        tcsetattr(*fd_rs1,TCSANOW,&newtio);

	return 0;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

#define dev1 "/dev/ttyS1" //fifo

//void task_ch_to_flash (void)
void* fun_serial_to_flash (void *arg)
{
int ret;
char select_int=0;
char *buffer;
int length;
int fd_rs1; //file descriptor

time_t t;
fd_set rfds;

open_rs(&fd_rs1,dev1);

buffer = (char*) malloc(sizeof(char)*300+1);
t = time(NULL);

	while(1)
	{
		//setting watch points for select
		FD_ZERO(&rfds);
		FD_SET(fd_rs1, &rfds);
		
		//si blocca su quetsa linea
		ret = select(1+fd_rs1, &rfds, 0, 0, 0);//maybe pselect?
		
		select_int=0;
		
		if (ret == -1){
			if(errno==EINTR){select_int=1;
				FD_ZERO(&rfds);//cannot relay on FD_ISSET result, so reset all
				fprintf(stdout,"select intetupted, %s\n",strerror(errno));
				fflush(stdout);
				}
				else
				printf("Oops! select returned with ret == -1\n");
			}
		printf("after select\n");
		
		
		
		//RS comunication
		if (FD_ISSET(fd_rs1, &rfds)){//check what caused select to out
			if((length=read(fd_rs1,buffer,256))==-1)
				{
				fprintf(stdout,"rs1 receive failed, %s\n",strerror(errno));
				fflush(stdout);
				//break;
				}else{
				buffer[length]=0;
				printf("received from RS1: \"%s\", !!!!!!length: %d!!!!!\n",buffer,length);
				
				
				// Write... maybe useful later
				/*
				if((length=write(fd_rs2,buffer,length))==-1){
					fprintf(stdout,"rs2 transmit failed, %s\n",strerror(errno));
					}
				fprintf(stdout,"rs2 transmit, %d\n",length);
				*/
				fflush(stdout);
				
				}
			}

	tcflush(fd_rs1,TCIOFLUSH);
	}

	free(buffer);
	return;
}


