#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <math.h>
#include <opencv2/opencv.hpp>

#define PNG ".png"
#define JPG ".jpg"
#define MAX_RECV_SIZE 1024
#define MAX_SEND_LEN 1024*4
#define FILE_PATH_LEN 200
#define FILE_NAME_LEN 100
#define FILE_TYPE_LEN 5
#define BUFFER_SIZE   100
#define CLIENT_CLOSE 0
#define CLIENT_AWAKE 1
#define CLIST_FULL   1
#define CLIST_FREE   0

using namespace cv;

struct Client
{
    char **client_info;
    /**
    *  client_info[0]: ip
    *  client_info[1]: name
    *  client_info[2]: port
    *  client_info[3]: exit "0X0000"
    */
    unsigned client_status;
    struct Client *next;
};

struct Client_List
{
    unsigned list_status;//列表状态
    unsigned client_sum; //Client总数
    char     *account;   //账号
    char     *psw;       //密码
    struct Client *info_ptr;//获取Client的信息
    struct Client_List *next;//指向下一个Client List

};

struct Client_List *clist;

struct UDP
{
    int RecvLen;
    int SendLen;
    int Result;
    char *buf;
    char file_path[FILE_PATH_LEN];
    char file_name[FILE_NAME_LEN];
    char file_type[FILE_TYPE_LEN];
    long file_size;

    FILE *fp;
    SOCKET socket;
    SOCKET socket_server;
    SOCKET bind_port;
    SOCKADDR_IN Server;
};

struct UDP *udp;

struct TCP
{

    int RecvLen;
    char *buffer;
    SOCKET socket_server;//定义服务器套接字
    SOCKET socket_recv;
    SOCKADDR_IN Server;  //服务器地址信息结构
    SOCKADDR_IN Client;

};

struct TCP *tcp;

void init_socket()
{
    WORD wVersionRequested; //字（word）：unsigned short
    WSADATA wsaData; //库版本信息结构
    int error;//表示错误

    wVersionRequested = MAKEWORD(2,2);
    printf("============Server============\n");
    error = WSAStartup(wVersionRequested,&wsaData);

    if(error != 0)
    {
        printf("INIT ERROR!\n");
        return ;
    }

    if(LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
    {
        WSACleanup();
        return ;
    }
}

void init_tcp(int port)
{
    tcp = (struct TCP*)malloc(sizeof(struct TCP));
    tcp->buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);
}

void init_udp(int port)
{
    udp = (struct UDP*)malloc(sizeof(struct UDP));
    udp->buf = (char*)malloc(sizeof(char)*MAX_SEND_LEN);
    udp->socket = socket(AF_INET, SOCK_DGRAM, 0);
    udp->RecvLen = 0;
    udp->SendLen = 0;
    udp->file_size = 0;
    strcpy(udp->file_name,"F://UDP_TRANSFORM//video_call_udp_server//video//video");
    strcpy(udp->file_type,PNG);
    //创建网络通信对象
    udp->Server.sin_family = AF_INET;
    udp->Server.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    udp->Server.sin_port = htons(port);

    udp->bind_port = bind(udp->socket, (SOCKADDR*)&udp->Server, sizeof(udp->Server));
    if (udp->bind_port < 0)
    {
        printf("BIND ERROR\n");
        return ;
    }
    printf("UDP INITIAL SUCCESSFULLY\n");
}


long char_to_int(char *str)//将字符串"12345"变成 12345
{
    int i,j,len,n,num;

    i   = 0;
    len = strlen(str);
    j   = len-1;
    num = 0;
    while(i<len)
    {
        n = *(str+i) - '0';
        num  += n*pow(10,j);
        j--;
        i++;
    }
    return num;
}

void init_client_list(int STATUS)
{
    //创建Client List
    clist = (struct Client_List*)malloc(sizeof(struct Client_List));
    clist->client_sum = 0;
    clist->list_status = STATUS;
    clist->info_ptr = NULL;
    clist->next = NULL;

}

void get_info_from_client(char **buffer)
{
    int i= 0;
    int len = sizeof(udp->Server);
    while(i < 4)
    {
        udp->Result = recvfrom(udp->socket, buffer[i], BUFFER_SIZE, 0, (SOCKADDR*)&udp->Server, &len);
        if(!strcmp(buffer[i],"0X0000"))
            break;
        i++;
    }
}

struct Client * create_client()
{
    struct Client *client = (struct Client*)malloc(sizeof(struct Client));
    client->client_info = (char**)malloc(sizeof(char*)*4);
    for(int i=0; i<4 ; i++)
    {
        client->client_info[i] = (char*)malloc(sizeof(char)*BUFFER_SIZE);
    }
    client->client_status = CLIENT_AWAKE;
    get_info_from_client(client->client_info);
    client->next = NULL;
    return client;
}

struct Client_List * create_clist_node(int sum,int STATUS)
{
    struct Client_List *cnode = (struct Client_List*)malloc(sizeof(struct Client_List));
    cnode->client_sum  = sum;
    cnode->list_status = STATUS;
    cnode->info_ptr    = create_client();
    cnode->next        = NULL;
    return cnode;
}

void insert_client_node(struct Client_List *chead, int STATUS)
{
    struct Client_List *temp = NULL;
    struct Client_List *newCNode = create_clist_node(clist->client_sum,STATUS);
    if(newCNode == NULL)
        return ;
    temp = clist;
    while(temp->next)
        temp = temp->next;
    temp->next = newCNode;
    temp = newCNode;
    temp->next = NULL;

}


void free_client_list()
{
    struct Client_List *temp = clist->next;
    struct Client_List *pm   = NULL;
    while(temp)
    {
        pm = temp->next;
        if(temp->info_ptr != NULL)
        {
            for(int i = 0; i<4; i++)
            {
                free(temp->info_ptr->client_info[i]);
            }
            free(temp->info_ptr->client_info);
        }
        free(temp->info_ptr);
        free(temp);
        temp = pm;
    }
    free(clist);
    printf("Free Successfully! \n");
}

void video_call()
{
    int i = 1;
    int Len = sizeof(udp->Server);
    namedWindow("video",WINDOW_FREERATIO);
    do
    {
        if(i > 15)
            i = 1;
        sprintf(udp->file_path,"%s%d%s",udp->file_name,i,udp->file_type);
        udp->fp = fopen(udp->file_path,"wb");
        if(udp->fp == NULL)
        {
            printf("Can't Open\n");
            return ;
        }
        else
            printf("Writing....\n");
        long recv_size = 0;
        while (true)
        {
            udp->RecvLen = recvfrom(udp->socket, udp->buf, MAX_RECV_SIZE, 0, (SOCKADDR*)&udp->Server, &Len);
            if(udp->RecvLen < 0)
            {
                printf("Received Failed\n");
                remove(udp->file_path);
                break;
            }
            else
            {
                if(!strcmp(udp->buf,"Send Over"))
                    break;
                recv_size += udp->RecvLen;
                fwrite(udp->buf, sizeof(char), udp->RecvLen, udp->fp);
            }

        }
        fclose(udp->fp);
        Mat video = imread(udp->file_path);
        if(!video.empty())
            imshow("video",video);
        waitKey(15);
        remove(udp->file_path);
        printf("Received Successfully!\n");
        i++;

    }
    while(true);
    destroyAllWindows();

}

void free_socket()
{
    free(udp->buf);
    free(udp);
    free(udp->buf);
    free(udp);
    closesocket(udp->socket);
    WSACleanup();
}

void try_to_connect(struct Client_List *chead)
{
    int len = sizeof(tcp->Server);
    tcp->socket_server=socket(AF_INET,SOCK_STREAM,0);
    int len2 = sizeof(udp->Server);
    printf("start to connect!\n");
    do
    {
        tcp->RecvLen = 0;
        tcp->Server.sin_family=AF_INET;
        tcp->Server.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
        tcp->Server.sin_port=htons(5000);
        printf("TCP INITIAL SUCCESSFULLY\n");
        if(bind(tcp->socket_server,(SOCKADDR*)&tcp->Server,sizeof(SOCKADDR))==SOCKET_ERROR)
        {
            printf("BIND ERROR!\n");
        }

        if(listen(tcp->socket_server,5)<0)
        {
            printf("LISTEN ERROR!\n");
        }
        tcp->socket_recv=accept(tcp->socket_server,(SOCKADDR*)&tcp->Client,&len);
        tcp->RecvLen = recv(tcp->socket_recv, tcp->buffer, BUFFER_SIZE, 0);
        if(tcp->RecvLen > 0 && !strcmp(tcp->buffer,"CONNECT"))
        {
            insert_client_node(chead, CLIENT_AWAKE);
            clist->client_sum++;
        }
        udp->RecvLen = recvfrom(udp->socket, udp->buf, BUFFER_SIZE, 0, (SOCKADDR*)&udp->Server, &len2);
        if(udp->RecvLen > 0)
            printf("Client: %s\n",udp->buf);
    }
    while(tcp->socket_recv != INVALID_SOCKET);
    closesocket(tcp->socket_server);
    closesocket(tcp->socket_recv);
}

void show_client_info()
{
    struct Client_List *p = clist->next;
    while(p)
    {
        for(int i=0; i<4; i++)
            printf("Client: %s\n", p->info_ptr->client_info[i]);
        p = p->next;
    }
}

void start()
{
    init_socket();
    //init_tcp(5000);
    init_udp(2022);
    init_client_list(CLIENT_AWAKE);

    while(true)
    {
        try_to_connect(clist);
        show_client_info();
        if(!clist->client_sum)
            break;
    }
    free_client_list();
    //video_call();
    free_socket();
}

int main()
{
    start();
    return 0;
}
