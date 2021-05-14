#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define SERV_PORT 9999
#define EPOLLSIZE 1024

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

    int epfd = epoll_create(EPOLLSIZE);
    if(epfd == -1) {
        perror("epoll_create error");
        exit(1);
    }

    struct epoll_event tmp, ep[1024];
    tmp.events = EPOLLIN;
    tmp.data.fd = lfd;
    int epoll_ctl_ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &tmp);
    if(epoll_ctl_ret == -1) {
        perror("epoll_ctl error");
        exit(1);
    }           

    while(1) {
        int epoll_wait_ret = epoll_wait(epfd, ep, 1024, -1);
        if(epoll_wait_ret == -1) {
            perror("epoll_wait error");
            exit(1);
        }
        int n = epoll_wait_ret, i;
        for(i = 0; i < n; i++) {
            if(ep[i].data.fd == lfd) {
                struct sockaddr_in clit_addr;
                char str[INET_ADDRSTRLEN];
                socklen_t clit_addr_len = sizeof(clit_addr);
                int cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
                if(cfd == -1) {
                    perror("accept error");
                    exit(1);
                }
                printf("revecied from %s port %d\n",
                inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
                ntohs(clit_addr.sin_port));
                tmp.events = EPOLLIN;
                tmp.data.fd = cfd;
                int eopll_ctl_ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tmp);
                if(eopll_ctl_ret == -1) {
                    perror("epoll_ctl error");
                    exit(1);
                }
            }else{
                char buf[BUFSIZ];
                int read_size = read(ep[i].data.fd, buf, sizeof(buf));
                if(read_size == 0) {
                    int epoll_ctl_ret = epoll_ctl(epfd, EPOLL_CTL_DEL, ep[i].data.fd, NULL);
                    if(epoll_ctl_ret == -1) {
                        perror("epoll_ctr error");
                        exit(1);
                    }
                    close(ep[i].data.fd);
                }else if(read_size > 0) {
                    int j;
                    for(j = 0; j < read_size; j++) {
                        buf[j] = toupper(buf[j]);    
                    }
                    write(ep[i].data.fd, buf, read_size);
                }
            }    
        }

    }

    return 0;
}
