#include "headers.h"
char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
using namespace std;
void *acceptConnection(void *);
template <typename T1, typename T2>
struct clientLogin
{
    T1 port;
    T2 ip;
};
unordered_map<string, string> users;
unordered_map<string, vector<string>> groups;         //[grpid]->owner,users
unordered_map<string, string> userGids;               //[user]->gid
unordered_map<string, string> fileHash;               //[filename]->hashval
unordered_map<string, vector<string>> fileOwners;     //[filename]->owners vector
unordered_map<string, pair<string, int>> portIpUsers; //user->[ip,port]
unordered_map<string, int> SocIdsUsers;

struct clientDetails
{
    int socketId;
    int port;
    string ip;
};
int convertToInt(string st)
{
    stringstream con(st);
    int x = 0;
    con >> x;
    return x;
}
string searchUser(int port, string ip)
{
    string s;
    int p;
    for (auto m : portIpUsers)
    {
        s = m.second.first;
        p = m.second.second;
        if (port == p && ip == s)
            return m.first;
    }
    return "";
}
bool create_user(string user, string password)
{
    if (users.find(user) != users.end() && users[user] == password)
    {
        return false;
    }
    users[user] = password;
    // for (auto x : users)
    // {
    //     cout << x.first << " " << x.second << endl;
    // }
    return true;
}
bool login(string u, string password, string ip, int port, int socid)
{
    if (users.find(u) != users.end() && users[u] == password)
    {
        portIpUsers[u] = make_pair(ip, port);
        SocIdsUsers[u] = socid;
        // for (auto x : portIpUsers)
        // {
        //     cout << x.first << " " << x.second.first << " " << x.second.second << endl;
        // }
        return true;
    }
    else
        return false;
}
void create_group(string gid, string ip, int port)
{
    string userId = searchUser(port, ip);
    groups[gid].push_back(userId);
    userGids[userId] = gid;
    for (auto x : groups)
    {
        cout << x.first;
        for (auto y : x.second)
        {
            cout << y << " ";
        }
        cout << endl;
    }
}
int join_group(string gid)
{
    string owner = groups[gid][0];
    int ownerSocId = SocIdsUsers[owner];
    return ownerSocId;
}
void leave_group(string gid, string ip, int port)
{
    string userId = searchUser(port, ip);
    vector<int>::iterator it;
    for (auto it = groups[gid].begin(); it != groups[gid].end(); ++it)
    {
        if (*it == userId)
        {
            groups[gid].erase(it);
            break;
        }
    }
    for (auto x : groups)
    {
        cout << x.first;
        for (auto y : x.second)
        {
            cout << y << " ";
        }
        cout << endl;
    }
}
string list_groups()
{
    string groupIds = "";
    for (auto m : groups)
    {
        groupIds = groupIds + m.first;
        // cout<<"GroupId: "<<x.first<<endl;
        // cout<<"Owner: "<<x.second[0]<<endl;
        // cout<<"Members: "
    }
    return groupIds;
}
void upload_file(string gid, string hashval, string ip, int port, string filename)
{
    string userId = searchUser(port, ip);
    fileHash[filename] = hashval;
    fileOwners[filename].push_back(userId);
}
string download_file(string gid, string filename)
{
    string port;
    string ip, hashval;
    for (string userId : fileOwners[filename])
    {
        if (userGids[userId] == gid)
        {
            ip = portIpUsers[userId].first;
            port = to_string(portIpUsers[userId].second);
            hashval = fileHash[filename];
        }
    }
    string res = ip + " " + port + " " + hashval;
    return res;
}
void *acceptConnection(void *arguments)
{
    struct clientDetails *args = (struct clientDetails *)arguments;
    int clientSocD = args->socketId;
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
        vector<string> command;
        istringstream ss(data);
        string intermediate;
        // cout << "line 54" << endl;
        while (ss >> intermediate)
        {
            command.push_back(intermediate);
        }
        // cout << "line 59" << endl;
        // cout << "command[0] " << command[0] << endl;
        if (command[0] == "create_user")
        {
            // uid,pwd
            if (create_user(command[1], command[2]))
                send(clientSocD, REGISTERED, 1024, 0);
            else
                send(clientSocD, "user already exists", 1024, 0);
        }
        else if (command[0] == "login")
        {
            // uid,pwd
            int port = convertToInt(command[4]);
            if (login(command[1], command[2], command[3], port, clientSocD))
                send(clientSocD, LOGGEDIN, 1024, 0);
            else
                send(clientSocD, "user doesn't exists", 1024, 0);
        }
        else if (command[0] == "getport")
        {
            send(clientSocD, "2001", 1024, 0);
        }
        else if (command[0] == "create_group")
        {
            int port = convertToInt(command[3]);
            string ip = command[2];
            create_group(command[1], ip, port); //command[1] = gid
        }
        else if (command[0] == "join_group")
        {
            int port = convertToInt(command[3]);
            string ip = command[2];
            int ownSocId = join_group(command[1]);
            string req = "peer " + ip + " " + to_string(port) + " " + "wants to join group" + command[1];
            char reqData[1024] = {
                0,
            };
            strcpy(reqData,req.c_str());
            send(ownSocId, reqData, 1024, 0);
        }
        else if (command[0] == "leave_group")
        {
            int port = convertToInt(command[4]);
            string ip = command[3];
            // leave_group(command[1], ip, port); //command[1] = gid
        }
        else if (command[0] == "list_groups")
        {
            string dataToSend = list_groups(); //command[1] = gid
        }

        else if (command[0] == "upload_file")
        {
            //command[1] = gid command[2]=filename command[3]= hashvaloffile
            // upload_file(command[1], command[3], ip, port, command[2]);
        }
        else if (command[0] == "download_file")
        {
            //command[1] = gid command[2]=filename
            string dataToSend = download_file(command[1], command[2]);
        }

        // cout << "line 79" << endl;
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
unsigned int change_endian(unsigned int x)
{
    unsigned char *ptr = (unsigned char *)&x;
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
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
    struct sockaddr_in address, my_addr;
    startListening(port1, ip1, server_socd, address);

    int i = 0;
    while (1)
    {
        int clientSocD = 0;
        socklen_t len = sizeof(struct sockaddr);
        check((clientSocD = accept(server_socd, (struct sockaddr *)&address, &len)), "accept error");
        struct clientDetails args;
        args.socketId = clientSocD;
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