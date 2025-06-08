#include "Server.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

struct Server   *server;
struct User     *user;
struct UDP      *udp;
struct TCP      *tcp;

void get_local_ip()
{
    //char host[255]="";
    struct hostent *p=gethostbyname(NULL);
    if(p==0)
    {
        printf("无法获取计算机主机名及IP\n");
    }
    else
    {
        //本机IP:循环输出本机所有网卡的IP，其中包括虚拟网卡
        for(int i=0; p->h_addr_list[i]!=0; i++)
        {
            struct in_addr in;
            memcpy(&in,p->h_addr_list[i],sizeof(struct in_addr));
            printf("本机的第%d块网卡的IP为：%s\n",i+1,inet_ntoa(in));
        }
    }
}

void get_hostname_ip(const char* hostname)
{

    struct hostent *p=gethostbyname(hostname);
    if(p==0)
    {
        printf("无法获取%s的信息\n",hostname);
    }
    else
    {
        //本机IP:循环输出本机所有网卡的IP，其中包括虚拟网卡
        int i;
        for(i=0; p->h_addr_list[i]!=0; i++)
        {
            struct in_addr in;
            memcpy(&in,p->h_addr_list[i],sizeof(struct in_addr));
            printf("解析%s的第%d个IP为：%s\n",hostname,i+1,inet_ntoa(in));
        }
    }
}

void get_ip(char* ip)
{
    char host_name[MAX_NAME_LENGTH];
    gethostname(host_name, sizeof(host_name));//获取本机名
    struct hostent* host = gethostbyname(host_name);//获取IP
    if(host != NULL)
    {
        for(int i = 0; host->h_addr_list[i] != NULL; ++i)
            memcpy(ip,inet_ntoa (*(struct in_addr *)host->h_addr_list[i]),MAX_IP_LENGTH);
        //printf("%s\n",ip);

    }
}


FILE* open_files(const char* file_name, const char* model)
{
    FILE* fp = fopen(file_name,model);
    if(fp == NULL)
    {
        printf("can't open file\n");
        exit(0);
    }
    return fp;
}

void get_cur_run_time(char *run_time)
{
    time_t rawtime;
    struct tm *info;

    time( &rawtime );
    info = localtime( &rawtime );
    //printf("%s", asctime(info));
    memcpy(run_time,asctime(info),RUN_TIME_LENGTH);
}

void save_info()
{
    printf("%s%15s%15s%10d\n",server->id,server->ip,server->password,server->status);

}

void set_server()
{
    char file_name[MAX_NAME_LENGTH] = "Server.data";
    FILE *fp = open_files(file_name, OPEN_WD);

    server = (struct Server*)malloc(sizeof(struct Server));
    printf("please input server id:\n");
    scanf("%s",server->id);

    //获取IP
    get_ip(server->ip);

    printf("please input server password:\n");
    scanf("%s",server->password);

    get_cur_run_time(server->run_time);

    printf("please input server status:\n");
    scanf("%d", &server->status);
    fprintf(fp,"%s%15s%15s%10d%35s\n",server->id,server->ip,server->password,server->status,server->run_time);
    printf("set successfully\n");
    printf("Welcome %s\n",server->id);
    free(server);
    fclose(fp);
}

void init_server()
{
    char file_name[MAX_NAME_LENGTH] = "Server.data";
    FILE *fp = open_files(file_name, OPEN_RD);
    server = (struct Server*)malloc(sizeof(struct Server));

    server->id = (char*)malloc(sizeof(char)*MAX_ID_LENGTH);
    server->ip = (char*)malloc(sizeof(char)*MAX_IP_LENGTH);
    server->password = (char*)malloc(sizeof(char)*MAX_PSW_LENGTH);
    server->run_time = (char*)malloc(sizeof(char)*RUN_TIME_LENGTH);

    fscanf(fp,"%s%15s%15s%10d",server->id,server->ip,server->password,&server->status);
    fclose(fp);
}

void init_user_table()
{

}

void init_user_data()
{

}


void init_tcp(int port)
{
    tcp = (TCP*)malloc(sizeof(TCP));
    tcp->Buf = (char*)malloc(sizeof(char)*MAX_SEND_SIZE1);
    //创建socket对象
    tcp->Server_addr.sin_family=AF_INET;
    tcp->Server_addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    tcp->Server_addr.sin_port=htons(port);

    tcp->socket_server=socket(AF_INET,SOCK_STREAM,0);

    if(bind(tcp->socket_server, (SOCKADDR*)&tcp->Server_addr, sizeof(SOCKADDR))==SOCKET_ERROR)
    {
        printf("BIND ERROR!\n");
    }

    if(listen(tcp->socket_server,5)<0)
    {
        printf("LISTEN ERROR!\n");
    }
    printf("tcp initial successfully\n");
}

void send_info_by_tcp()
{
    int Len = sizeof(SOCKADDR);
    tcp->socket_receive=accept(tcp->socket_server,(SOCKADDR*)&tcp->Client_addr,&Len);
    if(tcp->socket_receive < 0)
    {
        printf("CONNECT EORROR!\n");
    }

    while(true)
    {
        tcp->RecvLen = recv(tcp->socket_receive, tcp->Buf, MAX_RECV_SIZE1, 0);
        if(tcp->RecvLen<0)
        {
            printf("ERROR!\n");
            printf("EXIT!|\n");
            break;
        }
        else
        {
            printf("client say: %s\n",tcp->Buf);
        }


        printf("please send message:\n");
        scanf("%s",tcp->Buf);
        if(strcmp("exit",tcp->Buf)==0){
            break;
        }
        tcp->SendLen = send(tcp->socket_receive,tcp->Buf,100,0);
        if(tcp->SendLen < 0)
        {
            printf("SEND ERROR!\n");
        }

    }
    free(tcp->Buf);
    closesocket(tcp->socket_receive);
    closesocket(tcp->socket_server);
}

void init_udp(int port)
{
    udp = (UDP*)malloc(sizeof(UDP));
    udp->Buf = (char*)malloc(sizeof(char)*MAX_SEND_SIZE1);
    //创建socket对象
    udp->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    //创建网络通信对象
    udp->Server_addr.sin_family = AF_INET;
    udp->Server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    udp->Server_addr.sin_port = htons(port);

    SOCKET ret = bind(udp->sockfd, (SOCKADDR*)&udp->Server_addr, sizeof(udp->Server_addr));
    if (ret < 0)
    {
        printf("bind error\n");
        return ;
    }
    printf("udp initial successfully\n");
}

void send_info_by_udp()
{
    int Len = sizeof(udp->Server_add);
    printf("-----Server-------\n");

    while (true)
    {
        printf("waiting...\n");
        udp->RecvLen = recvfrom(udp->sockfd, udp->Buf, MAX_RECV_SIZE1, 0, (SOCKADDR*)&udp->Server_add, &Len);
        if(udp->RecvLen < 0)
        {
            printf("received failed\n");
        }

        if(!strcmp("exit",udp->Buf))
        {
            printf("Client: OVER!\n");
            break;
        }
        if(udp->RecvLen > 0){
            printf("Client: %s\n", udp->Buf);
            printf("input message:\n");
            scanf("%s",udp->Buf);
            sendto(udp->sockfd, udp->Buf, sizeof(udp->Buf), 0, (SOCKADDR*)&udp->Server_add, Len);
        }
    }
    free(udp->Buf);
    closesocket(udp->sockfd);
}


void free_server()
{
    sprintf(server->id,"%s","");
    sprintf(server->ip,"%s","");
    sprintf(server->password,"%s","");
    sprintf(server->run_time,"%s","");
    server->status  = 0x0000;
    server->next = NULL;
    free(server);
    server = NULL;
}

void free_user()
{
    free(user);
}

void free_socket()
{
    free(udp);
    free(tcp);
}

