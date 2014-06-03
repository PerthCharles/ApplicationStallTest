#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#define True 1
#define False 0

void SetNonBlocking(int sock)
{
    int flags;
    if ((flags = fcntl(sock, F_GETFL, 0)) < 0) {
        flags = 0;
    }
    if (fcntl(sock, F_SETFL, flags|O_NONBLOCK) < 0) {
        perror("cannot set socket into non-blocking");
    }
}

int main(int argc, char * argv[])
{
    struct sockaddr_in S_sockaddr;
    int SocketFD;

    char *buf;
    uint32_t bufsize = atoi(argv[2]);
    uint32_t sent = 0;
    int len;

    int i;
    
    if (argc != 3) {
        printf("Usage: %s [test_case_no] buffer_size\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
   
    buf = (char *)malloc(sizeof(char)*bufsize);
    for (i = 0; i < bufsize - 1; i++) {
        buf[i] = 'A' + i%26;
    }
    buf[bufsize - 1] = '\0';

    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&S_sockaddr, 0, sizeof(S_sockaddr));
    S_sockaddr.sin_family = AF_INET;
    S_sockaddr.sin_port = htons(8077);
    S_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(SocketFD, (struct sockaddr *)&S_sockaddr, sizeof(S_sockaddr)) == -1) {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (listen(SocketFD, 10) == -1) {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    while (True) {
        int connectFD = accept(SocketFD, NULL, NULL);
        if (connectFD < 0) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }

        switch(atoi(argv[1])) {
            case 1:
		// Send a large file with Blocking IO socket
		sent = 0;
                send(connectFD, buf, bufsize, 0);
		sent = bufsize;
                break;
	    case 2:
		// Send so quick so possible with Non Blocking IO socket
		sent = 0;
        	SetNonBlocking(connectFD);

		while (sent < bufsize) {
		    len = send(connectFD, buf + sent, bufsize - sent, 0);
		    if (len > 0) {
			sent += len;
		    }
		}
		break;
	    case 3:
		// Send ONE PAGE(4K) a time with Blocking IO socket
		sent = 0;
		while(sent < bufsize) {
		    if (bufsize - sent > 4096) {
                	len = send(connectFD, buf + sent, 4096, 0);
			if (len > 0) sent += len;
		    } else {
                	len = send(connectFD, buf + sent, bufsize - sent, 0);
			if (len > 0) sent += len;
		    }
		}
		break;
	    case 4:
		// Send ONE PAGE(4K) a time with Non Blocking IO socket
		sent = 0;
        	SetNonBlocking(connectFD);

		while(sent < bufsize) {
		    if (bufsize - sent > 4096) {
                	len = send(connectFD, buf + sent, 4096, 0);
			if (len > 0) sent += len;
		    } else {
                	len = send(connectFD, buf + sent, bufsize - sent, 0);
			if (len > 0) sent += len;
		    }
		}
		break;
            default:
                printf("No corresponding test case number yet.\n");
        }

	printf("Finish sending. Sent %u bytes.\n", sent);
        close(connectFD);
    }
    free(buf);
    close(SocketFD);
    return EXIT_SUCCESS;
}





