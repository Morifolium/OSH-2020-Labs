#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define CONSOLE "/dev/pts/0"
#define BASH "./shell2.cpp.o"

char cmd[256];
char *args[128];
char *order[128];
char *dct[128];
int s_pip = 1, s_rp, s_rt, s_end, s_p, ss_pip, ss_end;
char result[1000];
char gap[1280];
char *control[10][128];
int con = 0;
FILE *fi, *fo;
char rt[100] = "data";
int flag = 0;

int main()
{
    int copystr(char *a, char *b);
    int redirect(char *isa[128], int &i);
    int command(char *args[128]);
    int format(char *args[128]);
    void handle2(int sig);
    signal(2, handle2);
    while (1)
    {
        //truncate("data.out", 0);
        for (int j = 0; j < 10; j++)
        {
            args[j] = NULL;
        }
        flag = 1;
        int s0 = format(args);
        flag = 0;
        if (s0)
            continue;
        freopen("data.out", "w", stdout);
        do
        {
            //为pipe作准备
            {
                for (s_end = 0; args[s_end] != NULL; s_end++)
                {
                    if (strcmp(args[s_end], "|") == 0)
                        break;
                }
                int i;
                for (i = 0; i < s_end; i++)
                {
                    order[i] = (char *)malloc(100 * sizeof(char));
                    copystr(order[i], args[i]);
                }
                order[s_end] = NULL;
                ss_end = s_end;
                for (int m = 0; m < con; m++)
                {
                    if (strcmp(order[0], control[m][0]) == 0)
                    {
                        int n;
                        for (n = 0; control[m][n + 1] != NULL; n++)
                        {
                            order[i] = (char *)malloc(100 * sizeof(char));
                            copystr(order[i], control[m][n + 1]);
                        }
                        order[n] = NULL;
                        break;
                    }
                }
                for (i = 0; args[i + s_end + 1] != NULL; i++)
                {
                    args[i] = (char *)malloc(100 * sizeof(char));
                    copystr(args[i], args[i + s_end + 1]);
                }
                args[i] = NULL;
                args[i + 1] = NULL;
                for (s_end = 0; args[s_end] != NULL; s_end++)
                    ;
            }

            s_rt = redirect(order, s_rp);
            if (s_p == 1 && s_pip != ss_pip)
            {

                freopen(CONSOLE, "w", stdout);
                fi = fopen("data.out", "r");
                int num = 0, a;
                while (fscanf(fi, "%s", result) != EOF)
                {
                    dct[num] = (char *)malloc(10000 * sizeof(char));
                    copystr(dct[num], result);
                    num++;
                }
                /*
                for (a = 0; a < num; a++)
                {
                    order[s_end + a] = (char *)malloc(1000 * sizeof(char));
                    copystr(order[s_end + a], dct[a]);
                }
                order[s_end + a] = NULL;
                s_end += a;
                */
                FILE *hf = fopen("data", "w");
                for (int i = 0; dct[i] != NULL; i++)
                {
                    fprintf(hf, "%s\n", dct[i]);
                }
                order[ss_end] = (char *)malloc(100 * sizeof(char));
                copystr(order[ss_end], rt);
                ss_end++;
                order[ss_end] = NULL;
                fclose(hf);
                //truncate("data.out", 0);
                fclose(fi);
                fi = fopen("data.out", "w+");
                fclose(fi);
                freopen("data.out", "w", stdout);
            }
            if (s_rt == 3)
            {

                if (strcmp(order[s_rp + 1], "EOF") == 0)
                {
                    freopen(CONSOLE, "w", stdout);
                    FILE *hf = fopen("data", "w");
                    do
                    {
                        printf(">");
                        fgets(result, 256, stdin);
                        if(strcmp(result,"EOF\n")==0)
                            break;
                        fprintf(hf, "%s", result);
                    } while (1);
                    //order[s_rp] = NULL;
                    fclose(hf);
                    freopen("data.out", "w", stdout);
                }
                else
                {

                    fi = fopen(order[s_rp + 1], "r");
                    int num = 0, a;
                    while (fscanf(fi, "%s", result) != EOF)
                    {
                        dct[num] = (char *)malloc(1000 * sizeof(char));
                        copystr(dct[num], result);
                        num++;
                    }
                    for (a = 0; a < num; a++)
                    {
                        order[ss_end + a] = (char *)malloc(1000 * sizeof(char));
                        copystr(order[ss_end + a], dct[a]);
                    }
                    order[ss_end + a] = NULL;
                    fclose(fi);
                }
            }
            else if (s_rt == 2)
            {
                fo = fopen(order[s_rp + 1], "a+");
                fi = fopen("./data.out", "r");
            }
            else if (s_rt == 1)
            {
                fo = fopen(order[s_rp + 1], "w");
                fi = fopen("./data.out", "r");
            }
            if (s_rt)
            {
                order[s_rp] = NULL;
                order[s_rp + 1] = NULL;
            }
            if(s_rt==3)
            {
                order[s_rp] = (char *)malloc(100 * sizeof(char));
                copystr(order[s_rp], rt);
            }
            //命令实现
            //truncate("data.out", 0);
            int s1 = command(order);
            if (s1)
                return 0;

            if (s_rt == 1)
            {
                freopen(CONSOLE, "w", stdout);
                while (fscanf(fi, "%s", gap) != EOF)
                    fprintf(fo, "%s\n", gap);
                fclose(fo);
                fclose(fi);
                freopen("data.out", "r+", stdout);
            }
            else if (s_rt == 2)
            {
                freopen(CONSOLE, "w", stdout);
                while (fscanf(fi, "%s", gap) != EOF)
                    fprintf(fo, "%s\n", gap);
                fclose(fo);
                fclose(fi);
                freopen("data.out", "r+", stdout);
            }
            s_pip--;
        } while (s_pip > 0);
        freopen(CONSOLE, "w", stdout);
        FILE *fp = fopen("./data.out", "r");
        while (fscanf(fp, "%s", result) != EOF)
        {
            printf("%s\n", result);
        }
        printf("\n");
        fclose(fp);
    }
}

int format(char *ist[128])
{
    int i;
    s_pip = 1;
    fflush(stdin);
    printf("# ");
    fgets(cmd, 256, stdin);
    for (i = 0; cmd[i] != '\n'; i++)
    {
        if (cmd[i] == '^' && cmd[i + 1] == 'C')
            return 1;
        if (cmd[i] == '|')
            s_pip++;
    }
    cmd[i] = '\0';
    ist[0] = cmd;
    for (i = 0; *ist[i]; i++)
        for (ist[i + 1] = ist[i] + 1; *ist[i + 1]; ist[i + 1]++)
            if (*ist[i + 1] == ' ')
            {
                *ist[i + 1] = '\0';
                ist[i + 1]++;
                break;
            }
    ist[i] = NULL;
    ss_pip = s_pip;
    if (s_pip > 1)
        s_p = 1;
    else
        s_p = 0;
    return 0;
}

int command(char *ist[128])
{
    int copystr(char *a, char *b);
    /* 没有输入命令 */
    if (!ist[0])
        return 0;
    /* 内建命令 */
    else if (strcmp(ist[0], "cd") == 0)
    {
        if (ist[1])
            chdir(ist[1]);
        return 0;
    }
    else if (strcmp(ist[0], "pwd") == 0)
    {
        char wd[4096];
        puts(getcwd(wd, 4096));
        return 0;
    } /*
    else if (strcmp(ist[0], "wc") == 0 && s_p == 1)
    {
        int line, nas = 0;
        for (line = 1; order[line] != NULL; line++)
        {
            nas += strlen(order[line]);
        }
        printf("%d %d %d\n", line - 1, line - 1, nas + line - 1);
        return 0;
    }*/
    else if (strcmp(ist[0], "echo") == 0 && strcmp(ist[1], "$PATH") == 0)
    {
        puts(getenv("PATH"));
        return 0;
    }
    else if (strcmp(ist[0], "export") == 0)
    {
        for (int i = 1; ist[i] != NULL; i++)
        {
            /*处理每个变量*/
            char *name = ist[i];
            char *value = ist[i] + 1;
            while (*value != '\0' && *value != '=')
                value++;
            *value = '\0';
            value++;
            setenv(name, value, 1);
        }
        return 0;
    }
    else if (strcmp(ist[0], "exit") == 0)
        return 1;
    else if (strcmp(ist[0], "alias") == 0)
    {
        for (int i = 0; ist[i + 1] != NULL; i++)
        {
            control[con][i] = (char *)malloc(100 * sizeof(char));
            copystr(control[con][i], ist[i + 1]);
        }
        con++;
    }
    else
    {

        /* 外部命令 */
        pid_t pid = fork();
        if (pid == 0)
        {
            if (strcmp(ist[0], BASH) == 0)
            {
                freopen(CONSOLE, "w", stdout);
                printf("#");
            }

            /* 子进程 */
            execvp(ist[0], ist);
            /* execvp失败 */
            return 255;
        }
        /* 父进程 */
        wait(NULL);
    }
    return 0;
}
void handle2(int sig)
{

    wait(NULL);
    fprintf(stderr, "\n");
    if(flag)
        fprintf(stderr, "#");
}

int redirect(char *isa[128], int &i)
{
    for (i = 0; isa[i] != NULL; i++)
    {
        if (strcmp(isa[i], ">") == 0)
            return 1;
        else if (strcmp(isa[i], ">>") == 0)
            return 2;
        else if (strcmp(isa[i], "<") == 0)
            return 3;
    }
    return 0;
}

int copystr(char *a, char *b)
{
    int i;
    for (i = 0; b[i] != '\0'; i++)
    {
        a[i] = b[i];
    }
    a[i] = '\0';
    return 0;
}
