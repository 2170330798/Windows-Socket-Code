#include <stdio.h>
#include <string.h>
#include <winsock2.h> //引入SOCK

void Init_Socket()
{
    WORD wVersionRequested; //字（word）：unsigned short
    WSADATA wsaData; //库版本信息结构
    int error;//表示错误

    wVersionRequested = MAKEWORD(2,2);
    printf("============Client============\n");
    error = WSAStartup(wVersionRequested,&wsaData);

    if(error != 0)
    {
        printf("INIT Error!\n");
        return ;
    }

    if(LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
    {
        WSACleanup();
        return ;
    }
}

void Communicate(const char *dir,const char *ip)
{

    long file_size = 0;
    SOCKET socket_send;
    SOCKADDR_IN Server_add;
    Server_add.sin_family=AF_INET;
    Server_add.sin_addr.S_un.S_addr=inet_addr(ip);
    Server_add.sin_port=htons(2022);
    socket_send=socket(AF_INET,SOCK_DGRAM,0);
    printf("%s\n",dir);

    FILE *fp = fopen(dir,"rb");
    fseek(fp,0,SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    char *file_data = (char*)malloc(sizeof(char)*file_size);
    if(file_data == NULL)
        printf("can't allocate\n");
    fread(file_data,sizeof(char),file_size,fp);
    fclose(fp);
    int SendLen = 0;
    int bytes_sent = 0;
    int bytes_left = file_size;

    int len = sizeof(Server_add);
    while(bytes_sent < file_size)
    {
        int bytes_to_send = bytes_left < 1024 ? bytes_left : 1024;
        SendLen = sendto(socket_send, file_data+bytes_sent, bytes_to_send, 0, (SOCKADDR*)&Server_add, len);
        //,,,0);
        if(SendLen<=0){
            sendto(socket_send, "end",4, 0, (SOCKADDR*)&Server_add,len);
            break;
        }
        bytes_sent += SendLen;
        bytes_left -= SendLen;
    }

    free(file_data);
    if(file_size <= 0)
    {
        printf("file_size: %ldKB\n",file_size/1024);
        printf("Send Failed!\n");
    }
    else
    {
        printf("file_size: %ldKB\n",file_size/1024);
        printf("Send Successfully!\n");
        //remove(dir);
    }
    closesocket(socket_send);
}



int main()
{

    Init_Socket();

    Communicate("video//test1.png", "192.168.1.9");
    WSACleanup();

    return 0;
}
