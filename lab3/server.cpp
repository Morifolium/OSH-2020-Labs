#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define MAXCONNECTION 2
#define msleep(x) (usleep(x * 1000))
struct Data
{
    int live; //0  无人用   1有人用
    int sockfd;
    struct in_addr in;
    unsigned short port;
};
struct Data array[MAXCONNECTION] = {0};
void *do_thread_showconnect(void *arg);
void *do_thread_clientopt(void *arg);
int main(void)
{
    int sockfd;
    //1.获取套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("get socket fail");
        return -1;
    }
    //2.设置端口复用
    int on = 4;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    //3.绑定IP与端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10086);
    //设置为INADDR_ANY，表示监听所有的。
    addr.sin_addr.s_addr = INADDR_ANY;
    int ret;
    ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        perror("bind error");
        return -2;
    }
    //监听
    listen(sockfd, 30);
    int peersockfd;
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(struct sockaddr_in);
    struct Data tmp;
    char message[] = "too more connction , connect fail";
    int i;
    pthread_t tid;
    //创建一条线程用于显示已连接的客户端的个数
    pthread_create(&tid, NULL, do_thread_showconnect, NULL);
    pthread_detach(tid);
    while (1)
    {
        peersockfd = accept(sockfd, (struct sockaddr *)&peeraddr, &len);
        if (peersockfd < 0)
        {
            perror("accept fail");
            return -3;
        }
        tmp.sockfd = peersockfd;
        tmp.in = peeraddr.sin_addr;
        tmp.port = ntohs(peeraddr.sin_port);
        tmp.live = 1;
        for (i = 0; i < MAXCONNECTION; i++)
        {
            if (array[i].live == 0)
            {
                array[i] = tmp;
                break;
            }
        }
        //判断是否连接个数已满
        if (i == MAXCONNECTION)
        {
            write(peersockfd, message, strlen(message));
            close(peersockfd);
            continue;
        }
        //创建一条线程用于显示客户端之间互相发送的即时信息
        pthread_create(&tid, NULL, do_thread_clientopt, (void *)&i);
        pthread_detach(tid);
    }
    return 0;
}
//线程执行函数，用于显示已连接的客户端的个数。
void *do_thread_showconnect(void *arg)
{
    int i, count = 0;
    while (1)
    {
        //system("clear");
        printf("客户端连接信息:  连接人数:%d\n", count);
        count = 0;
        for (i = 0; i < MAXCONNECTION; i++)

        {
            if (array[i].live == 1)

            {
                count++;
                printf("IP:%s   port:%d \n", inet_ntoa(array[i].in), array[i].port);
            }
        }
        msleep(5000);
    }
    return NULL;
}
//线程执行函数，用于显示客户端之间互相发送的即时信息
void *do_thread_clientopt(void *arg)
{
    //转发信息
    int *xx = (int *)arg;
    int num = *xx;
    char buffer[12240] = {0};
    char tmp[10240] = {0};
    int ret;
    struct timeval tv;
    struct timezone tz;
    struct tm *tt;
    int i;
    while (1)
    {
        ret = read(array[num].sockfd, tmp, 1024);
        if (ret <= 0)
            break;
        tmp[ret] = '\0';
        gettimeofday(&tv, &tz);
        tt = localtime(&tv.tv_sec);
        sprintf(buffer, "%s @ %d:%d:%d :\n%s", inet_ntoa(array[num].in), tt->tm_hour, tt->tm_min, tt->tm_sec, tmp);
        for (i = 0; i < MAXCONNECTION; i++)
        {
            if (array[i].live == 1)
            {
                write(array[i].sockfd, buffer, strlen(buffer));
            }
        }
    }
    close(array[num].sockfd);
    array[num].live = 0;
    return NULL;
}