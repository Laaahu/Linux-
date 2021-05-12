#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define SERV_PORT 8888

int main(int argc, char* argv[])
{
    int clit_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(clit_fd < 0) {
        perror("socket error");
        exit(1);
    }
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr.s_addr);
    int ret = connect(clit_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if(ret < 0) {
        perror("connect error");
        exit(1);
    }

    while(1) {
        char buf[1024];
        scanf("%s", buf);
        int write_ret = write(clit_fd, buf, sizeof(buf));
        printf("%s\n", buf);
        int read_ret = read(clit_fd, buf, write_ret);
        printf("%s\n", buf);
    }
    close(clit_fd);
    return 0;
}
