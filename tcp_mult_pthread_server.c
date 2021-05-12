#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>

#define SERV_PORT 9998
const int maxn = 1000;

struct s_info{
    struct sockaddr_in clit_addr;
    int connfd;
};

void* do_work(void *arg) {
    char buf[BUFSIZ];
    char str[INET_ADDRSTRLEN];
    struct s_info *ts = (struct s_info *)arg;
    pthread_detach(pthread_self());
    while(1) {
        int n = read(ts->connfd, buf, sizeof(buf));
        if(n == 0) {
            printf("client has truned off!\n");
            break;
        }
        printf("received from %s port %d\n",
        inet_ntop(AF_INET, &(*ts).clit_addr.sin_addr, str, sizeof(str)),
        ntohs((*ts).clit_addr.sin_port));
        int i;
        for(i = 0; i < n; i++) {
            buf[i] = toupper(buf[i]);    
        }
        write(ts->connfd, buf, n);
    }
    close(ts->connfd);

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
    //accept
    int i = 0;
    struct s_info ts[maxn];
    while(1) {
        struct sockaddr_in clit_addr;
        socklen_t clit_addr_len = sizeof(clit_addr);
        int cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
        ts[i].clit_addr = clit_addr;
        ts[i].connfd = cfd;
        pthread_t tid;
        pthread_create(&tid, NULL, do_work, (void*)&ts[i]);
        i++;
    }


    return 0;
}
