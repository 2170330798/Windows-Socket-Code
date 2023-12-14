#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <locale.h>

/*
void ListFiles1(const char *dir)//查找当前所有目录
{
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    LARGE_INTEGER size1;
    hFind = FindFirstFile(dir,&findData);
    if(hFind == INVALID_HANDLE_VALUE)
    {
        printf("Faild to find first file!\n");
    }

    do
    {
        if(strcmp(findData.cFileName,".")==0 || strcmp(findData.cFileName,"..")==0)
            continue;
        if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("<Dir>: %s\n",findData.cFileName);
        }
        else
        {
            size1.LowPart = findData.nFileSizeLow;
            size1.HighPart = findData.nFileSizeHigh;
            printf(findData.cFileName,size1.QuadPart);
        }
    }
    while(FindNextFile(hFind,&findData));
    printf("Done!\n");
    FindClose(hFind);
}


void inputDir(const char *dir)
{
    char Dir[100];
    sprintf(Dir,"%s\\*.*",dir);
    ListFiles1(Dir);
}
*/

void ListFiles2(const char* dir,const char *ip)//查找所有文件
{
    FILE *fp = NULL;//文件指针
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    //LARGE_INTEGER size1;
    char dirNew[1024];
    SOCKET socket_send; //定义套接字
    SOCKADDR_IN Server_add; // 地址信息结构
    Server_add.sin_family = AF_INET;
    Server_add.sin_addr.S_un.S_addr = inet_addr(ip);//set ip address
    Server_add.sin_port = htons(5000);//set port

    printf("============Sender Server============\n");
    sprintf(dirNew,"%s\\*.*",dir);//split joint string

    hFind = FindFirstFile(dirNew,&findData);
    do
    {

        if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
                (strcmp(findData.cFileName,".")!=0) &&
                (strcmp(findData.cFileName,"..")!=0))
        {
            sprintf(dirNew,"%s\\%s",dir,findData.cFileName);
            ListFiles2(dirNew,ip);
        }
        else
        {
            if(strstr(findData.cFileName,".png"))
            {
                //connect
                sprintf(dirNew,"%s>%s\\%s",ip,dir,findData.cFileName);//split joint string
                printf("Sending File: %s\n",dirNew);
                socket_send = socket(AF_INET,SOCK_STREAM,0);
                while(connect(socket_send,(SOCKADDR*)&Server_add,sizeof(SOCKADDR)) == SOCKET_ERROR);
                fp = fopen(dirNew,"rb");
                send(socket_send,dirNew,1024,0);//send
            }
        }
        Sleep(200);
    }
    while(FindNextFile(hFind,&findData));
    fclose(fp);
    send(socket_send,"Send Over!\n",13,0);
    closesocket(socket_send);//close socket
    FindClose(hFind);//close file searching point
}

void getIP(char *ip,int length)//get current IP_Address
{
    struct hostent *host_info = gethostbyname("");
    char *p = inet_ntoa(*((struct in_addr *)(*host_info->h_addr_list)));
    strncpy(ip,p,length-1);
    ip[length-1] = '\0';
}

int main()
{
    WSADATA wsaData;//library information
    WSAStartup(MAKEWORD(2,2),&wsaData);
    setlocale(LC_ALL,"Chinese");
    char IP[100] = {0};
    getIP(IP,sizeof(IP));
    ListFiles2("C://Users//（TJX-love-LJQ）//Pictures//Screenshots",IP);
    WSACleanup();//close library
    return 0;
}

