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
	n = write(newsockfd,"MSGRECV",strlen("MSGRECV"));

	close(newsockfd);
	close(sockfd);
}

int connectToAndSendOnSocket(int portno, char* host, char* data) {
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		return 3;
	}

	server = gethostbyname(host);
	if (server == NULL) {
		close(sockfd);
		return 3;
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		close(sockfd);
		return 1;
	}

	close(sockfd);
	return 0;
}

#endif
