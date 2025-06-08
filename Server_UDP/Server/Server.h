#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

/**SIZE**/
#define MAX_NAME_LENGTH 100
#define MAX_IP_LENGTH  33
#define MAX_ID_LENGTH  11
#define MAX_PSW_LENGTH 12
#define MAX_SEND_SIZE1  1024 //1KB
#define MAX_SEND_SIZE2  4*1024 //4KB
#define MAX_RECV_SIZE1  1024
#define MAX_RECV_SIZE2  4*1024

/**FILE STATUS**/
#define OPEN_ERROR1  100
#define OPENED1      200
#define OPEN_ERROR2  300
#define OPEN_RD "r"
#define OPEN_WD "w+"

/**Server**/
#define RUN_TIME_LENGTH 70
#define STATUS1  100 //停止服务
#define STATUS2  200 //运行可访问
#define STATUS3  300 //维护中

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <winsock2.h>

struct Server
{
    char *id;//[MAX_ID_LENGTH];//服务器id
    char *password;//[MAX_PSW_LENGTH];//服务器密码
    char *run_time;//[RUN_TIME_LENGTH];//服务器运行时间
    char *ip;//[MAX_IP_LENGTH];//服务器IP地址
    int  status;//服务器状态
    struct Server *next;

};


struct User
{
    char *name;//[MAX_NAME_LENGTH];
    char *ip;//[MAX_IP_LENGTH];
    char *id;//[MAX_ID_LENGTH];
    char *password;//[MAX_PSW_LENGTH];

};//User结构体

struct TCP{

    int RecvLen;
    int SendLen;
    char *Buf;

    SOCKET socket_server;//定义服务器套接字
    SOCKET socket_receive;//定义用于连接的套接字

    SOCKADDR_IN Server_addr;  //服务器地址信息结构
    SOCKADDR_IN Client_addr;  //客户端地址信息结构

};

struct UDP{

    int RecvLen;
    int SendLen;
    char *Buf;
    SOCKET sockfd; //= socket(AF_INET, SOCK_DGRAM, 0);
    SOCKADDR_IN Server_addr;
    SOCKET socket_server;//定义服务器套接字

    SOCKADDR_IN Server_add;  //服务器地址信息结构
    //Server_addr.sin_family = AF_INET;
    //Server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    //Server_addr.sin_port = htons(2024);
};

void save_info();
void get_ip(char* ip);
FILE* open_files(const char* file_name, const char* model);
void set_server();
void get_cur_run_time(char *run_time);
void init_server();
void init_user_table();
void init_user_data();
void send_info_by_tcp();
void send_info_by_udp();
void get_local_ip();
void get_hostname_ip(const char* hostname);
void init_tcp(int port);
void init_udp(int port);
void free_server();
void free_user();
void free_socket();

#endif // SERVER_H_INCLUDED
