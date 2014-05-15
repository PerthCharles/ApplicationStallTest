#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define True 1
#define False 0

void SetNonBlocking(int sock)
{
    int flags;
    if ((flags = fcntl(sock, F_SETFL, 0)) < 0) {
        flags = 0;
    }
    if (fcntl(sock, F_SETFL, flags|O_NONBLOCK) < 0) {
        perror("cannot set socket into non-blocking");
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in S_sockaddr, C_sockaddr;
    char *buf;
    int bufsize, length, received_len = 0, SocketFD;
	fd_set c_fds;
	struct timeval tv;
	int sel_result;

    if (argc != 3) {
        printf("Usage: %s server-address buffer-size.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    bufsize = atoi(argv[2]);
    buf = (char *)malloc(sizeof(char)*bufsize);
    memset(buf, 0, sizeof(char)*bufsize);

    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&C_sockaddr, 0, sizeof(C_sockaddr));
    C_sockaddr.sin_family = AF_INET;
    C_sockaddr.sin_port = htons(8067);
    C_sockaddr.sin_addr.s_addr = htons(INADDR_ANY);

    memset(&S_sockaddr, 0, sizeof(S_sockaddr));
    S_sockaddr.sin_family = AF_INET;
    inet_aton(argv[1], &S_sockaddr.sin_addr);
    S_sockaddr.sin_port = htons(8077);

    if (bind(SocketFD, (struct sockaddr *)&C_sockaddr, sizeof(C_sockaddr)) == -1) {
        perror("bind failure");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (connect(SocketFD, (struct sockaddr *)&S_sockaddr, sizeof(C_sockaddr)) == -1) {
        perror("connect error");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    SetNonBlocking(SocketFD);
    while (received_len < bufsize) {
        FD_ZERO(&c_fds);
		FD_SET(SocketFD, &c_fds);
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		sel_result = select(SocketFD+1, &c_fds, NULL, NULL, &tv);
		switch(sel_result) {
			case 0:
				printf("timeout.\n");
				continue;
			case 1:
				break;
			default:
				printf("error.\n");
				continue;
		}
		//printf("select  = %d\n", sel_result);
        length = recv(SocketFD, buf, bufsize, 0);
		//printf("length = %d total = %d\n", length, received_len);
        if (length < 0) {
            perror("recv error");
            break;
        } else if (length > 0) {
         //   printf("    receive %d from server. total = %d\n", length, received_len);
            received_len += length;
        }
    }

    printf("received completed with %d bytes.\n", received_len);

    free(buf);
    close(SocketFD);
    return 0;
}

