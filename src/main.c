#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <domainManager.h>
#include <gui.h>
#include <socket.h>

// Global Variables

char *domains[5] = {"gitserver","ca","webserver","ldapserver","logserver"};
char *host = "qemu+tcp://cis235-studentvm/system";
int ports[5] = {559,558,557,556,555};
virConnectPtr conn;

// Function Prototypes

void rebootDomains();
void startDomains(int);
void stopDomains();

void wait();
void pass() {};

// Usage info
void printUsage(char* arg) {
	fprintf(stderr,"\
usage: %s --start <domain>\n\
       %s --stop <domain>\n\
       %s --restart-app [with-wait] # with-wait requires root\n\
       %s --start-app [with-wait]   # \"                     \"\n\
       %s --stop-app\n",arg,arg,arg, arg, arg);
}

int main(int argc, char** argv) {
	int ret = 0;
	int i = 1;
	if (argc == 1)
		goto inputError;
	initGui();
	conn = getConnectionPtr(host,1);
	if (conn == NULL) {
		ret = -1;
		goto end;
	}
	for (char* arg = argv[i]; i < argc; i++) {
		if (strcmp(arg, "--restart-app") == 0) {
			if ((i + 1) < argc) {
				if (strcmp(argv[++i],"with-wait") == 0) {
					rebootDomains(1);
					goto end;
				}
			}
			rebootDomains(0);
			goto end;
		} else if (strcmp(arg,"--start-app") == 0) {
			if ((i + 1) < argc)
				if (strcmp(argv[++i],"with-wait") == 0) {
					printf("Got with-wait\n");
					sleep(1);
					startDomains(1);
					goto end;
				}
			startDomains(0);
			goto end;
		} else if (strcmp(arg,"--stop-app") == 0) {
			stopDomains();
			goto end;
		} else if (strcmp(arg,"--stop")==0) {
                        if ((argc - i) < 2)
                           goto inputError;
                        char* domain;

                        domain = argv[++i];
                        virDomainPtr doma = getDomainPtr(domain,conn);
                        ret = stopDomain(doma);
			while (isRunning(doma) == 1)
				pass();
                        virDomainFree(doma);
                        goto end;
                } else if (strcmp(arg,"--start") == 0) {
                        if ((argc -i) < 2)
                                goto inputError;
                        char *domain;

                        domain = argv[++i];
                        virDomainPtr doma = getDomainPtr(domain,conn);
                        ret = startDomain(doma);
			while (isRunning(doma) != 1)
				pass();
                        virDomainFree(doma);
                        goto end;
		} else
			goto inputError;
	}
inputError:
	printUsage(argv[0]);
	ret = -1;
end:
	if (conn != NULL)
		virConnectClose(conn);
	killGui();
	return(ret);
}

void stopDomains() {
	for (int i = 0; i < sizeof(domains)/sizeof(char *); i++) {
		virDomainPtr doma = getDomainPtr(domains[i], conn);
		if (doma != NULL) {
			char message[28];
			sprintf(message, "Stopping domain %s", domains[i]);
			drawMessage(message);
			stopDomain(doma);
			while (isRunning(doma) == 1)
				pass();
			virDomainFree(doma);
		}	
	}	
}

void startDomains(int waitForStart) {
	for (int i = sizeof(domains)/sizeof(char *) - 1; i >= 0; i--) {
		virDomainPtr doma = getDomainPtr(domains[i], conn);
		if (doma != NULL) {
			char message[50];
			sprintf(message, "Starting domain %s", domains[i]);
			drawMessage(message);
			startDomain(doma);
			while (isRunning(doma) != 1)
				pass();
			if (waitForStart) {
				sprintf(message, "Waiting for domain %s on %i", domains[i], ports[i]);
				drawMessage(message);
				openAndWaitOnSocket(ports[i]);
			}	
		}
	}
}

void rebootDomains(int waitForStart) {
	stopDomains();
	wait();
	startDomains(waitForStart);
}

void wait() {
	int i = 30;
	char message[30];
	while (i >= 0) {
		sprintf(message, "sleeping for %i secs", i);
		drawMessage(message);
		sleep(1);
		i--;
	}
}
