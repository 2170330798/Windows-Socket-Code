//socket udp 服务端
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <winsock2.h>


void shift(void *str)
{
    char *ptr = (char*)str;
    while(*ptr != '\0')
    {
        if(*ptr >= 'A' && *ptr <= 'Z')
            *ptr += ('a' - 'A');
        ptr++;
    }

}

int main()
{

    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建socket对象
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    //创建网络通信对象
    SOCKADDR_IN Server_addr;
    Server_addr.sin_family = AF_INET;
    Server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    Server_addr.sin_port = htons(2022);

    //绑定socket对象与通信链接
    SOCKET ret = bind(sockfd, (SOCKADDR*)&Server_addr, sizeof(Server_addr));
    if (ret == SOCKET_ERROR)
    {
        printf("bind\n");
        return -1;

    }

    char buf[4048] = { 0 };
    SOCKADDR_IN Client_addr;
    int len = sizeof(Client_addr);
    printf("-----Server-------\n");

    char dir[100];
    int i=1;

    printf("waiting...\n");

    sprintf(dir,"%s//recv%d.png","video",i);
    FILE *fp = fopen(dir,"wb");
    long file_size = 0;
    while (true)
    {
        printf("receiving size: %ld\n",file_size);
        long recvLen = recvfrom(sockfd, buf, 4048, 0, (SOCKADDR*)&Client_addr, &len);
        if(!strcmp("end",buf))
            break;
        file_size += recvLen;
        fwrite(buf, sizeof(char), recvLen, fp);
        if(recvLen < 0)
            break;
    }
    printf("file size: %ldMB\n",file_size/1024);
    closesocket(sockfd);
    fclose(fp);
    WSACleanup();

    return 0;
}


/*
int main()
{

    FILE *fp1 = NULL,*fp2 = NULL;
    fp1 = fopen("test1.png","rb");

    fseek(fp1, 0, SEEK_END);
    long file_size = ftell(fp1);
    rewind(fp1);
    char buf[4048] = {'\0'};

    long cur_size = 0;
    long realise_size = file_size;
    fp2 = fopen("tes2.png","wb");

    while(cur_size < file_size)
    {
        long send_size = realise_size < 1024 ? realise_size : 1024;
        long size = fread(buf,sizeof(char), send_size,fp1);
        fwrite(buf, sizeof(char), size, fp2);
        cur_size += send_size;
        realise_size -= send_size;
    }
    fclose(fp1);
    fclose(fp2);

    return 0;
}*/

