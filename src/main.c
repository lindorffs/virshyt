#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <domainManager.h>
#include <gui.h>
#include <socket.h>

int rebootDomains();
void startDomains(int);
void stopDomains();
void printUsage(char* arg) {
	fprintf(stderr,"\
usage: %s --restart\n\
       %s --stop <domain>\n\
       %s --start <domain>\n\
", 	arg, arg, arg);
}

int main(int argc, char** argv) {
	int ret = 0;
	int i = 1;
	if (argc == 1)
		goto inputError;

	for (char* arg = argv[i]; i < argc; i++) {
		if (strcmp(arg, "--restart") == 0) {
			if ((i + 1) < argc) {
				if (strcmp(argv[++i],"with-wait") == 0) {
					printf("Got with-wait\n");
					sleep(2);
					ret = rebootDomains(1);
					goto end;
				}
			}
			ret = rebootDomains(0);
			goto end;
		} else if (strcmp(arg,"--stop")==0) {
		  	if ((argc - i) < 2)
			   goto inputError;
			virConnectPtr conn = getConnectionPtr(NULL,1);
			char* domain;

			domain = argv[++i];
			virDomainPtr doma = getDomainPtr(domain,conn);
			ret = stopDomain(doma);
			goto end;
		} else if (strcmp(arg,"--start") == 0) {
			if ((argc -i) < 2)
				goto inputError;
			virConnectPtr conn = getConnectionPtr(NULL,1);
			char *domain;
			
			domain = argv[++i];
			virDomainPtr doma = getDomainPtr(domain,conn);
			ret = startDomain(doma);
			goto end;
		} else if (strcmp(arg,"--gui") == 0) {
			initGui();
			clear();
			waitInput();
			killGui();
			goto end;
		} else if (strcmp(arg,"--startapp") == 0) {
			startDomains(0);
		} else
			goto inputError;
	}
inputError:
	printUsage(argv[0]);
	return(1);
end:
	return(ret);
}

void pass(){}

int rebootDomains(int waitForStart) {
	initGui();
	if (waitForStart != 1)
		print("Did not get wait for start.\n");
	sleep(1);
	virConnectPtr conn = getConnectionPtr(NULL,1);
	if (conn == NULL)
		goto connError;
	virDomainPtr logserver = getDomainPtr("logserver",conn);
	virDomainPtr ca = getDomainPtr("ca",conn);
	virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
	virDomainPtr webserver = getDomainPtr("webserver",conn);
	virDomainPtr gitserver = getDomainPtr("gitserver",conn);

	print("Stopping domain gitserver\n");
	stopDomain(gitserver);
	print("Stopping domain webserver\n");
	stopDomain(webserver);
	print("Stopping domain ldapserver\n");
	stopDomain(ldapserver);
	print("Stopping domain ca\n");
	stopDomain(ca);

	print("Waiting for domains to report stopped\n");
	int i = 0;
	while (isRunning(ca) == 1 | isRunning(ldapserver) == 1 |\
		isRunning(webserver) == 1 | isRunning(gitserver) == 1) {
		clear();
		int running = isRunning(ca) ? 1 : 0;
		running += isRunning(ldapserver) ? 1 : 0;
	running += isRunning(webserver) ? 1: 0;
		running += isRunning(gitserver) ? 1 : 0;
		printw("Waiting for domains to report stopped");
		if (i > 3)
			i = 0;
		for (int a = 0; a < i; a++)
			printw(".");
		i++;
		refresh();
		sleep(1);
	}

	print("Stopping logserver\n");
	stopDomain(logserver);
	print("Waiting for logserver to report stopped\n");
	while(isRunning(logserver) == 1)
		pass();

	print("Starting domain logserver\n");
	startDomain(logserver);
	if (waitForStart != 1) {
		goto logStarted;
	}
	print("Waiting for logserver to ping\n");
	sleep(2);
	openAndWaitOnSocket(555);
logStarted:

	print("Starting domain ca\n");
	startDomain(ca);
	if (waitForStart != 1)
		goto caStarted;
	print("Waiting for ca to report started\n");
	openAndWaitOnSocket(556);
caStarted:

	print("Starting domain ldapserver\n");
	startDomain(ldapserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto ldapStarted;
	}
	print("Waiting for ldapserver to report started\n");
	sleep(1);
	openAndWaitOnSocket(557);
ldapStarted:

	print("Starting domain webserver\n");
	startDomain(webserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto webStarted;
	}
	print("Waiting for webserver to report started\n");
	openAndWaitOnSocket(558);
webStarted:

	print("Starting domain gitserver\n");
	startDomain(gitserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto normalExit;
	}
	print("Waiting for gitserver to report started\n");
	openAndWaitOnSocket(559);
normalExit:
	virDomainFree(logserver);
	virDomainFree(ca);
	virDomainFree(ldapserver);
	virDomainFree(webserver);
	virDomainFree(gitserver);
	virConnectClose(conn);
	clear();
	killGui();
	return(0);
connError:
	clear();
	killGui();
	return(2);
}

void shutdownDomains() {
	initGui();
	virConnectPtr conn = getConnectionPtr(NULL,1);
        virDomainPtr logserver = getDomainPtr("logserver",conn);
        virDomainPtr ca = getDomainPtr("ca",conn);
        virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
        virDomainPtr webserver = getDomainPtr("webserver",conn);
        virDomainPtr gitserver = getDomainPtr("gitserver",conn);

        print("Stopping domain gitserver\n");
        stopDomain(gitserver);
        print("Stopping domain webserver\n");
        stopDomain(webserver);
        print("Stopping domain ldapserver\n");
        stopDomain(ldapserver);
        print("Stopping domain ca\n");
        stopDomain(ca);

        print("Waiting for domains to report stopped\n");
        int i = 0;
        while (isRunning(ca) == 1 | isRunning(ldapserver) == 1 |\
                isRunning(webserver) == 1 | isRunning(gitserver) == 1) {
                clear();
                int running = isRunning(ca) ? 1 : 0;
                running += isRunning(ldapserver) ? 1 : 0;
        running += isRunning(webserver) ? 1: 0;
                running += isRunning(gitserver) ? 1 : 0;
                printw("Waiting for domains to report stopped");
                if (i > 3)
                        i = 0;
                for (int a = 0; a < i; a++)
                        printw(".");
                i++;
                refresh();
                sleep(1);
        }
        print("Stopping logserver\n");
        stopDomain(logserver);
        print("Waiting for logserver to report stopped\n");
        while(isRunning(logserver) == 1)
                pass();
        virDomainFree(logserver);
        virDomainFree(ca);
        virDomainFree(ldapserver);
        virDomainFree(webserver);
        virDomainFree(gitserver);
        virConnectClose(conn);
        clear();
        killGui();

}

void startDomains(int waitForStart) {
	initGui();
        virConnectPtr conn = getConnectionPtr(NULL,1);
        virDomainPtr logserver = getDomainPtr("logserver",conn);
        virDomainPtr ca = getDomainPtr("ca",conn);
        virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
        virDomainPtr webserver = getDomainPtr("webserver",conn);
        virDomainPtr gitserver = getDomainPtr("gitserver",conn);

        print("Starting domain logserver\n");
        startDomain(logserver);
        if (waitForStart != 1)
                goto logStarted;
        print("Waiting for logserver to ping\n");
        openAndWaitOnSocket(555);
logStarted:

        print("Starting domain ca\n");
        startDomain(ca);
        if (waitForStart != 1)
                goto caStarted;
        print("Waiting for ca to report started\n");
        openAndWaitOnSocket(556);
caStarted:

        print("Starting domain ldapserver\n");
        startDomain(ldapserver);
        if (waitForStart != 1)
                goto ldapStarted;
        print("Waiting for ldapserver to report started\n");
        openAndWaitOnSocket(557);
ldapStarted:

        print("Starting domain webserver\n");
        startDomain(webserver);
        if (waitForStart != 1)
                goto webStarted;
        print("Waiting for webserver to report started\n");
        openAndWaitOnSocket(558);
webStarted:

        print("Starting domain gitserver\n");
        startDomain(gitserver);
        if (waitForStart != 1)
                goto normalExit;
        print("Waiting for gitserver to report started\n");
        openAndWaitOnSocket(559);
normalExit:
        virDomainFree(logserver);
        virDomainFree(ca);
        virDomainFree(ldapserver);
        virDomainFree(webserver);
        virDomainFree(gitserver);
        virConnectClose(conn);
        clear();
        killGui();
}
