#include "headers.h"
char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
using namespace std;
void *acceptConnection(void *);
unordered_map<string, string> users;

struct arg_struct
{
    int arg1;
    int arg2;
};
bool create_user(string user, string password)
{
    if (users.find(user) != users.end() && users[user] == password)
    {
        return false;
    }
    users[user] = password;
    return true;
}
bool checkCredentials(string u, string password)
{
    if (users.find(u) != users.end() && users[u] == password)
    {
        return true;
    }
    else
        return false;
}
void *acceptConnection(void *arguments)
{
    struct arg_struct *args = (struct arg_struct *)arguments;
    int port = args->arg1;
    int clientSocD = args->arg2;
    send(clientSocD, "client connection success", 25, 0);
    while (1)
    {
        char data[1024] = {
            0,
        };
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
        cout <<"line 54" << endl;
        while (ss >> intermediate)
        {
            command.push_back(intermediate);
        }
         cout <<"line 59" << endl;
        cout << "command[0] " << command[0]<<endl;
        if (command[0] == "create_user")
        {
            if (create_user(command[1], command[2]))
                send(clientSocD, REGISTERED, 1024, 0);
            else
                send(clientSocD, "user already exists", 1024, 0);
        }
        else if (command[0] == "login")
        {
            if (checkCredentials(command[1], command[2]))
                send(clientSocD, LOGGEDIN, 1024, 0);
            else
                send(clientSocD, "user doesn't exists", 1024, 0);
        }
        else if (command[0] == "getport")
        {
            cout<<"sent"<<"2001"<<endl;
            send(clientSocD, "2001", 1024, 0);
        }
        cout <<"line 79" << endl;
    }
    pthread_exit(NULL);
}
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
// void getPortandIp(char *argv[])
void getPortandIp(string argv[], vector<string> &trackerdetails)
{
    // to open the file tracker_file.txt and assign port and ip
    FILE *fp;
    // fp = fopen(argv[1], "r"); //for command line arguments
    fp = fopen((const char *)argv[1].c_str(), "r"); // for debugging purpose
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

    cout << "waiting for connection....." << endl;
}
// int main(int argc, char *argv[])
int main()
{
    int argc = 3;
    string argv[] = {"./tracker", "tracker_file.txt", "1"};
    int port1, port2, tracker_no;
    string ip1, ip2;
    // for checking the arguments
    if (argc < 3)
    {
        cout << "Insufficiet command line arguments" << endl;
        exit(-1);
    }
    // tracker_no = convertToInt(argv[2]); //for command line arguments
    tracker_no = convertToInt(argv[2]); // for debugging purpose
    vector<string> tracker_details;
    getPortandIp(argv, tracker_details);
    ip1 = tracker_details[0];
    port1 = convertToInt(tracker_details[1]);
    ip2 = tracker_details[2];
    port2 = convertToInt(tracker_details[3]);

    // cout<<ip1<<" "<<port1<<" "<<ip2<<" "<<port2;
    // --------------------------------------------------------------------------------------
    pthread_t tid[60];
    int server_socd;
    struct sockaddr_in address;
    startListening(port1, ip1, server_socd, address);
    int i = 0;
    while (1)
    {
        int clientSocD = 0;
        socklen_t len = sizeof(struct sockaddr);
        check((clientSocD = accept(server_socd, (struct sockaddr *)&address, &len)), "accept error");
        struct arg_struct args;
        args.arg1 = port1;
        args.arg2 = clientSocD;
        check(pthread_create(&tid[i++], NULL, acceptConnection, (void *)&args), "Failed to create thread");
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
    return 0;
}