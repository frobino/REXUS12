#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

#define MAXDATASIZE 100 // max number of bytes we can get at once 

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//this function receive every kind of file through soket,
//in packets of 1500 bytes.
//First, it receive the size of the file in a single packet, then
//it receive the packets of the file
void receivefile(char* receivedfile, int new_fd)
{
        char GotFileSize[1024];
	long fileSize;
        int read_rec = recv(new_fd, GotFileSize, 1024, 0);
	printf("received at file size %d",read_rec);
        long FileSize = atoi(GotFileSize);//sprintf(FileSize,"%s",GotFileSize)
        long SizeCheck = 0;

        printf("Filesize: %d\n", FileSize);
        FILE *fp = fopen(receivedfile, "wb+");
        char* mfcc;
	        if(FileSize > 1499){
		        
		  
			long still_to_be_sent;
		        while(SizeCheck < FileSize)
			{
			        still_to_be_sent = (FileSize - SizeCheck); 
			        if (still_to_be_sent > 1499)
				{
				mfcc = (char*)malloc(1500);
			        int Received = recv(new_fd, mfcc, 1500, 0);
			        SizeCheck += Received;
			        fwrite(mfcc, 1, Received, fp);
			        fflush(fp);
			        printf("Filesize: %d\nSizecheck: %d\nReceived: %d\n\n", FileSize, SizeCheck, Received);
				}
				else
				{
				mfcc = (char*)malloc(still_to_be_sent+1);
			        int Received = recv(new_fd, mfcc, still_to_be_sent, 0);
			        SizeCheck += Received;
			        fwrite(mfcc, 1, Received, fp);
			        fflush(fp);
			        printf("Filesize: %d\nSizecheck: %d\nReceived: %d\n\n", FileSize, SizeCheck, Received);
				}
				
			free(mfcc);
		        }
		        
		        
	        }
	        else{
		        mfcc = (char*)malloc(FileSize + 1);
		        int Received = recv(new_fd, mfcc, FileSize, 0);
		        fwrite(mfcc, 1, Received, fp);
		        fflush(fp);
	        }

	fclose(fp);
	//free(mfcc);
}


void* fun_tcp_to_flash (void* arg)
{
	int sockfd, new_fd, numbytes;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	char buf[MAXDATASIZE];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

        int i=0;
        char recv_filename[16];

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

//NOTE: fork not needed, because we got 1 connection from 1 client, no more.

		//if (!fork()) { // this is the child process
		//	close(sockfd); // child doesn't need the listener
		
//////////////////RECEIVE///////////////////////////////////////////////

// Specify inside of here how many packets to receive.
// In the final version this will be a while (1) with i++

                        int i=0;
                        char recv_filename[16];
                        for(i=0;i<10;i++) {
                                sprintf(recv_filename,"received%d.jp2",i);
                                receivefile(recv_filename, new_fd);
                               
                        }
                       
////////////////////////////////////////////////////////////////////////			
			
			//exit(0); //related to fork
		//}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

