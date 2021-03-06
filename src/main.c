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

int rebootDomains(int);
int startDomains(int);
int stopDomains(void);

void wait(void);
void pass(void) {};

// is called, but returns right away.
// still working on this....
void loadConfig(void) {
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
       %s --restart-app [with-wait]\n\
       %s --start-app [with-wait]\n\
       %s --stop-app\n",arg,arg,arg,arg,arg,arg);
}

// this ungodly function uses a hideous loop. it's unneeded.
// i just don't want to get rid of it. there's probably
// 10,000,000 other ways to do it, all of which are better.
// if you've got a problem with "it works, leave it be..."
// you can fix it.

int main(int argc, char** argv) {
	int ret = 0; // we return this value at the end, setting as we go.
	int i = 1;
	if (argc == 1) // we need more than 1 argument, no matter what.
		goto inputError;
	conn = getConnectionPtr(host,1); // get our libvirt connection pointer
					 // store it outside the function so our
					 // other functions can use it.
	if (conn == NULL) {
		ret = 99;		 // return an absurd value, because our
					 // connection to the host failed.
		goto end;
	}
	for (char* arg = argv[i]; i < argc; i++) {
		if (strcmp(arg, "--restart-app") == 0) {
			initGui(); // we're going to need ncurses
			if ((i + 1) < argc) {
				if (strcmp(argv[++i],"with-wait") == 0) {
					setWait(); // set this so our ncurses
						   // displays a little message
						   // in the corner.
					ret = rebootDomains(1); // error handling
								// occurs within
								// rebootDomains()
					killGui();	// kill ncurses.
					goto end;
				}
			}
			ret = rebootDomains(0); // error handling occurs within
						// rebootDomains()
			killGui();
			goto end;
		} else if (strcmp(arg,"--start-app") == 0) {
			initGui(); // ncurses
			if ((i + 1) < argc)
				if (strcmp(argv[++i],"with-wait") == 0) {
					setWait(); // show a thingy
					ret = startDomains(1); // returns how many
							       // domains did not
							       // start.
					killGui(); // no more ncurses.
					goto end;
				}
			ret = startDomains(0); // returns how many domains did not
					       // start.
			killGui(); // no more ncurses
			goto end;
		} else if (strcmp(arg,"--stop-app") == 0) {
			initGui(); // ncurses
			ret = stopDomains(); // returns how many domains did not
					     // stop.
			killGui(); // no more ncurses
			goto end;
		} else if (strcmp(arg,"--stop") == 0) {
                        if ((argc - i) < 2)
                           goto inputError;
                        char* domain;

                        domain = argv[++i];
                        virDomainPtr doma = getDomainPtr(domain,conn);
                        ret = stopDomain(doma); // returns 0 if the domain stops
						// returns 1 if not.
                        virDomainFree(doma);
                        goto end;
                } else if (strcmp(arg,"--start") == 0) {
                        if ((argc -i) < 2)
                                goto inputError;
                        char *domain;

                        domain = argv[++i];
                        virDomainPtr doma = getDomainPtr(domain,conn);
                        ret = startDomain(doma); // returns 0 if the domain starts
						 // returns 1 if not.
                        virDomainFree(doma);
                        goto end;
		} else if (strcmp(arg,"--status") == 0) {
			if ((i + 1) > argc)
				goto inputError;
			char *domain;

			domain = argv[++i];
			virDomainPtr doma = getDomainPtr(domain,conn);
			ret = isRunning(doma); // returns 0 if not running.
					       // returns 1 if it is. 
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
int stopDomains(void) {
	int ret = sizeof(domains)/sizeof(char*);
	for (int i = 0; i < sizeof(domains)/sizeof(char *); i++) {
		virDomainPtr doma = getDomainPtr(domains[i], conn);
		if (doma != NULL) {
			char message[28];
			sprintf(message, "Stopping domain %s", domains[i]);

			drawMessage(message);

			if (stopDomain(doma) == 0) {
				// decrement the number of domains
				// to be stopped.
				ret--;
				while (isRunning(doma) != 0) {
					// wait until stopped.
				}
			}
			// free the domain
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

			// if the domain failed to start
			// increment the error count
			if (startDomain(doma) != 0)
				ret += 1;
			else {
				// else wait for it to report started
				sprintf(message, "Waiting for ''start'' of %s", domains[i]);
				while (isRunning(doma) != 1) {

				}

				// if told to wait for actual start
				if (waitForStart) {
					sprintf(message, "Waiting for domain %s to do the ssh", domains[i]);
					drawMessage(message);
					int s = 1;
					// wait for a connection to port 22 on the domain.
					while ( s != 0 )
						s = connectToAndSendOnSocket(22, domains[i]);
				}
			}
			virDomainFree(doma);
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
void wait(void) {
	int i = 30;
	char message[30];
	while (i >= 0) {
		sprintf(message, "sleeping for %i secs", i);
		drawMessage(message);
		sleep(1);
		i--;
	}
}
