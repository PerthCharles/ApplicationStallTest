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

int main(int argc, char * argv[])
{
    struct sockaddr_in S_sockaddr;
    int SocketFD;

    char *buf;
    int bufsize = atoi(argv[2]);
    buf = (char *)malloc(sizeof(char)*bufsize);
    for (int i = 0; i < bufsize - 1; i++) {
        buf[i] = 'A' + i%26;
    }
    buf[bufsize - 1] = '\0';

    
    if (argc != 3) {
        printf("Usage: %s [test_case_no] buffer_size\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
   
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&S_sockaddr, 0, sizeof(S_sockaddr));
    S_sockaddr.sin_family = AF_INET;
    S_sockaddr.sin_port = htons(23000);
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
        SetNonBlocking(connectFD);

        switch(atoi(argv[1])) {
            case 1:
                send(connectFD, buf, bufsize, 0);
                break;
            default:
                printf("No corresponding test case number yet.\n");
        }

        close(connectFD);
    }
    close(SocketFD);
    return EXIT_SUCCESS;
}





