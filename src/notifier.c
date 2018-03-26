#include <socket.h>
#include <string.h>

void printUsage(char* prog) {
	printf("usage: %s <hostname>\n",prog);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printUsage(argv[0]);
		return(2);
	}
	char* host = argv[1];
	sendData(host);
	return (0);
}
