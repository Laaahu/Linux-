#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>


#define SERV_PORT 9999

void do_sigchild(int num) {
    while(waitpid(0, NULL, WNOHANG) > 0);
}


int main(int argc, char* argv[])
{
    //注册信号处理函数
    struct sigaction newact;
    newact.sa_handler = do_sigchild;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGCHLD, &newact, NULL);
    //socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket error");
        exit(1);
    }
    //bind
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERV_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bind_ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
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
    //accept
    while(1) {
        struct sockaddr_in clit_addr;
        socklen_t clit_addr_len = sizeof(clit_addr);
        int cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
        if(cfd == -1) {
             perror("accept error");
             exit(1);
        }

        int fd = fork();
        if(fd == -1) {
            perror("fork error");
            exit(1);
        }else if(fd == 0) {
            close(lfd);
            while(1) {
                char buf[1024];
                char str[50];
                int n = read(cfd, buf, 1024);
                if(n == 0) {
                    printf("clitent has truned off!\n");
                    break;
                }
                printf("received from %s port %d\n",
                inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
                ntohs(clit_addr.sin_port));
                int i;
                for(i = 0; i < n; i++) {
                    buf[i] = toupper(buf[i]);    
                }
                write(STDOUT_FILENO, buf, n);
                write(cfd, buf, n);
            }
            close(cfd);
            return 0;
        }else {
            close(cfd);
        }
    }



    return 0;
}
