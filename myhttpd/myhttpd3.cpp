#include <iostream>
#include <queue>
#include <sys/stat.h>
#include <syslog.h>

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
bool g_terminated = false;

void usage()
{
	printf("myhttpd, a simple webserver\n");
	printf("ver 1.0, 2014\n");
	printf("Usage Summary: myhttpd -h -p portno \n");
	printf("	-h: display the summary\n");
	printf("	-p: change default port number for example: -p 8080\n\n");
}

void signal_handler(int sig) {

    switch(sig) {
        case SIGHUP:
            syslog(LOG_WARNING, "Received SIGHUP signal.");
            break;
        case SIGTERM:
            syslog(LOG_WARNING, "Received SIGTERM signal.");
            g_terminated = true;
            break;
        default:
            syslog(LOG_WARNING, "Unhandled signal ");
            break;
    }
}

void *httpHandler(void* socketfd)
{
	char buffer[1024];
	int buffer_len = 1024;
	int bytecount;
	int socket = *((int*)socketfd);

	memset(buffer, 0, buffer_len);
	if ((bytecount = recv(socket, buffer, buffer_len, 0))== -1)
	{
		syslog(LOG_ERR, "[%d] Error receiving data %d\n", socket, errno);
		return 0;
	}

	syslog(LOG_INFO, "[%d] Received bytes %d", socket, bytecount);

	strcpy(buffer, "HTTP/1.1 200 OK\nServer: demo\nContent-Length: 37\nConnection: close\nContent-Type: html\n\n<html><body>Welcome to my first page!</body></html>");

	if ((bytecount = send(socket, buffer, strlen(buffer), 0))== -1)
	{
		syslog(LOG_ERR, "[%d] Error sending data %d\n", socket, errno);
		return 0;
	}

	syslog(LOG_INFO, "[%d] Sent bytes %d\n", socket, bytecount);

	close(socket);
	sleep(1);	// Added a sleep just to make sure they are running
				// concurrently.
	return 0;
}

int httpd (int argc, char *argv[])
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

	openlog ("myhttpd", LOG_PID, LOG_DAEMON);
	syslog(LOG_INFO, "daemon attempting to begin.");
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
		syslog(LOG_ERR, "Error initializing socket %d\n", errno);
		closelog();
		exit(-1);
	}

	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;

	if ((setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) )
	{
		syslog(LOG_ERR, "Error setting options %d\n", errno);
		free(p_int);
		closelog();
		exit(-1);
	}
	free(p_int);

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);

	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;

	if (bind(hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
	{
		syslog(LOG_ERR,"Error binding to socket, make sure nothing else is listening on this port %d\n",errno);
		closelog();
		exit(-1);
	}

	if (listen(hsock, 10) == -1 )
	{
		syslog(LOG_ERR, "Error listening %d\n",errno);
		closelog();
		exit(-1);
	}

	//Now lets do the server stuff
	syslog (LOG_INFO, "myhttpd server listening on port %d\n", host_port);

	addr_size = sizeof(sockaddr_in);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
	while (!g_terminated)
	{
		syslog(LOG_INFO, "waiting for a connection\n");
		if ((socketfd = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1)
		{
			syslog(LOG_INFO, "Received connection from %s\n",inet_ntoa(sadr.sin_addr));
			pthread_create(&thread_id, NULL, &httpHandler, &socketfd);
		}
		else
		{
			syslog(LOG_ERR, "Error accepting %d\n", errno);
		}
	}
	syslog(LOG_INFO, "Exiting.");
	closelog();
}


int main(int argc, char *argv[])
{
    									// http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
    pid_t pid, sid;
    
    pid = fork();                       // Fork off parent process
    if (pid < 0) exit (EXIT_FAILURE);   // 
    if (pid > 0) exit (EXIT_SUCCESS);   // exit parent process
    umask(0);                          	// Change file mode mask in order to
                                        // write to files.
    sid = setsid();                     // Create session ID for the 
                                        // child process
    if (sid < 0) exit(EXIT_FAILURE);    // the child is an orphan!
    
    if ((chdir("/")) < 0) exit(EXIT_FAILURE);   // Attempt to change working
                                                // directory to root because
                                                // all systems have a root
                                                // directory
    close(STDIN_FILENO);        // Close out file descriptors
    close(STDOUT_FILENO);       // They may introduce a 
    close(STDERR_FILENO);       // security risk.
    
    httpd (argc,argv);
    
    exit(EXIT_SUCCESS);
}



