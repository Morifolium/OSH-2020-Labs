#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFF_LEN 10
#define MS_LEN 102

struct Pipe
{
    int fd_send;
    int fd_recv;
};

void *handle_chat(void *data)
{
    struct Pipe *pipe = (struct Pipe *)data;
    char title[] = "Message:";
    char message[MS_LEN];
    char buffer[BUFF_LEN];
    ssize_t len;
    int pos_r = 0, pos_s = 0, flag_e = 1;
    while ((len = recv(pipe->fd_send, buffer, BUFF_LEN, 0)) > 0)
    {
        pos_r = 0;
        while (pos_r < len)
        {
            message[pos_s] = buffer[pos_r];
            if (buffer[pos_r] == '\n')
            {
                if (flag_e == 1)
                    send(pipe->fd_recv, title, 8, 0);
                send(pipe->fd_recv, buffer, pos_s + 1, 0);
                pos_s = -1;
                flag_e = 1;
            }
            pos_s++;
            pos_r++;
        }
        if (pos_r == len && buffer[len - 1] != '\n')
        {
            if (flag_e == 1)
                send(pipe->fd_recv, title, 8, 0);
            send(pipe->fd_recv, buffer, pos_s, 0);
            pos_s = 0;
            flag_e = 0;
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int port = atoi(argv[1]);
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
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind");
        return 1;
    }
    if (listen(fd, 2))
    {
        perror("listen");
        return 1;
    }
    int fd1 = accept(fd, NULL, NULL);
    int fd2 = accept(fd, NULL, NULL);
    if (fd1 == -1 || fd2 == -1)
    {
        perror("accept");
        return 1;
    }
    pthread_t thread1, thread2;
    struct Pipe pipe1;
    struct Pipe pipe2;
    pipe1.fd_send = fd1;
    pipe1.fd_recv = fd2;
    pipe2.fd_send = fd2;
    pipe2.fd_recv = fd1;
    pthread_create(&thread1, NULL, handle_chat, (void *)&pipe1);
    pthread_create(&thread2, NULL, handle_chat, (void *)&pipe2);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}
