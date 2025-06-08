#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "Server.h"


int main()
{

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    init_udp(2022);
    free_socket();
    send_info_by_udp();


    WSACleanup();

    return 0;
}
