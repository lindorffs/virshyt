#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <domainManager.h>
#include <gui.h>
#include <socket.h>

int rebootDomains();
void startDomains(int);
void shutdownDomains();
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

	for (char* arg = argv[i]; i < argc; i++) {
		if (strcmp(arg, "--restart-app") == 0) {
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
			virConnectPtr conn = getConnectionPtr("qemu+tcp://cis235-studentvm/system",1);
			char* domain;

			domain = argv[++i];
			virDomainPtr doma = getDomainPtr(domain,conn);
			ret = stopDomain(doma);
			virDomainFree(doma);
			virConnectClose(conn);
			goto end;
		} else if (strcmp(arg,"--start") == 0) {
			if ((argc -i) < 2)
				goto inputError;
			virConnectPtr conn = getConnectionPtr("qemu+tcp://cis235-studentvm/system",1);
			char *domain;
			
			domain = argv[++i];
			virDomainPtr doma = getDomainPtr(domain,conn);
			ret = startDomain(doma);
			virDomainFree(doma);
			virConnectClose(conn);
			goto end;
		} else if (strcmp(arg,"--gui") == 0) {
			initGui();
			clear();
			killGui();
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
			ret = 0;
			goto end;
		} else if (strcmp(arg,"--stop-app") == 0) {
			shutdownDomains();
			ret = 0;
			goto end;
		} else if (strcmp(arg,"--print-kitteh") == 0) {
			initGui();
			drawMessage("test!");
			getch();
			killGui();
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
void wait();

int rebootDomains(int waitForStart) {
	initGui();
	if (waitForStart != 1)
		drawMessage("Did not get wait for start.");
	sleep(1);
	virConnectPtr conn = getConnectionPtr("qemu+tcp://cis235-studentvm/system",1);
	if (conn == NULL)
		goto connError;
	virDomainPtr logserver = getDomainPtr("logserver",conn);
	virDomainPtr ca = getDomainPtr("ca",conn);
	virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
	virDomainPtr webserver = getDomainPtr("webserver",conn);
	virDomainPtr gitserver = getDomainPtr("gitserver",conn);

	drawMessage("stopping gitserver");
	stopDomain(gitserver);
	drawMessage("stopping webserver");
	stopDomain(webserver);
	drawMessage("stopping ldapserver");
	stopDomain(ldapserver);
	drawMessage("stopping ca");
//	drawMessage("git add ssl.key/*.key.pem \\ git commit -m \"added key to repo\" \\ git push");
	stopDomain(ca);

	int i = 0;
	while (isRunning(ca) == 1 | isRunning(ldapserver) == 1 |\
		isRunning(webserver) == 1 | isRunning(gitserver) == 1) {
		clear();
		drawMessage("It it dead yet?");
		if (i > 3)
			i = 0;
		for (int a = 0; a < i; a++)
			printw(".");
		i++;
		refresh();
		sleep(1);
	}


	drawMessage("stopping logserver");
	stopDomain(logserver);

	wait();


	drawMessage("starting logserver");
	while(isRunning(logserver) == 1)
		pass();

	drawMessage("starting logserver");
	startDomain(logserver);
	if (waitForStart != 1) {
		goto logStarted;
	}
	drawMessage("waiting for logserver");
	openAndWaitOnSocket(555);
logStarted:

	drawMessage("starting for ca");
	startDomain(ca);
	if (waitForStart != 1)
		goto caStarted;
	drawMessage("waiting for ca");
	openAndWaitOnSocket(556);
caStarted:

	drawMessage("starting ldapserver");
	startDomain(ldapserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto ldapStarted;
	}
	drawMessage("waiting for ldapserver");
	openAndWaitOnSocket(557);
ldapStarted:

	drawMessage("starting webserver");
	startDomain(webserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto webStarted;
	}
	drawMessage("waiting for webserver");
	openAndWaitOnSocket(558);
webStarted:

	drawMessage("starting gitserver");
	startDomain(gitserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto normalExit;
	}
	drawMessage("waiting for gitserver");
	openAndWaitOnSocket(559);
normalExit:
	virDomainFree(logserver);
	virDomainFree(ca);
	virDomainFree(ldapserver);
	virDomainFree(webserver);
	virDomainFree(gitserver);
	virConnectClose(conn);
	drawMessage("the things have been done");
	sleep(2);
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
	virConnectPtr conn = getConnectionPtr("qemu+tcp://cis235-studentvm/system",1);
        virDomainPtr logserver = getDomainPtr("logserver",conn);
        virDomainPtr ca = getDomainPtr("ca",conn);
        virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
        virDomainPtr webserver = getDomainPtr("webserver",conn);
        virDomainPtr gitserver = getDomainPtr("gitserver",conn);

        drawMessage("Stopping domain gitserver");
        stopDomain(gitserver);
        drawMessage("Stopping domain webserver");
        stopDomain(webserver);
        drawMessage("Stopping domain ldapserver");
        stopDomain(ldapserver);
        drawMessage("Stopping domain ca");
        stopDomain(ca);

        drawMessage("Waiting for domains to report stopped");
        int i = 0;
        while (isRunning(ca) == 1 | isRunning(ldapserver) == 1 |\
                isRunning(webserver) == 1 | isRunning(gitserver) == 1) {
                clear();
                int running = isRunning(ca) ? 1 : 0;
                running += isRunning(ldapserver) ? 1 : 0;
        running += isRunning(webserver) ? 1: 0;
                running += isRunning(gitserver) ? 1 : 0;
                drawMessage("Waiting for domains to report stopped");
                if (i > 3)
                        i = 0;
                for (int a = 0; a < i; a++)
                        printw(".");
                i++;
                refresh();
                sleep(1);
        }
        drawMessage("Stopping logserver");
        stopDomain(logserver);
        drawMessage("Waiting for logserver to report stopped");
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
        virConnectPtr conn = getConnectionPtr("qemu+tcp://cis235-studentvm/system",1);
        virDomainPtr logserver = getDomainPtr("logserver",conn);
        virDomainPtr ca = getDomainPtr("ca",conn);
        virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
        virDomainPtr webserver = getDomainPtr("webserver",conn);
        virDomainPtr gitserver = getDomainPtr("gitserver",conn);

        drawMessage("Starting domain logserver");
        startDomain(logserver);
        if (waitForStart != 1)
                goto logStarted;
        drawMessage("Waiting for logserver to ping");
        openAndWaitOnSocket(555);
logStarted:

        drawMessage("Starting domain ca");
        startDomain(ca);
        if (waitForStart != 1)
                goto caStarted;
        drawMessage("Waiting for ca to report started");
        openAndWaitOnSocket(556);
caStarted:

        drawMessage("Starting domain ldapserver");
        startDomain(ldapserver);
        if (waitForStart != 1)
                goto ldapStarted;
        drawMessage("Waiting for ldapserver to report started");
        openAndWaitOnSocket(557);
ldapStarted:

        drawMessage("Starting domain webserver");
        startDomain(webserver);
        if (waitForStart != 1)
                goto webStarted;
        drawMessage("Waiting for webserver to report started");
        openAndWaitOnSocket(558);
webStarted:

        drawMessage("Starting domain gitserver");
        startDomain(gitserver);
        if (waitForStart != 1)
                goto normalExit;
        drawMessage("Waiting for gitserver to report started");
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
