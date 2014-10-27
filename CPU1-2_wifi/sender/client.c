/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//this function sends every kind of file through soket,
//in packets of 1500 bytes.
//First, it sends the size of the file in a single packet, then
//it sends the packets of the file
void send_file_eth(char* filename, int sockfd)
{
FILE* fp = fopen("test.jp2", "rb+");
if (fp==NULL) {fputs ("File error",stderr); exit (1);}
	
//obtain file size
fseek(fp, 0, SEEK_END);
long FileSize = ftell(fp);
char GotFileSize[1024];
sprintf(GotFileSize,"%d",FileSize);//itoa(FileSize, GotFileSize, 10);
send(sockfd, GotFileSize, sizeof(FileSize), 0);
rewind(fp);

long SizeCheck = 0;
char* mfcc;
	
if(FileSize > 1499){
        // allocate memory to contain the part of file:
        mfcc = (char*)malloc(1500);
        if (mfcc == NULL) {fputs ("Memory error",stderr); exit (2);}
	
	while(SizeCheck < FileSize){
		int Read = (int)fread(mfcc, sizeof(char), 1500, fp); //<-----
		//if (Read != FileSize) {fputs ("Reading error while",stderr); exit (3);}
		int Sent = send(sockfd, mfcc, Read, 0);
		SizeCheck += Sent;
		printf("Filesize: %d\nSizecheck: %d\nRead: %d\nSent: %d\n\n", FileSize, SizeCheck, Read, Sent);
		}
}
else{
	mfcc = (char*)malloc(FileSize + 1);
	if (mfcc == NULL) {fputs ("Memory error",stderr); exit (2);}
	int Read = (int)fread(mfcc, sizeof(char), FileSize, fp);
	if (Read != FileSize) {fputs ("Reading error",stderr); exit (3);}
	send(sockfd, mfcc, FileSize, 0);
}
fclose(fp);
free(mfcc);

return;
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv,i;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

        /***************************************************************
        Here i start with the typical set of functions to manage socket
        communications: (http://beej.us/guide/bgnet)
        
        getaddrinfo();
        socket();
        bind();
        
        connect(); (if I just send)
                or
        listen(); + accept(); (to send and receive)
        
        ***************************************************************/

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	
/////////////////SEEEEEEEND/////////////////////////////////////////////	

// Here you can change the number of packets sent, the name of the file
// sent, and the delay between packets sent.
// NOTE: a small sleep between send is needed! Or the buffer will not have time to flush.

        //send 10 files consequently	
	for (i=0;i<10;i++) {
	        send_file_eth("test.jp2", sockfd);
	        printf("\n\nNEXT FILE\n\n");
		//sleep(1);
	        //usleep(1000);// added to wait buffer to flush. Without this it does not work
	}
		
/////////////end/SEEEEEEEND/////////////////////////////////////////////

	return 0;
}

