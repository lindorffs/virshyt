#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <domainManager.h>
#include <gui.h>
#include <socket.h>

int rebootDomains();
int shutdownDomain(char* name);
int startupDomain(char* name);
void printUsage(char* arg) {
	fprintf(stderr,"\
usage: %s --restart\n\
       %s --stop <domain>\n\
       %s --start <domain>\n\
       ", arg, arg, arg);
}

int main(int argc, char** argv) {
	int ret = 0;
	int i = 1;
	if (argc == 1)
		goto inputError;

	for (char* arg = argv[i]; i < argc; i++) {
		if (strcmp(arg, "--restart") == 0) {
			ret = rebootDomains();
			goto end;
		} else if (strcmp(arg,"--stop")==0) {
		  	if ((argc - i) < 2)
			   goto inputError;
			char* domain;

			domain = argv[++i];
			ret = shutdownDomain(domain);
			goto end;
		} else if (strcmp(arg,"--start") == 0) {
			if ((argc -i) < 2)
				goto inputError;
			char *domain;
			
			domain = argv[++i];
			ret = startupDomain(domain);
			goto end;
		} else if (strcmp(arg,"--gui") == 0) {
			initGui();
			clear();
			waitInput();
			killGui();
			goto end;
		} else if (strcmp(arg,"--serv") == 0) {
			if (argc < i + 2)
				goto inputError;
			int port = htons(atoi(argv[++i]));
			openAndWaitOnSocket(port);
			goto end;
		} else if (strcmp(arg,"--cli") == 0) {
			if (argc < i + 4)
				goto inputError;
			char *host = argv[++i];
			int port = htons(atoi(argv[++i]));
			char *data = argv[++i];
			connectToAndSendOnSocket(port, host, data);
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

int shutdownDomain(char* domain) {
	virConnectPtr conn = getConnectionPtr(NULL,1);
	if (conn == NULL)
		goto connError;
	virDomainPtr doma = getDomainPtr(domain,conn);
	if (doma == NULL)
		goto domaError;
	if (stopDomain(doma) != 0)
		goto stopError;
normalExit:
	virDomainFree(doma);
	virConnectClose(conn);
	return(0);
connError:
	return(2);
domaError:
	virConnectClose(conn);
	return(3);
stopError:
	virDomainFree(doma);
	virConnectClose(conn);
	return(4);
}

int startupDomain(char* domain) {
        virConnectPtr conn = getConnectionPtr(NULL,1);
        if (conn == NULL)
                goto connError;
        virDomainPtr doma = getDomainPtr(domain,conn);
        if (doma == NULL)
                goto domaError;
        if (startDomain(doma) != 0)
                goto startError;
normalExit:
        virDomainFree(doma);
        virConnectClose(conn);
        return(0);
connError:
        return(2);
domaError:
        virConnectClose(conn);
        return(3);
startError:
        virDomainFree(doma);
        virConnectClose(conn);
        return(4);
}

int rebootDomains() {
	initGui();
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
	print("Waiting for logserver to ping\n");
	openAndWaitOnSocket(555);

	print("Starting domain ca\n");
	startDomain(ca);
	print("Waiting for ca to report started\n");
	openAndWaitOnSocket(556);

	print("Starting domain ldapserver\n");
	startDomain(ldapserver);
	print("Waiting for ldapserver to report started\n");
	openAndWaitOnSocket(557);

	print("Starting domain webserver\n");
	startDomain(webserver);
	print("Waiting for webserver to report started\n");
	openAndWaitOnSocket(558);

	print("Starting domain gitserver\n");
	startDomain(gitserver);
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

