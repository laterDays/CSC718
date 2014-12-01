#include <iostream>
#include <queue>

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int host_port = 8080;

void usage()
{
	printf("myhttpd, a simple webserver\n");
	printf("ver 1.0, 2014\n");
	printf("Usage Summary: myhttpd -h -p portno \n");
	printf("	-h: display the summary\n");
	printf("	-p: change default port number for example: -p 8080\n\n");
}

void *httpHandler(void* socketfd)
{
	char buffer[1024];
	int buffer_len = 1024;
	int bytecount;
	int socket = *((int*)socketfd);

	printf("[%d] Begin.\n", socket);
	memset(buffer, 0, buffer_len);
	if ((bytecount = recv(socket, buffer, buffer_len, 0))== -1)
	{
		fprintf(stderr, "[%d] Error receiving data %d\n", socket, errno);
		return 0;
	}

	//printf("[%d] Received bytes %d\nReceived string \"%s\"\n", socket, bytecount, buffer);

	strcpy(buffer, "HTTP/1.1 200 OK\nServer: demo\nContent-Length: 37\nConnection: close\nContent-Type: html\n\n<html><body>Welcome to my first page!</body></html>");

	if ((bytecount = send(socket, buffer, strlen(buffer), 0))== -1)
	{
		fprintf(stderr, "[%d] Error sending data %d\n", socket, errno);
		return 0;
	}

	printf("[%d] Sent bytes %d\n", socket, bytecount);

	close(socket);
	sleep(1);	// Added a sleep just to make sure they are running
				// concurrently.
	printf("[%d] End.\n", socket);
	return 0;
}




int main(int argc, char *argv[])
{
	struct sockaddr_in my_addr;

	int hsock;
	int * p_int ;
	int err;
	int socketfd;

	socklen_t addr_size = 0;
	int* csock;
	sockaddr_in sadr;
	pthread_t thread_id=0;

	int opt = 0;
	opt = getopt( argc, argv,"dhl:p:r:t:n:s:" );	// I still dont really understand this
		//	getopt returns the decimal ascii value of a valid option
		//	e.g. it will return 112 for a p as long as an argument is provided,
		//	but for invalid options, it returns 63
	//std::cout << "Argument: " << static_cast<char>(opt) << "(" << opt << ")"<< std::endl;
 	while (opt != -1)
	{
		switch (opt)
		{
			case 'h':
                		usage();
                		exit(0);

			case 'p':
				host_port = atoi(optarg);
                  		break;

			case 'r':
				break;
		}
		opt = getopt( argc, argv, "dhl:p:r:t:n:s:" );
	}

	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if (hsock == -1)
	{
		printf("Error initializing socket %d\n", errno);
		exit(-1);
	}

	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;

	if ((setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) )
	{
		printf("Error setting options %d\n", errno);
		free(p_int);
		exit(-1);
	}
	free(p_int);

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);

	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;

	if (bind(hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
	{
		fprintf(stderr,"Error binding to socket, make sure nothing else is listening on this port %d\n",errno);
		exit(-1);
	}

	if (listen(hsock, 10) == -1 )
	{
		fprintf(stderr, "Error listening %d\n",errno);
		exit(-1);
	}

	//Now lets do the server stuff
	printf("myhttpd server listening on port %d\n", host_port);

	addr_size = sizeof(sockaddr_in);

	while (true)
	{
		printf("waiting for a connection\n");
		if ((socketfd = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1)
		{
			printf("Received connection from %s\n",inet_ntoa(sadr.sin_addr));
			pthread_create(&thread_id, NULL, &httpHandler, &socketfd);
			printf("Launched thread %lu.\n", thread_id);
		}
		else
		{
			fprintf(stderr, "Error accepting %d\n", errno);
		}
	}
}


