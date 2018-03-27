#include <socket.h>
#include <string.h>

void printUsage(char* prog) {
	printf("usage: %s <hostname>\n       %s -s\n",prog,prog);
}

int main(int argc, char** argv) {
	if (argc == 1) {
		printUsage(argv[0]);
		return(2);
	}

	if (strcmp(argv[1], "-s") == 0)
		goto server;
	char* host = argv[1];
	sendData(host);
	goto end;
server:
	waitForConnection();
end:
	return (0);
}
