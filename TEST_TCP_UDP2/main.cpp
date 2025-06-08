#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUFFERSIZE 100
#define BUFROWS    5

struct _SOCKET
{

    int RecvLen;
    int SendLen;
    char **Buf;
    /**
    *   BUF[0]: client_ip
    *   BUF[1]: client_user_name
    *   BUF[2]: port
    *   BUF[3]: message
    *   BUF[4]: break info 0x0000  / connect info 0x0001
    *   BUF[5]: ...
    **/

    /**WINSOCKET**/
    WORD wVersion;
    WSADATA wsaData;

    /**UDP**/
    SOCKET Usocket;
    SOCKADDR_IN UServer;
    /**TCP**/
    long connect_sum;
    SOCKET Tsocket;
    SOCKET Trsocket;
    SOCKADDR_IN TServer;
    SOCKADDR_IN TClient;

};

struct _SOCKET *msocket;


void int_to_char(int num, char *str)
{
    long temp = num;
    int i = 0;
    while(temp)
    {
        i++;
        temp /= 10;
    }
    i--;

    *(str+i) = '\0';
    while(num)
    {
        *(str+i) = '0'+(num % 10);
        num /= 10;
        i--;
    }
}

void get_local_ip(char *ip)
{
    char *getip = NULL;
    struct hostent *pHostInfo = gethostbyname(NULL);
    for(int i = 0; NULL != pHostInfo&& NULL != pHostInfo->h_addr_list[i]; ++i)
    {
        getip = inet_ntoa(*(struct in_addr *)pHostInfo->h_addr_list[i]);
    }
    strcpy(ip,getip);
}


void init_tcp(int port)
{
    get_local_ip(msocket->Buf[0]);
    printf("IPV4: %s\n",msocket->Buf[0]);
    strcpy(msocket->Buf[1],"ArcticFox");
    strcpy(msocket->Buf[4],"0x0001");
    int_to_char(port, msocket->Buf[2]);
    msocket->TServer.sin_family=AF_INET;
    msocket->TServer.sin_addr.S_un.S_addr=inet_addr(msocket->Buf[0]);
    msocket->TServer.sin_port=htons(port);

    msocket->Tsocket=socket(AF_INET,SOCK_STREAM,0);

    /******/
    printf("tcp initial successfully\n");
}

void init_udp(int port)
{
    get_local_ip(msocket->Buf[0]);
    msocket->UServer.sin_family = AF_INET;
    msocket->UServer.sin_addr.S_un.S_addr = inet_addr(msocket->Buf[0]);
    msocket->UServer.sin_port = htons(port);

    msocket->Usocket = socket(AF_INET, SOCK_DGRAM, 0);

    /****/
    printf("udp initial successfully\n");
}

void init_protocol(int tport, int uport)
{
    init_tcp(tport);
    init_udp(uport);
}

void init_socket(int tport, int uport)
{
    msocket = (struct _SOCKET*)malloc(sizeof(struct _SOCKET));
    msocket->connect_sum = 0;
    msocket->Buf = (char**)malloc(sizeof(char*)*BUFROWS);
    for(int i=0; i<BUFROWS; i++)
        msocket->Buf[i] = (char*)malloc(sizeof(char)*BUFFERSIZE);

    msocket->wVersion = MAKEWORD(2,2);
    printf("============Client============\n");
    int reError = WSAStartup(msocket->wVersion,&msocket->wsaData);

    if(reError != 0)
    {
        printf("INIT ERROR!\n");
        return ;
    }

    if(LOBYTE(msocket->wsaData.wVersion)!=2 || HIBYTE(msocket->wsaData.wVersion)!=2)
    {
        printf("INIT ERROR!\n");
        WSACleanup();
        return ;
    }

    init_protocol(tport, uport);
}

void try_connect_to_clients()
{
    int len = sizeof(SOCKADDR);
    printf("START TO CONNECT!\n");

    while(connect(msocket->Tsocket,(SOCKADDR*)&msocket->TServer,len));

    for(int i=0; i<BUFROWS; i++)
    {
        msocket->SendLen = send(msocket->Tsocket, msocket->Buf[i], BUFFERSIZE, 0);
    }
    printf("CONNECT SUCCESSFULLY!\n");

}

void send_infomation()
{
    int len = sizeof(msocket->UServer);
    printf("-----Server-------\n");

    while (true)
    {
        printf("waiting...\n");
        printf("input message:\n");
        scanf("%s",msocket->Buf[3]);
        msocket->SendLen = sendto(msocket->Usocket, msocket->Buf[3], BUFFERSIZE, 0, (SOCKADDR*)&msocket->UServer, len);

        if(!strcmp("0x0000", msocket->Buf[3]))
        {
            sendto(msocket->Usocket, msocket->Buf[3], BUFFERSIZE, 0, (SOCKADDR*)&msocket->UServer, len);
            printf("SEND EXIT\n");
            break;
        }

        if(msocket->SendLen <= 0)
        {
            printf("send failed! %d\n",WSAGetLastError());
        }
        msocket->RecvLen = recvfrom(msocket->Usocket, msocket->Buf[3], BUFFERSIZE, 0, (SOCKADDR*)&msocket->UServer, &len);
        if(msocket->RecvLen > 0)
            printf("Server: %s\n",msocket->Buf[3]);

    }
    //关闭socket
    closesocket(msocket->Tsocket);
    closesocket(msocket->Trsocket);
    closesocket(msocket->Usocket);
    if(msocket->connect_sum > 0)
        msocket->connect_sum--;
}


void free_socket()
{
    for(int i=0; i<BUFROWS; i++)
        free(msocket->Buf[i]);
    free(msocket);
    printf("FREE SUCCESSFULLY!\n");
    WSACleanup();
}


int main()
{
    init_socket(2022, 2023);
    try_connect_to_clients();
    send_infomation();
    free_socket();
    return 0;
}
