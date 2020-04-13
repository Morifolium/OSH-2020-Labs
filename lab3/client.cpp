#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>


void *do_thread(void *arg);
int main(void)
{
    int sd;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("get socket  fail");
        return -1;
    }
    //1.连接对应的服务器
    //connect
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10086);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret;
    ret = connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (ret != 0)
    {
        perror("connect fail");
        return -3;
    }
    printf("connect success ... \n");
    pthread_t tid;
    //创建一条线程用于接收从服务器端收到的数据
    pthread_create(&tid, NULL, do_thread, (void *)&sd);
    pthread_detach(tid);
    char buffer[1024] = {0};
    while (1)
    {
        //阻塞从标准输出读取信息到buffer
        ret = read(0, buffer, 1024);
        if (ret > 1024)
            continue;
        //按下回车后将buffer中的内容写到文件描述符
        //通过服务器转发给其它正在连接的客户端
        write(sd, buffer, ret);
    }

    return 0;
}
void *do_thread(void *arg)
{
    int *xx = (int *)arg;
    int sd = *xx;
    int ret;
    char buffer[1024] = {0};
    while (1)
    {
        //从服务器读取数据并显示在客户端上
        ret = read(sd, buffer, 1024);
        if (ret <= 0)
            break;
        buffer[ret] = '\0';
        printf("recv:%s", buffer);
    }
    return NULL;
}