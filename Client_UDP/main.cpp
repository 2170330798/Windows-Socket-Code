#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>



int main()
{
    system("mkdir video");
    FILE *fp = fopen("video//test.png","wb");
    if(fp == NULL)
        printf("can't open\n");
    else
        printf("opened\n");

    fclose(fp);
    return 0;
}




