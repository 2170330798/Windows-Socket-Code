#include <stdio.h>
#include <stdbool.h>
#include <winsock2.h>
#include <windows.h>
#include <pthread.h>

void* send_message(void *argv)
{
    SOCKET sockfd;
    SOCKADDR_IN Server_addr;
    sockfd  = socket(AF_INET, SOCK_DGRAM, 0);

    Server_addr.sin_family = AF_INET;
    Server_addr.sin_addr.S_un.S_addr = inet_addr("172.28.0.1");
    Server_addr.sin_port = htons(2022);

    char buf[100];
    int len = sizeof(Server_addr);

    while(true)
    {
        printf("-----Client-----\n");
        printf("please input message:\n");
        scanf("%s", buf);

        int SendLen = sendto(sockfd, buf, sizeof(buf), 0, (SOCKADDR*)&(Server_addr), len);

        if(!strcmp("exit", buf))
        {
            pthread_exit(NULL);
            printf("SEND EXIT\n");
        }

        if(SendLen <= 0)
        {
            printf("send failed! %d\n",WSAGetLastError());
        }
        int RecvLen = recvfrom(sockfd, buf , 1024, 0, (SOCKADDR*)&Server_addr, &len);
        if(RecvLen > 0)
            printf("Server: %s\n",buf);
    }
    closesocket(sockfd);
    return NULL;
}

void udp_chart()
{
    pthread_t pid[2];

    pthread_create(&pid[0], NULL, send_message, NULL);

    printf("%d\n",getpid());

    pthread_join(pid[0], NULL);

}


int main()
{

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    udp_chart();

    WSACleanup();
    return 0;
}
