#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>

#define SERV_PORT 9999
int max(int a, int b) {
    return a > b ? a : b;    
}

int main(int argc, char* argv[])
{
    //socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket error");
        exit(1);
    }
    //bind
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);
    int bind_ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(bind_ret == -1) {
        perror("bind error");
        exit(1);
    }
    //listen
    int listen_ret = listen(lfd, 128);
    if(listen_ret == -1) {
        perror("listen error");
        exit(1);
    }
    fd_set rset, allset;
    int max_fd = lfd;
    FD_ZERO(&allset);
    FD_SET(lfd, &allset);
    while(1) {
        rset = allset;
        int select_ret = select(max_fd + 1, &rset, NULL, NULL, NULL);
        if(select_ret < 0) {
            perror("select error");
            exit(1);
        }
        if(FD_ISSET(lfd, &rset)) {
            char str[INET_ADDRSTRLEN];
            struct sockaddr_in clit_addr;
            socklen_t clit_addr_len = sizeof(clit_addr);
            int cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
            FD_SET(cfd, &allset);
            printf("receviced from %s port %d\n", 
                    inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
                    ntohs(clit_addr.sin_port));
            if(cfd ==  -1) {
                perror("accpet error");
                exit(1);
            }
            max_fd = max(max_fd, cfd);
            if(0 == --select_ret) {
                continue;    
            }
        }
        int i;
        for(i = lfd + 1; i <= max_fd; i++) {
            if(FD_ISSET(i, &rset)) {
                char buf[BUFSIZ];
                int n = read(i, buf, sizeof(buf));
                printf("%d\n", n);
                if(n == 0) {
                    close(i);
                    FD_CLR(i, &allset);
                }else if(n > 0) {
                    int j;
                    for(j = 0; j < n; j++) {
                        buf[j] = toupper(buf[j]);    
                    }
                    write(STDOUT_FILENO, buf, n);
                    write(i, buf, n);                            
                }else{
                    perror("read error");
                    exit(1);
                }
            }
        }

    }
    close(lfd); 
    return 0;
}
