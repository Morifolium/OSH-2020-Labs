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
#define PORT 6666

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int ready = 1;

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

    char title[20];
    char buffer[BUFF_LEN];
    char message[BUFF_LEN];
    ssize_t len;
    int pos_r = 0, pos_s = 0, flag_e = 1;

    sprintf(title, "Message%d:", d);
    /*
    pthread_mutex_lock(&mutex);
    while (ready == 0)
    {
        pthread_cond_wait(&cv, &mutex);
    }
    ready = 0;
    */
    while (1)
    {
        len = recv(cli[d].sockfd, buffer, BUFF_LEN, 0);
        if (len <= 0)
            break;

        pthread_mutex_lock(&mutex);
        while (ready == 0)
        {
            pthread_cond_wait(&cv, &mutex);
        }
        ready = 0;

        pos_r = 0;
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

        ready = 1;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&mutex);
    }
    close(cli[d].sockfd);
    /*
    ready = 1;
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mutex);
    */
    cli[d].flag = 0;
    return NULL;
}

int main()
{
    //int port = atoi(argv[1]);
    int port = PORT;
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
