#include <socket.h>

void printUsage(char* prog) {
	printf("usage: %s --serv <port>\n\
       %s --cli <host> <port> <data>\n",prog,prog);
}

int main(int argc, char** argv) {
        int ret = 0;
        int i = 1;
        if (argc == 1)
                goto inputError;

        for (char* arg = argv[i]; i < argc; i++) {
		if (strcmp(arg,"--serv") == 0) {
                        if (argc < i + 2)
                                goto inputError;
                        int port = atoi(argv[++i]);
                        openAndWaitOnSocket(port);
                        goto end;
                } else if (strcmp(arg,"--cli") == 0) {
                        if (argc < i + 4)
                                goto inputError;
                        char *host = argv[++i];
                        int port = atoi(argv[++i]);
                        char *data = argv[++i];
			printf("%d",port);
                        connectToAndSendOnSocket(port, host, data);
                        goto end;
                }
	}
inputError:
	printUsage(argv[0]);
	return(1);
end:
	return(0);
}
