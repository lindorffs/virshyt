#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 500

void openAndWaitOnSocket(int portno) {
	printf("Opening socket on %d\n",portno);
	int sockfd, newsockfd, clilen;

	char buffer[BUF_SIZE];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		fprintf(stderr, "Failed to open socket\n");
		return;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	printf("Hosting on port %d\n",serv_addr.sin_port);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Failed to bind socket\n");
		return;
	}

	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

	if (newsockfd < 0) {
		fprintf(stderr, "Failed to accept socket\n");
		return;
	}

	bzero(buffer,BUF_SIZE);
	n = read(newsockfd, buffer, BUF_SIZE-1);

	if (n < 0) {
		fprintf(stderr,"Failed to read socket\n");
		return;
	}
	printf("%s\n",buffer);
	n = write(newsockfd,"MSGRECV",strlen("MSGRECV"));

	close(newsockfd);
	close(sockfd);
}

void connectToAndSendOnSocket(int portno, char* host, char* data) {
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		fprintf(stderr,"Failed to open socket\n");
		return;
	} else
		printf("Opened socket\n");

	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr,"Host does not exist\n");
		close(sockfd);
		return;
	} else
		printf("Found host\n");

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	printf("Connecting to host on port %d\n",serv_addr.sin_port);

	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr,"Failed to connect on socket\n");
		close(sockfd);
		return;
	} else
		printf("Connected to host\n");

	n = write(sockfd, data, strlen(data));

	if (n < 0) {
		fprintf(stderr,"Failed to write data on socket\n");
		close(sockfd);
		return;
	}

	close(sockfd);
}

#endif
