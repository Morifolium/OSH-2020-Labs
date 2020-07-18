#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

char *args[128];
char *arg1[128];
char *arg2[128];
char cmd[256];
int arg_len;
int h_flag;
int in_flag;
int ina_flag;
int out_flag;
int pip_flag;
int bck_flag;
int and_flag;

int main()
{
    void handle(int sig);
    void run();
    void run_dir(char *arg[128]);

    signal(2, handle);

    while (1)
    {
        printf("# ");

        fflush(stdin);
        h_flag = 1;
        fgets(cmd, 256, stdin);
        h_flag = 0;
        int i;
        for (i = 0; cmd[i] != '\n'; i++)
            ;
        cmd[i] = '\0';
        args[0] = cmd;
        for (i = 0; *args[i]; i++)
            for (args[i + 1] = args[i] + 1; *args[i + 1]; args[i + 1]++)
                if (*args[i + 1] == ' ')
                {
                    *args[i + 1] = '\0';
                    args[i + 1]++;
                    break;
                }
        args[i] = NULL;
        arg_len = i;

        pip_flag = 0;
        bck_flag = 0;
        and_flag = 0;
        for (i = 0; i < arg_len; i++)
        {
            if (strcmp(args[i], "|") == 0)
                pip_flag = 1;
            if (strcmp(args[i], "&") == 0)
                bck_flag = 1;
            if (strcmp(args[i], ";") == 0)
                and_flag = 1;
        }
        if (bck_flag == 1)
        {
            args[arg_len - 1] = NULL;
            arg_len--;
        }
        if (args[0] == NULL)
        {
            continue;
        }
        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }
        //run_dir(args);
        run();
    }
    return 0;
}

void handle(int sig)
{
    wait(NULL);
    fprintf(stderr, "\n");
    if (h_flag == 1)
        fprintf(stderr, "# ");
}

void run()
{
    void run_dir(char *arg[128]);
    void run_and();
    void run_pip(int left, int right);
    pid_t pid;
    int status;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        return;
    }
    if (pid == 0)
    {
        if (pip_flag == 0 && and_flag == 0)
            run_dir(args);
        else if (and_flag == 1)
            run_and();
        else if (pip_flag == 1)
            run_pip(0, arg_len);
        exit(0);
    }
    else
    {
        if (bck_flag == 1)
        {
            printf("[process id %d]\n", pid);
            return;
        }

        if (waitpid(pid, &status, 0) == -1)
            printf("wait error\n");
    }
    return;
}

void run_dir(char *arg[128])
{
    int i;
    int status;
    char *file;
    in_flag = 0;
    ina_flag = 0;
    out_flag = 0;
    for (i = 0; arg[i] != NULL; i++)
    {
        if (arg[i][0] == '>' && arg[i][1] == '\0')
        {
            in_flag = 1;
            file = arg[i + 1];
            arg[i] = NULL;
            break;
        }
        if (arg[i][0] == '>' && arg[i][1] == '>')
        {
            ina_flag = 1;
            file = arg[i + 1];
            args[i] = NULL;
            break;
        }
        if (arg[i][0] == '<')
        {
            out_flag = 1;
            file = arg[i + 1];
            arg[i] = NULL;
            break;
        }
    }

    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        return;
    }
    if (pid == 0)
    {
        int fp;
        if (in_flag == 1)
        {
            fp = open(file, O_CREAT | O_TRUNC | O_WRONLY);
            dup2(fp, 1);
        }
        else if (ina_flag == 1)
        {
            fp = open(file, O_CREAT | O_WRONLY | O_APPEND);
            dup2(fp, 1);
        }
        else if (out_flag == 1)
        {
            fp = open(file, O_RDONLY);
            dup2(fp, 0);
        }

        if (strcmp(arg[0], "cd") == 0)
        {
            if (arg[1])
                chdir(arg[1]);
            return;
        }
        if (strcmp(arg[0], "pwd") == 0)
        {
            char wd[4096];
            puts(getcwd(wd, 4096));
            return;
        }
        if (strcmp(arg[0], "export") == 0)
        {
            for (i = 1; arg[i] != NULL; i++)
            {

                char *name = arg[i];
                char *value = arg[i] + 1;
                while (*value != '\0' && *value != '=')
                    value++;
                *value = '\0';
                value++;
                setenv(name, value, 1);
            }
            return;
        }
        if (strcmp(arg[0], "exit") == 0)
            return;

        execvp(arg[0], arg);
        return;
    }
    {
        if (waitpid(pid, &status, 0) == -1)
            printf("wait error\n");
    }
}

void run_and()
{
    void run_dir(char *arg[128]);
    char *a[128];
    char s[] = ";";
    args[arg_len] = s;
    args[arg_len + 1] = NULL;
    int i = 0;
    int ls = 0;
    int j;
    for (i = 0; i <= arg_len; i++)
    {
        if (args[i][0] == ';')
        {
            for (j = ls; j < i; j++)
            {
                a[j - ls] = args[j];
            }
            a[j - ls] = NULL;
            ls = i + 1;
            run_dir(a);
        }
    }
    return;
}

void run_pip(int left, int right)
{
    char *a[128];
    if (left >= right)
        return;

    int pip_pos = -1;
    int i;
    for (i = left; i < right; i++)
    {
        if (strcmp(args[i], "|") == 0)
        {
            pip_pos = i;
            break;
        }
    }
    if(pip_pos==-1)
    {
        int p;
        for (p = left; p < right;p++)
        {
            a[p - left] = args[p];
        }
        a[p - left] = NULL;
        run_dir(a);
        return;
    }

    int fd[2];
    if (pipe(fd) == -1)
    {
        printf("pipe error\n");
    }
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error");
    }
    else if (pid == 0)
    {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        int p;
        for (p = left; p < pip_pos; p++)
        {
            a[p - left] = args[p];
        }
        a[p - left] = NULL;
        run_dir(a);
        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);

        if (pip_pos + 1 < right)
        {
            dup2(fd[0], 0);
            close(fd[0]);
            close(fd[1]);
            run_pip(pip_pos + 1, right);
        }
    }
    return;
}