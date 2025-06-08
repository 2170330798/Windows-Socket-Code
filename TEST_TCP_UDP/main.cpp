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


void init_tcp(int port)
{
    msocket->TServer.sin_family=AF_INET;
    msocket->TServer.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    msocket->TServer.sin_port=htons(port);

    msocket->Tsocket=socket(AF_INET,SOCK_STREAM,0);

    SOCKET rest = bind(msocket->Tsocket, (SOCKADDR*)&msocket->TServer, sizeof(msocket->TServer));
    if(rest < 0)
    {
        printf("TCP BIND ERROR!\n");
    }

    if(listen(msocket->Tsocket,5)<0)
    {
        printf("TCP LISTEN ERROR!\n");
    }
    printf("tcp initial successfully\n");
}

void init_udp(int port)
{
    msocket->UServer.sin_family = AF_INET;
    msocket->UServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    msocket->UServer.sin_port = htons(port);

    msocket->Usocket = socket(AF_INET, SOCK_DGRAM, 0);

    SOCKET ret = bind(msocket->Usocket, (SOCKADDR*)&msocket->UServer, sizeof(msocket->UServer));
    if (ret < 0)
    {
        printf("UDP BIND ERROR!\n");
        return ;
    }
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
    printf("============Server============\n");
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

void show_client_info()
{
    printf("+-------Client:---------+\n");
    for(int i=0;i<BUFROWS;i++){
        if(i == 3 || i == 4)
            continue;
        printf("+%s\n",msocket->Buf[i]);
    }
    printf("+-----------------------+\n");
}


void try_connect_to_clients()
{

    int len = sizeof(SOCKADDR);
    printf("START TO CONNECT!\n");
    msocket->Trsocket = accept(msocket->Tsocket,(SOCKADDR*)&msocket->TClient,&len);
    if(msocket->Trsocket == (SOCKET)SOCKET_ERROR)
    {
        printf("CONNECT FAILED!\n");
        return ;
    }
    printf("Client info :%s\n",inet_ntoa(msocket->TClient.sin_addr));


    int i = 0;
    while(true)
    {
        if(i > 4)
            i = 0;
        sprintf(msocket->Buf[i],"%s"," ");
        msocket->RecvLen = recv(msocket->Trsocket, msocket->Buf[i], BUFFERSIZE, 0);

        if(!strcmp(msocket->Buf[4],"0x0001"))
           break;
        i++;
    }
    msocket->connect_sum++;
    printf("CONNECT SUCCESSFULLY!\n");
    //show_client_info();
}

void send_infomation()
{
    int len = sizeof(msocket->UServer);
    printf("-----Server-------\n");
    printf("waiting...\n");
    while (true)
    {
        msocket->RecvLen = recvfrom(msocket->Usocket, msocket->Buf[3], BUFFERSIZE, 0, (SOCKADDR*)&msocket->UServer, &len);
        if(msocket->RecvLen < 0)
        {
            printf("received failed\n");
        }

        if(!strcmp("0x0000",msocket->Buf[3]))
        {
            printf("Client: OVER!\n");
            break;
        }
        if(msocket->RecvLen > 0)
        {
            printf("Client: %s\n", msocket->Buf[3]);
            printf("input message:\n");
            scanf("%s",msocket->Buf[3]);
            sendto(msocket->Usocket, msocket->Buf[3], BUFFERSIZE, 0, (SOCKADDR*)&msocket->UServer, len);
            if(!strcmp(msocket->Buf[3],"0x0000"))
                break;
        }
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
