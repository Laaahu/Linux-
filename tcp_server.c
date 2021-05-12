#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) 
{
    //创建socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf("error socket\n");
    }
    //绑定地址结构bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bind_res = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
    if(bind_res != 0) {
        printf("error bind\n");
    }
    //设置连接上限数目listen
    int listen_res = listen(socket_fd, 10);
    if(listen_res) {
        printf("error listen\n");
    }


    while(1) {
        struct sockaddr_in clit_addr;
        socklen_t clit_addr_len = sizeof(clit_addr);
        int conn_fd = accept(socket_fd, (struct sockaddr *)&clit_addr, &clit_addr_len);
        if(conn_fd < 0) {
            printf("error accept\n");
        }
        char buf[1024];
        char str[50];
        while(1) {
            int read_res = read(conn_fd, buf, 1024);
            printf("reseived from %s at port %d\n", inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)             ),ntohs(clit_addr.sin_port));
            int i;
            for(i = 0; i < read_res; i++) {
                buf[i] = toupper(buf[i]);
            }
            printf("%s", buf);
            int write_res = write(conn_fd, buf, read_res);
            
            }
            close(conn_fd);
    }

    return 0;
    
}
