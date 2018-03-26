#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define LISTEN_PORT 555
#define DEBUG 1


int waitForConnection() {
	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr,"Failed to open socket\n");
		return(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = LISTEN_PORT;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(LISTEN_PORT);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr,"Failed to bind socket\n");
		return(2);

	}
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
	if (newsockfd < 0) {
		fprintf(stderr,"Failed to accept\n");
		return(3);
	}
	bzero(buffer,256);
	n = read(newsockfd,buffer,255);
	if (n < 0) {
		fprintf(stderr,"Failed to read from socket\n");
		return(4);
	}
	n = write(newsockfd,"MSG RECV",8);
	if (n < 0) {
		fprintf(stderr,"Failed to write to socket\n");
		return(5);
	}
	return 0;
}

int sendData(char* host) {
	int sockfd, portno, n;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr,"Failed to open socket\n");
		return(1);
	}

	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr,"Failed to find host\n");
		return(2);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(LISTEN_PORT);
	if (connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr,"Failed to connect to host\n");
		return (3);
	}
	n = write(sockfd,"STARTED",strlen("STARTED"));
	if (n < 0) {
		fprintf(stderr,"Failed to write to server\n");
		return(4);
	}
	n = read(sockfd,buffer,255);
	if (n < 0) {
		fprintf(stderr,"Failed to read from server\n");
		return(5);
	}
	return(0);
}

#endif
