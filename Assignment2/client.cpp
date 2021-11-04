#include "headers.h"
using namespace std;

void *FromTracker(void *);
void *ToTracker(void *);
sem_t x;
unordered_map<string, vector<string>> pendingReq;
struct arg_struct
{
    int arg1;
    string arg2;
    int cliport;
    string cliip;
};
struct arg_struct_cli
{
    int arg1;
    int arg2;
};
struct clientDetails
{
    int socketId;
    int port;
    string ip;
};
int check(int exp, const char *msg)
{
    if (exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}
int convertToInt(string st)
{
    stringstream con(st);
    int x = 0;
    con >> x;
    return x;
}
void getPortandIp(char *argv[], vector<string> &trackerdetails, int &port, string &ip)
// void getPortandIp(string argv[], vector<string> &trackerdetails, int &port, string &ip)
{
    FILE *fp;
    fp = fopen(argv[2], "r"); //for command line arguments
    // fp = fopen((const char *)argv[2].c_str(), "r"); // for debugging purpose
    if (fp)
    {
        char c;
        string p = "";
        for (char c = getc(fp); c != EOF; c = getc(fp))
        {
            if (c == ' ')
            {
                trackerdetails.push_back(p);
                p = "";
                continue;
            }
            p = p + c;
        }
        trackerdetails.push_back(p);
        p = "";
        fclose(fp);
    }
    else
    {
        cout << "Unable top open file" << endl;
        exit(-1);
    }
    string p = "";
    string agv = argv[1];
    for (int i = 0; i < agv.size(); i++)
    {
        if (argv[1][i] == ':')
        {
            ip = p;
            p = "";
            continue;
        }
        p = p + argv[1][i];
    }
    port = convertToInt(p);
}
void getConnection(int port, string ip)
{
    int client_socd;
    struct sockaddr_in address;
    int opt = 1;
    check((client_socd = socket(AF_INET, SOCK_STREAM, 0)), "socket of peer failed");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    memset(&address.sin_zero, 0, sizeof(address.sin_zero));
    check((connect(client_socd, (struct sockaddr *)&address, sizeof(address))), "connection with peer failed");
    char data[1024] = {
        0,
    };
    // cout<<"line 88"<<endl;
    recv(client_socd, data, 1024, 0);
    // cout<<"line 89"<<endl;
    cout << data << endl;
    string d;
    getline(cin, d);
    strcpy(data, d.c_str());
    data[1024] = {
        0,
    };
    send(client_socd, data, 1024, 0);
}
void *FromTracker(void *arguments)
{
    struct clientDetails *args = (struct clientDetails *)arguments;
    int client_socd = args->socketId;
    while (1)
    {
        char data[1024] = {
            0,
        };
        recv(client_socd, data, 1024, 0);
        cout << data << endl;
        // processing received data
        vector<string> received;
        istringstream sst(data);
        string inter;
        while (sst >> inter)
        {
            received.push_back(inter);
        }
        if (received[0] == "peer")
        {
            string uidReq = received[1];
            string gidReq = received[received.size() - 1];
            sem_wait(&x);
            pendingReq[gidReq].push_back(uidReq);
            sem_post(&x);
        }
        string dumm = data;
        if (dumm == "2001")
        {
            cout << "result" << dumm << endl;
            getConnection(2001, "127.0.0.1");
        }
    }
    pthread_exit(NULL);
}
void *ToTracker(void *arguments)
{
    struct clientDetails *args = (struct clientDetails *)arguments;
    int client_socd = args->socketId;
    while (1)
    {
        cout << "================================" << endl
             << "Enter Commands:" << endl
             << "--------------------------------" << endl
             << setw(15) << left << "create_user"
             << "<uid> <pwd>" << endl
             << setw(15) << left << "login"
             << "<uid> <pwd>" << endl
             << setw(15) << left << "create_group"
             << "<gid>" << endl
             << setw(15) << left << "join_group"
             << "<gid>" << endl
             << setw(15) << left << "list_requests"
             << "<gid>" << endl
             << setw(15) << left << "accept_request"
             << "<gid> <uid>" << endl
             << setw(15) << left << "list_groups" << endl
             << setw(15) << left << "leave_group"
             << "<gid>" << endl
             << setw(15) << left << "logout" << endl
             << "================================" << endl;
        char data[1024] = {
            0,
        };
        string inpFromUser;
        getline(cin, inpFromUser);
        // coverting string into words
        vector<string> command;
        istringstream ss(inpFromUser);
        string intermediate;
        while (ss >> intermediate)
        {
            command.push_back(intermediate);
        }
        if (command[0] == "login")
        {
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        else if (command[0] == "create_group")
        {
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        else if (command[0] == "join_group")
        {
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        else if (command[0] == "leave_group")
        {
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        else if (command[0] == "list_requests")
        {
            string gid = command[1];
            sem_wait(&x);
            vector<string> reqlist = pendingReq[gid];
            sem_post(&x);
            for (auto pai : reqlist)
            {
                cout << pai << endl;
            }
            continue;
        }
        else if (command[0] == "accept_request")
        {
            string gid = command[1];
            string uid = command[2];
            sem_wait(&x);
            for (auto it = pendingReq[gid].begin(); it != pendingReq[gid].end(); ++it)
            {
                if (*it == uid)
                {
                    pendingReq[gid].erase(it);
                    break;
                }
            }
            sem_post(&x);
        }
        else if (command[0] == "logout")
        {
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        cout << "sending " << inpFromUser << endl;
        strcpy(data, inpFromUser.c_str());
        send(client_socd, data, 1024, 0);
    }
    pthread_exit(NULL);
}
void *establishConnectionTracker(void *arguments)
{

    struct arg_struct *args = (struct arg_struct *)arguments;
    int port = args->arg1;
    string ip = args->arg2;
    int client_socd;
    struct sockaddr_in address;
    int opt = 1;
    check((client_socd = socket(AF_INET, SOCK_STREAM, 0)), "socket failed");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    memset(&address.sin_zero, 0, sizeof(address.sin_zero));
    check((connect(client_socd, (struct sockaddr *)&address, sizeof(address))), "connection with tracker failed");
    char buf[1024] = {
        0,
    };

    pthread_t fromTracker, toTracker;
    struct clientDetails cd;
    cd.socketId = client_socd;
    cd.ip = args->cliip;
    cd.port = args->cliport;

    pthread_create(&fromTracker, NULL, FromTracker, (void *)&cd);
    pthread_create(&toTracker, NULL, ToTracker, (void *)&cd);

    pthread_join(fromTracker, NULL);
    pthread_join(toTracker, NULL);

    pthread_exit(NULL);
}
void *acceptConnection(void *arguments)
{
    struct arg_struct_cli *args = (struct arg_struct_cli *)arguments;
    int port = args->arg1;
    int clientSocD = args->arg2;
    // cout << "line154" << endl;
    send(clientSocD, "peer connection success", 1024, 0);
    // cout<<"line 159"<<endl;
    while (1)
    {
        char data[1024] = {
            0,
        };
        // cout<<"line 165"<<endl;
        int nRet = recv(clientSocD, data, 1024, 0);
        if (nRet == 0)
        {
            cout << "something happened closing connection" << endl;
            close(clientSocD);
        }
        cout << data << endl;
        string p = "";
        vector<string> command;
        istringstream ss(data);
        string intermediate;
        while (ss >> intermediate)
        {
            command.push_back(intermediate);
        }
        // for (int i = 0; i < command.size(); i++)
        //     cout << command[i];
    }
    pthread_exit(NULL);
}
void startListening(int port, string ip1, int &server_socd, struct sockaddr_in &address)
{

    int opt = 1;
    check((server_socd = socket(AF_INET, SOCK_STREAM, 0)), "socket failed");

    // Forcefully attaching socket to the port
    check(setsockopt(server_socd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)), "setsockopt eeror");
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip1.c_str());
    address.sin_port = htons(port);
    // Forcefully attaching socket to the port

    check(bind(server_socd, (struct sockaddr *)&address, sizeof(address)), "bind failed");
    check(listen(server_socd, 3), "listen error");

    cout << "waiting for connection at " << port << endl;
}
void covertAsServer(int cliport, string ip)
{
    pthread_t tid[60];
    int server_socd;
    struct sockaddr_in address;
    startListening(cliport, ip, server_socd, address);
    int i = 0;
    while (1)
    {
        int clientSocD = 0;
        socklen_t len = sizeof(struct sockaddr);
        // cout<<"line 210"<<endl;
        check((clientSocD = accept(server_socd, (struct sockaddr *)&address, &len)), "accept error");
        struct arg_struct_cli args;
        args.arg1 = cliport;
        args.arg2 = clientSocD;
        // cout<<"line 215"<<endl;
        check(pthread_create(&tid[i++], NULL, acceptConnection, (void *)&args), "Failed to create thread");
        // cout<<"line 217"<<endl;
        if (i >= 50)
        {
            i = 0;
            while (i < 50)
            {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
    }
}
int main(int argc, char *argv[])
// int main()
{
    // int argc = 4;
    // string argv[] = {"./client", "127.0.0.1:2000", "tracker_file.txt"};
    if (argc < 3)
    {
        cout << "Insufficiet command line arguments" << endl;
        exit(-1);
    }
    sem_init(&x, 0, 1);
    vector<string> tracker_details;
    int port;
    string ip;
    getPortandIp(argv, tracker_details, port, ip);
    int tracker_port = convertToInt(tracker_details[1]);
    pthread_t peerToTracker;
    struct arg_struct args;
    args.arg1 = tracker_port;
    args.arg2 = tracker_details[0];
    args.cliip = ip;
    args.cliport = port;
    check(pthread_create(&peerToTracker, NULL, establishConnectionTracker, (void *)&args), "Failed to create thread");
    covertAsServer(port, ip);
    sleep(2);
    return 0;
}