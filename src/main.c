#include <stdio.h>
#include <string.h>

#include <domainManager.h>

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
	virConnectPtr conn = getConnectionPtr(NULL,1);
	if (conn == NULL)
		goto connError;
	virDomainPtr logserver = getDomainPtr("logserver",conn);
	virDomainPtr ca = getDomainPtr("ca",conn);
	virDomainPtr ldapserver = getDomainPtr("ldapserver",conn);
	virDomainPtr webserver = getDomainPtr("webserver",conn);
	virDomainPtr gitserver = getDomainPtr("gitserver",conn);

	printf("Stopping domain gitserver\n");
	stopDomain(gitserver);
	printf("Stopping domain webserver\n");
	stopDomain(webserver);
	printf("Stopping domain ldapserver\n");
	stopDomain(ldapserver);
	printf("Stopping domain ca\n");
	stopDomain(ca);

	printf("Waiting for domains to report stopped\n");
	while (isRunning(ca) == 1 | isRunning(ldapserver) == 1 |\
		isRunning(webserver) == 1 | isRunning(gitserver) == 1)
		pass();

	printf("Stopping logserver\n");
	stopDomain(logserver);
	printf("Waiting for logserver to report stopped\n");
	while(isRunning(logserver) == 1)
		pass();

	printf("Starting domain logserver\n");
	startDomain(logserver);
	printf("Waiting for logserver to ping\n");

	printf("Starting domain ca\n");
	startDomain(ca);
	printf("Waiting for ca to report started\n");

	printf("Starting domain ldapserver\n");
	startDomain(ldapserver);
	printf("Waiting for ldapserver to report started\n");

	printf("Starting domain webserver\n");
	startDomain(webserver);
	printf("Waiting for webserver to report started\n");

	printf("Starting domain gitserver\n");
	startDomain(gitserver);
	printf("Waiting for gitserver to report started\n");
normalExit:
	virDomainFree(logserver);
	virDomainFree(ca);
	virDomainFree(ldapserver);
	virDomainFree(webserver);
	virDomainFree(gitserver);
	virConnectClose(conn);
	return(0);
connError:
	return(2);
}

