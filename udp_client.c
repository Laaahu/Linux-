#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define SERV_PORT 9999

int main(int argc, char* argv[])
{
    int lfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(lfd == -1) {
        perror("sock error");
        exit(1);
    }
    struct sockaddr_in serv_addr, clit_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);
    socklen_t addrlen;
    char buf[BUFSIZ];
    while(fgets(buf, sizeof(buf), stdin) != NULL) {
        ssize_t send_n = sendto(lfd, buf, strlen(buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if(send_n == -1) {
            perror("sendto error");
            exit(1);
        }
        ssize_t recv_n = recvfrom(lfd, buf, sizeof(buf), 0, NULL, 0);
        if(recv_n == -1) {
            perror("recvfrrom error");
            exit(1);
        }
        write(STDOUT_FILENO, buf, recv_n);
        
    }
    return 0;    
}
