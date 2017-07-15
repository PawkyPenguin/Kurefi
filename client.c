#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int sock_fd;
	int option;
	while ((option = getopt(argc, argv, "h")) >= 0) {
		switch(option) {
		case 'h':
			printf("Usage: ./client.exe [FILE]...\n");
			exit(0);
		case '?': 
			err(EXIT_FAILURE, "Unknown flag.");
		}
	}

	if (!(sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0)))
		err(EXIT_FAILURE, "Could not create socket");
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_LOCAL;
	strncpy(addr.sun_path, "asdf", sizeof(addr.sun_path) - 1);
	if(connect(sock_fd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0)
		err(EXIT_FAILURE, "Could not connect to socket.");
	for (int i=optind; i < argc; i++) {
		send(sock_fd, argv[i], strlen(argv[i]), 0);
		send(sock_fd, "\n", 3, 0);
	}
	close(sock_fd);
	return(0);
}
