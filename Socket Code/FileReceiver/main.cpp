#include <stdio.h>
#include <string.h>
#include <winsock2.h> //引入SOCK

int main()
{
    //char destiFile[1024] = "E:/CEasyX/FileReceiver/recv.png";
    char Receivebuf[1024];//接收缓冲区
    //int ReceiveLen;//接收数据长度
    int Length;//表示SOCKADDR大小

    SOCKET socket_server;//定义服务器套接字
    SOCKET socket_receive;//定义用于连接的套接字

    SOCKADDR_IN Server_add;  //服务器地址信息结构
    SOCKADDR_IN Client_add;  //客户端地址信息结构

    WSADATA wsaData; //库版本信息结构
    int error;//表示错误

    printf("============Accept Server============\n");
    error = WSAStartup(MAKEWORD(2,2),&wsaData);

    if(error != 0)
    {
        printf("INIT Error!\n");
        return 0;
    }

    if(LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
    {
        WSACleanup();
        return 0;
    }

    Server_add.sin_family=AF_INET;
    Server_add.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    Server_add.sin_port=htons(5000);

    socket_server=socket(AF_INET,SOCK_STREAM,0);
    if(bind(socket_server,(SOCKADDR*)&Server_add,sizeof(SOCKADDR))==SOCKET_ERROR)
    {
        printf("BIND ERROR!\n");
    }

    if(listen(socket_server,5)<0)
    {
        printf("LISTEN ERROR!\n");
    }

    Length = sizeof(SOCKADDR);

    printf("Connect Successfully!\n");
    printf("Start to receive!\n");
    while(1)
    {
        do
        {   //accept request
            socket_receive=accept(socket_server,(SOCKADDR*)&Client_add,&Length);
        }
        while(socket_receive == (SOCKET)SOCKET_ERROR);
        //fp = fopen(destiFile,"a+");
        recv(socket_receive,Receivebuf,1024,0);
        printf("From : %s\n",Receivebuf);
    }
    closesocket(socket_receive);
    closesocket(socket_server);
    WSACleanup();

    return 0;
}

