#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sysmacros.h>
#include <stdlib.h>

int main()
{
    /*
    if (mknod("./ttyS0", S_IFCHR | S_IRUSR | S_IWUSR, makedev(4, 64)) == -1 || 
    mknod("./fb0", S_IFCHR | S_IRUSR | S_IWUSR, makedev(29, 0)) == -1)
    {
        perror("mknod() failed");
    }
    */
    if(system("./1")==-1||system("./2")==-1||system("./3")==-1)
    {
        printf("failed");
    }
    printf("Hello\n");
    return 0;
}