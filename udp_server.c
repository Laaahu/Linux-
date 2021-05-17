#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SERV_PORT 9999

int main(int argc, char* argv[])
{
    int lfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(lfd == -1) {
        perror("socket error");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);
    int bind_ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(bind_ret == -1) {
        perror("bind error");
        exit(1);
    }

    struct sockaddr_in clit_addr;
    socklen_t clit_addr_len;
    while(1) {
        char buf[BUFSIZ];
        char str[INET_ADDRSTRLEN];
        clit_addr_len = sizeof(clit_addr);
        ssize_t recv_n = recvfrom(lfd, buf, sizeof(buf), 0, (struct sockaddr *)&clit_addr, &clit_addr_len);
        if(recv_n == -1) {
            perror("recvfrom error");
            exit(1);
        }else if(recv_n == 0) {
            printf("client has tuened off!\n");
        }
        printf("recevied from %s port %d\n",
        inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
        ntohs(clit_addr.sin_port));
        int i;
        for(i = 0; i < recv_n; i++) {
            buf[i] = toupper(buf[i]);    
        }
        ssize_t send_n = sendto(lfd, buf, recv_n, 0, (struct sockaddr *)&clit_addr, clit_addr_len);
        if(send_n == -1) {
            perror("sendto error");
            exit(1);
        }
    }
    close(lfd);

    return 0;
}
