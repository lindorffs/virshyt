#include <socket.h>

void printUsage(char* prog) {
	printf("usage: %s --serv <port>\n\
       %s --cli <host> <port>\n",prog,prog);
}

int main(int argc, char** argv) {
        int ret = 0;
        int i = 1;
        if (argc == 1)
                goto inputError;

        for (char* arg = argv[i]; i < argc; i++) {
                if (strcmp(arg,"--cli") == 0) {
                        if (argc < i + 3)
                                goto inputError;
                        char *host = argv[++i];
                        int port = atoi(argv[++i]);
                        return(connectToAndSendOnSocket(port, host));
                        goto end;
                }
	}
inputError:
	printUsage(argv[0]);
	return(1);
end:
	return(0);
}
