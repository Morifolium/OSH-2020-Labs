#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define MAX_CON 32
#define BUFF_LEN 1024

struct client
{
    int flag = 0;
    int sockfd;
    struct in_addr in;
    int port;
} cli[MAX_CON];

void *handle_chat(void *data)
{

    int *x = (int *)data;
    int d = *x;
    
    char title[] = "Message:";
    char buffer[BUFF_LEN];
    char message[BUFF_LEN];
    ssize_t len;
    int pos_r = 0, pos_s = 0, flag_e = 1;
    while (1)
    {
        
        len = recv(cli[d].sockfd, buffer, BUFF_LEN, 0);
        if (len <= 0)
            break;
        pos_r = 0;
        for (int m = 0; m < MAX_CON; m++)
        {
            if(cli[m].flag==1)
            while (pos_r < len)
            {
                message[pos_s] = buffer[pos_r];
                if (buffer[pos_r] == '\n')
                {
                    if (flag_e == 1)
                        send(cli[m].sockfd, title, 8, 0);
                    send(cli[m].sockfd, message, pos_s + 1, 0);
                    pos_s = -1;
                    flag_e = 1;
                }
                pos_s++;
                pos_r++;
            }
            if (pos_r == len && buffer[len - 1] != '\n')
            {
                if (flag_e == 1)
                    send(cli[m].sockfd, title, 8, 0);
                send(cli[m].sockfd, message, pos_s, 0);
                pos_s = 0;
                flag_e = 0;
            }
        }
        
    }
    /*
    char buffer[12240] = {0};
    char tmp[10240] = {0};
    int ret;
    struct timeval tv;
    struct timezone tz;
    struct tm *tt;
    int i;
    while (1)
    {
        ret = read(cli[d].sockfd, tmp, 1024);
        if (ret <= 0)
            break;
        tmp[ret] = '\0';
        gettimeofday(&tv, &tz);
        tt = localtime(&tv.tv_sec);
        sprintf(buffer, "%s @ %d:%d:%d :\n%s", inet_ntoa(cli[d].in), tt->tm_hour, tt->tm_min, tt->tm_sec, tmp);
        for (i = 0; i < MAX_CON; i++)
        {
            if (cli[i].flag == 1)
            {
                write(cli[i].sockfd, buffer, strlen(buffer));
            }
        }
    }
    */
    close(cli[d].sockfd);
    cli[d].flag = 0;
    return NULL;
}

int main()
{
    //int port = atoi(argv[1]);
    int port = 6666;
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket");
        return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    socklen_t addr_len = sizeof(addr);
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind");
        return 1;
    }
    if (listen(fd, MAX_CON))
    {
        perror("listen");
        return 1;
    }
    int cli_fd, i;
    struct sockaddr_in cli_addr;
    pthread_t tid;
    while (1)
    {
        cli_fd = accept(fd, (struct sockaddr *)&cli_addr, &addr_len);
        if (cli_fd == -1)
        {
            perror("accept");
            return 1;
        }
        for (i = 0; i < MAX_CON; i++)
        {
            if (cli[i].flag == 0)
            {
                cli[i].flag = 1;
                cli[i].in = cli_addr.sin_addr;
                cli[i].port = ntohs(cli_addr.sin_port);
                cli[i].sockfd = cli_fd;
                break;
            }
        }
        pthread_create(&tid, NULL, handle_chat, (void *)&i);
        pthread_detach(tid);
    }
    return 0;
}
