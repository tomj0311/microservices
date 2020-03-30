#include <stdio.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

int main()
{
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
 
    WSAStartup(MAKEWORD(2,0), &WSAData);
    server = socket(AF_INET, SOCK_STREAM, 0);
 
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // replace the ip with your futur server ip address. If server AND client are running on the same computer, you can use the local ip 127.0.0.1
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5555);
    
    int req_result;
    req_result = connect(server, (SOCKADDR *)&addr, sizeof(addr));
    if (req_result > 0){
        printf("Connected to server!\n");
    } else {
        printf("Can't connect\n");
        return -1;
    }

    char buffer[1024];

    FILE *f;
    f = fopen("test.txt", "r");

    if (f == NULL){
        printf("Bad file\n");
        return 1;
    }

    // size_t nread = fread(buffer, 1, sizeof(buffer), f);
    int ntimes = 0;
    while(!feof(f)){
        ntimes++;
        fread(buffer, sizeof(buffer), 1, f); 
        send(server, buffer, sizeof(buffer), 0);
    }

    fclose(f);

    printf("File sent \n");  

    closesocket(server);
    WSACleanup();
    printf("Socket closed.");
}