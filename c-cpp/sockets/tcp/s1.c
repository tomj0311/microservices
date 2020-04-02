#include <stdio.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")
 
int main()
{
    WSADATA WSAData;
 
    SOCKET server, client;
 
    SOCKADDR_IN serverAddr, clientAddr;
 
    WSAStartup(MAKEWORD(2,0), &WSAData);
    server = socket(AF_INET, SOCK_STREAM, 0);
 
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
 
    bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
    listen(server, 0);
 
    printf("Listening for incoming connections...\n");
 
    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr);
    int recvresult = 0;
    int ntimes = 0; 

    FILE *f = NULL;
    f = fopen("tasksrecd.json", "w+");

    if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
    {
        do {
           ntimes++;
           recvresult = recv(client, buffer, sizeof(buffer), 0);
           printf("%s\n", buffer);
           memset(buffer, 0, sizeof(buffer));

        } while (recvresult > 0);
         
        printf("ntimes %d\n", ntimes);
        printf("Client disconnected.\n");
        closesocket(client);
    }

    WSACleanup();
}