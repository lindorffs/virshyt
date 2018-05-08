#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <domainManager.h>
#include <gui.h>
#include <socket.h>

// Global Variables

char *domains[] = {"ca","gitserver","webserver","ldapserver","logserver"};
char *host = "qemu+tcp://cis235-studentvm/system";
virConnectPtr conn;

// Function Prototypes

int rebootDomains();
int startDomains(int);
int stopDomains();

void wait();
void pass() {};

void loadConfig() {
	return;
	char *lines[] = {"qemu+tcp://cis235-studentvm/system", "gitserver", "ca" };
	memset(host, 0, sizeof(host));
	strcpy(host, lines[0]);
	for (int i = 0; i < sizeof(domains)/sizeof(char *); i++) {
		memset(domains[i], 0, sizeof(domains[i]));
	}
	strcpy(domains[0],"test");
//	memset(domains, 0, sizeof(domains));
//	strcpy(domains[0], lines[1]);
}

// Usage info
void printUsage(char* arg) {
	fprintf(stderr,"\
usage: %s --start <domain>\n\
       %s --stop <domain>\n\
       %s --status <domain>\n\
       %s --restart-app [with-wait] # with-wait requires root\n\
       %s --start-app [with-wait]   # \"                     \"\n\
       %s --stop-app\n",arg,arg,arg,arg,arg,arg);
}

int main(int argc, char** argv) {
	int ret = 0;
	int i = 1;
	if (argc == 1)
		goto inputError;
	conn = getConnectionPtr(host,1);
	if (conn == NULL) {
		ret = 99;
		goto end;
	}
	for (char* arg = argv[i]; i < argc; i++) {
		if (strcmp(arg, "--restart-app") == 0) {
			initGui();
			if ((i + 1) < argc) {
				if (strcmp(argv[++i],"with-wait") == 0) {
					setWait();
					ret = rebootDomains(1);
					killGui();
					goto end;
				}
			}
			ret = rebootDomains(0);
			killGui();
			goto end;
		} else if (strcmp(arg,"--start-app") == 0) {
			initGui();
			if ((i + 1) < argc)
				if (strcmp(argv[++i],"with-wait") == 0) {
					printf("Got with-wait\n");
					sleep(1);
					setWait();
					ret = startDomains(1);
					killGui();
					goto end;
				}
			ret = startDomains(0);
			killGui();
			goto end;
		} else if (strcmp(arg,"--stop-app") == 0) {
			initGui();
			stopDomains();
			killGui();
			goto end;
		} else if (strcmp(arg,"--stop") == 0) {
                        if ((argc - i) < 2)
                           goto inputError;
                        char* domain;

                        domain = argv[++i];
                        virDomainPtr doma = getDomainPtr(domain,conn);
                        ret = stopDomain(doma);
                        virDomainFree(doma);
                        goto end;
                } else if (strcmp(arg,"--start") == 0) {
                        if ((argc -i) < 2)
                                goto inputError;
                        char *domain;

                        domain = argv[++i];
                        virDomainPtr doma = getDomainPtr(domain,conn);
                        ret = startDomain(doma);
                        virDomainFree(doma);
                        goto end;
		} else if (strcmp(arg,"--status") == 0) {
			if ((i + 1) > argc)
				goto inputError;
			char *domain;

			domain = argv[++i];
			virDomainPtr doma = getDomainPtr(domain,conn);
			ret = isRunning(doma);
			virDomainFree(doma);
			goto end;
		}
	}
inputError:
	printUsage(argv[0]);
	ret = -1;
end:
	if (conn != NULL)
		virConnectClose(conn);
	return(ret);
}

// Stops the domains from bottom to top.
// returns number of running domains in app
// that should have been stopped but weren't.
// 0 is good, anything but is bad.
int stopDomains() {
	int ret = sizeof(domains)/sizeof(char*);
	for (int i = 0; i < sizeof(domains)/sizeof(char *); i++) {
		virDomainPtr doma = getDomainPtr(domains[i], conn);
		if (doma != NULL) {
			char message[28];
			sprintf(message, "Stopping domain %s", domains[i]);
			drawMessage(message);
			if (stopDomain(doma) == 0)
				ret--;
			while (isRunning(doma) != 0) {

			}
			virDomainFree(doma);
		}
	}
	clear();
	return ret;
}

// Start domains from top to bottom.
// Takes an int, which if set to anything other than
// 0, will cause to wait for the server to respond on the
// corresponding port.
// Returns the number of domains that did not start.
// 0 is good, anything but is bad.
int startDomains(int waitForStart) {
	int ret = 0;
	for (int i = sizeof(domains)/sizeof(char *) - 1; i >= 0; i--) {
		virDomainPtr doma = getDomainPtr(domains[i], conn);
		if (doma != NULL) {
			char message[50];
			sprintf(message, "Starting domain %s", domains[i]);
			drawMessage(message);
			if (startDomain(doma) != 0)
				ret += 1;
			else {
				sprintf(message, "Waiting for ''start'' of %s", domains[i]);
				while (isRunning(doma) != 1) {

				}
				if (waitForStart) {
					sprintf(message, "Waiting for domain %s to do the ssh", domains[i]);
					drawMessage(message);
					int s = 1;
					while ( s != 0 )
						s = connectToAndSendOnSocket(22, domains[i],"");
				}
			}
		}
	}
	return ret;
}

// restarts the domains, in the order of listing.
// returns if all steps went according to plan.
// 0 is good.
// 1 means failed to stop some.
// 2 means failed to start some.
// 3 means failed parts of both steps.
int rebootDomains(int waitForStart) {
	int ret = 0;
	if (stopDomains() != 0) {
		drawMessage("Failed to stop some domains. pls check");
		ret += 1;
		sleep(2);
	}	

	// wait for an annoying amount of time......
	wait();

	if (startDomains(waitForStart) != 0) {
		drawMessage("Failed to start some domains. pls check");
		ret += 2;
		sleep(2);
	}
	return ret;
}

// waits 30 seconds.
// returns nothing.
// make fun.
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
