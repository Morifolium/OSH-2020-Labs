#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CON 32
#define BUFF_LEN 1024
#define PORT 6666

struct client
{
    int flag = 0;
    int sockfd;
    struct in_addr in;
    int port;
} cli[MAX_CON];

char buffer[BUFF_LEN];
char message[BUFF_LEN];
char title[20];

int main()
{
    int fd;
    int port = PORT;
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

    fd_set clients;
    int max_fd = 0, cli_fd;
    struct sockaddr_in cli_addr;
    int len;
    int pos_r = 0, pos_s = 0, flag_e = 1;
    while (1)
    {
        max_fd = 0;
        FD_ZERO(&clients);
        FD_SET(fd, &clients);
        if (max_fd < fd)
            max_fd = fd;
        for (int i = 0; i < MAX_CON; i++)
        {
            if (cli[i].flag == 1)
            {
                FD_SET(cli[i].sockfd, &clients);
                if (max_fd < cli[i].sockfd)
                    max_fd = cli[i].sockfd;
            }
        }
        if (select(max_fd + 1, &clients, NULL, NULL, NULL) > 0)
        {
            if (FD_ISSET(fd, &clients))
            {
                cli_fd = accept(fd, (struct sockaddr *)&cli_addr, &addr_len);
                if (cli_fd == -1)
                {
                    perror("accept");
                    return 1;
                }
                for (int j = 0; j < MAX_CON; j++)
                {
                    if (cli[j].flag == 0)
                    {
                        cli[j].flag = 1;
                        cli[j].in = cli_addr.sin_addr;
                        cli[j].port = ntohs(cli_addr.sin_port);
                        cli[j].sockfd = cli_fd;
                        fcntl(cli[j].sockfd, F_SETFL, fcntl(cli[j].sockfd, F_GETFL, 0) | O_NONBLOCK);
                        break;
                    }
                }
            }
            for (int j = 0; j < MAX_CON; j++)
            {
                if (cli[j].flag)
                {
                    if (FD_ISSET(cli[j].sockfd, &clients))
                    {
                        len = recv(cli[j].sockfd, buffer, 1000, 0);
                        if (len > 0)
                        {
                            sprintf(title, "Message%d:", j);
                            for (int m = 0; m < MAX_CON; m++)
                            {
                                if (cli[m].flag == 1)
                                {
                                    pos_r = 0, pos_s = 0, flag_e = 1;
                                    while (pos_r < len)
                                    {
                                        message[pos_s] = buffer[pos_r];
                                        if (buffer[pos_r] == '\n')
                                        {
                                            if (flag_e == 1)
                                                send(cli[m].sockfd, title, sizeof(title), 0);
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
                                            send(cli[m].sockfd, title, sizeof(title), 0);
                                        send(cli[m].sockfd, message, pos_s, 0);
                                        pos_s = 0;
                                        flag_e = 0;
                                    }
                                }
                            }
                        }                        else
                        {
                            FD_CLR(cli[j].sockfd, &clients);
                            cli[j].flag = 0;
                            close(cli[j].sockfd);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
