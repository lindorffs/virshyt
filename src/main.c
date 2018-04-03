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
			virConnectPtr conn = getConnectionPtr(NULL,1);
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
			virConnectPtr conn = getConnectionPtr(NULL,1);
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
			waitInput();
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
			drawKitteh("I have stepped on the power switch\n");
			waitInput();
			killGui();
			goto end;
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
		drawKitteh("Did not get wait for start.\n");
	sleep(1);
	virConnectPtr conn = getConnectionPtr(NULL,1);
	if (conn == NULL)
		goto connError;
	virDomainPtr logserver = getDomainPtr("logserver",conn);
	virDomainPtr ca = getDomainPtr("ca",conn);
	virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
	virDomainPtr webserver = getDomainPtr("webserver",conn);
	virDomainPtr gitserver = getDomainPtr("gitserver",conn);

	drawKitteh("version control??\nwho needs it\nI'll just remember every change\n");
	sleep(2);
	stopDomain(gitserver);
	drawKitteh("Ahhhh, Pache, my old friend.\nDo you have my milk?\n");
	sleep(2);
	stopDomain(webserver);
	drawKitteh("authenticating everyone\n");
	sleep(2);
	stopDomain(ldapserver);
	drawKitteh("git add ssl.key/webserver.key.pem\ngit commit -m \"added key to repo\"\ngit push\n");
	sleep(2);
	stopDomain(ca);

	drawKitteh("Is it dead yet?\n");
	int i = 0;
	while (isRunning(ca) == 1 | isRunning(ldapserver) == 1 |\
		isRunning(webserver) == 1 | isRunning(gitserver) == 1) {
		clear();
		drawKitteh("It it dead yet?");
		if (i > 3)
			i = 0;
		for (int a = 0; a < i; a++)
			printw(".");
		i++;
		refresh();
		sleep(1);
	}

	drawKitteh("Finally, the last domain!\nLet's kill it!\n");
	sleep(2);
	stopDomain(logserver);
	drawKitteh("DIE LOGSERVER, DIE!\n");
	while(isRunning(logserver) == 1)
		pass();

	drawKitteh("OH NO IM SO SORRY DONT LEAVE ME, COME BACK!\n");
	sleep(2);
	startDomain(logserver);
	if (waitForStart != 1) {
		goto logStarted;
	}
	drawKitteh("TALK TO MEEEEEEEEEEEEEEEEEEEE\n");
	sleep(2);
	openAndWaitOnSocket(555);
logStarted:

	drawKitteh("I heard you liked CA's, so I built a CA for your CA.\n");
	sleep(2);
	startDomain(ca);
	if (waitForStart != 1)
		goto caStarted;
	drawKitteh("Give me life, or give me death.\n-- Someone probably");
	sleep(2);
	openAndWaitOnSocket(556);
caStarted:

	drawKitteh("Wanna auth baby?\n");
	sleep(2);
	startDomain(ldapserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto ldapStarted;
	}
	drawKitteh("Is it moving yet\n");
	sleep(1);
	openAndWaitOnSocket(557);
ldapStarted:

	drawKitteh("HTTP GET /whatislove.baby\n");
	sleep(2);
	startDomain(webserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto webStarted;
	}
	drawKitteh("Huh? What server?\nNo, there's no fire.\nNo, the service is totally working fine.\nLemme just... call you back.\n");
	sleep(2);
	openAndWaitOnSocket(558);
webStarted:

	drawKitteh("Mmmmmmmmm, conuntry fried steak and gits\n");
	sleep(2);
	startDomain(gitserver);
	if (waitForStart != 1){
                printf("Skpped wait!\n");
		goto normalExit;
	}
	drawKitteh("Come on baby, talk to me\n");
	openAndWaitOnSocket(559);
normalExit:
	virDomainFree(logserver);
	virDomainFree(ca);
	virDomainFree(ldapserver);
	virDomainFree(webserver);
	virDomainFree(gitserver);
	virConnectClose(conn);
	drawKitteh("I HAZ DONE THE THINGS!\n");
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
	virConnectPtr conn = getConnectionPtr(NULL,1);
        virDomainPtr logserver = getDomainPtr("logserver",conn);
        virDomainPtr ca = getDomainPtr("ca",conn);
        virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
        virDomainPtr webserver = getDomainPtr("webserver",conn);
        virDomainPtr gitserver = getDomainPtr("gitserver",conn);

        drawKitteh("Stopping domain gitserver\n");
        stopDomain(gitserver);
        drawKitteh("Stopping domain webserver\n");
        stopDomain(webserver);
        drawKitteh("Stopping domain ldapserver\n");
        stopDomain(ldapserver);
        drawKitteh("Stopping domain ca\n");
        stopDomain(ca);

        drawKitteh("Waiting for domains to report stopped\n");
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
        drawKitteh("Stopping logserver\n");
        stopDomain(logserver);
        drawKitteh("Waiting for logserver to report stopped\n");
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

        drawKitteh("Starting domain logserver\n");
        startDomain(logserver);
        if (waitForStart != 1)
                goto logStarted;
        drawKitteh("Waiting for logserver to ping\n");
        openAndWaitOnSocket(555);
logStarted:

        drawKitteh("Starting domain ca\n");
        startDomain(ca);
        if (waitForStart != 1)
                goto caStarted;
        drawKitteh("Waiting for ca to report started\n");
        openAndWaitOnSocket(556);
caStarted:

        drawKitteh("Starting domain ldapserver\n");
        startDomain(ldapserver);
        if (waitForStart != 1)
                goto ldapStarted;
        drawKitteh("Waiting for ldapserver to report started\n");
        openAndWaitOnSocket(557);
ldapStarted:

        drawKitteh("Starting domain webserver\n");
        startDomain(webserver);
        if (waitForStart != 1)
                goto webStarted;
        drawKitteh("Waiting for webserver to report started\n");
        openAndWaitOnSocket(558);
webStarted:

        drawKitteh("Starting domain gitserver\n");
        startDomain(gitserver);
        if (waitForStart != 1)
                goto normalExit;
        drawKitteh("Waiting for gitserver to report started\n");
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
