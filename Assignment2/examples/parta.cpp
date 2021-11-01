#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#define PORT 8888
using namespace std;
fd_set fr, fw, fe;
struct sockaddr_in address;
int clifd[5];
int server_fd;
void processMessage(int cliSoc)
{
    if(FD_ISSET(cliSoc, &fr))
    {
        cout << "Proceessing new message" << endl;
        cout << "fd belons to " << cliSoc << endl;
        char buf[256 + 1] = {
            0,
        };
        int nRet = recv(cliSoc, buf, 256, 0);
        if (nRet == 0)
        {
            cout << "something happened closing connection" << endl;
            close(cliSoc);
            for (int i = 0; i < 3; i++)
            {
                if (clifd[i] == cliSoc)
                    clifd[i] = 0;
            }
        }
        else
        {
            cout << "The message is" << buf << endl;
            send(cliSoc, "processed your req", 18, 0);
            cout << "----------------" << endl;
        }
    }
}
void processRequest()
{
    int cli = 0;
    if (FD_ISSET(server_fd, &fr))
    {
        int i = 0;
        socklen_t len = sizeof(struct sockaddr);
        cli = accept(server_fd, (struct sockaddr *)&address, &len);
        if (cli > 0)
        {
            cout << "Client fd   " << cli << endl;
            for (i = 0; i < 3; i++)
            {
                if (clifd[i] == 0)
                {
                    clifd[i] = cli;
                    cout << "in for loop " << clifd[i] << endl;
                    send(cli, "client connection success", 25, 0);
                    break;
                }
            }
            if (i == 3)
            {
                cout << "no space for new connection" << endl;
            }
        }
        else
        {
            cout << " accept is unsuccess" << endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            if (clifd[i] != 0)
            {
                processMessage(clifd[i]);
            }
        }
    }
}
int main()
{
    int new_socket, valread;
    int opt = 1;
    int max_sd;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int nextSD = server_fd;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (1)
    {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);
        FD_SET(server_fd, &fr);
        FD_SET(server_fd, &fe);
        max_sd = server_fd;
        for (int i = 0; i < 3; i++)
        {
            if (clifd[i] != 0)
            {
                FD_SET(clifd[i], &fr);
                FD_SET(clifd[i], &fe);
                if (clifd[i] > max_sd)
                    max_sd = clifd[i];
            }
        }
        int nRet = select(max_sd + 1, &fr, &fw, &fe, &tv);
        if (nRet > 0)
        {
            cout << "Got the connection" << endl;
            cout << max_sd << endl;
            processRequest();
        }
        else if (nRet == 0)
        {
            // cout << "Waiting for connection at port " << PORT << endl;
        }
        // sleep(2);
        // cout << (FD_ISSET(server_fd, &fr));
    }
}