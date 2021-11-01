#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#define PORT 8888

using namespace std;
int main()
{
    struct sockaddr_in address;
    int server_fd, new_socket, valread;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    memset(&address.sin_zero,0,sizeof(address.sin_zero));
     if (connect(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    else{
        cout<<" connection successful"<<endl;
        char buf[50]={0,};
        recv(server_fd,buf,sizeof(buf),0);
        // cout<<"Press any key"<<endl;
        // getchar();
        cout<<buf;
        char buff[256] = {0,};
        while(1)
        {
            fflush(stdin);
            cout<<"message:  ";
            cin>>buff;
            // fgets(buff,256,stdin);
            send(server_fd,buff,256,0);
            // cout<<"\nPress to get resp from server"<<endl;
            // getchar();
            memset(buff,0,sizeof(buff));
            recv(server_fd,buff,256,0);
            cout<<buff<<endl;
        }
    }
    
}