#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <err.h>
#include <stdlib.h>

// Used to send a custom event to the keydaemon's socket
int main(int argc, char *argv[]){
	int sock_fd;
	int option;
	int as_key = 0;
	while ((option = getopt(argc, argv, "hk")) >= 0) {
		switch(option) {
		case 'h':
			printf("Usage: ./client.exe [-k] event...\n");
			exit(0);
		case 'k':
			as_key = 1;
			break;
		case '?': 
			err(EXIT_FAILURE, "Unknown flag.");
		}
	}

	// open socket file descriptor
	if (!(sock_fd = socket(AF_LOCAL, SOCK_SEQPACKET, 0)))
		err(EXIT_FAILURE, "Could not create socket");
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_LOCAL;
	strncpy(addr.sun_path, "asdf", sizeof(addr.sun_path) - 1);
	// connect to socket
	if(connect(sock_fd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0)
		err(EXIT_FAILURE, "Could not connect to socket.");
	char *c;
	int size;
	// send all arguments through the socket. If the -k flag was passed, pretend that a keypress is being sent.
	for (int i=optind; i < argc; i++) {
		if (as_key) {
			if (-1 == (size = asprintf(&c, "_%s ", argv[i]))) {
				err(EXIT_FAILURE, "Allocating memory for string failed");
			}
		}
		else {
			if (-1 == (size = asprintf(&c, "%s ", argv[i]))) {
				err(EXIT_FAILURE, "Allocating memory for string failed");
			}
		}
		send(sock_fd, c, size, 0);
		free(c);
	}
	close(sock_fd);
	return(0);
}
