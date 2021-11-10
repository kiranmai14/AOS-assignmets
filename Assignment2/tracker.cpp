#include "headers.h"
using namespace std;

void *acceptConnection(void *);

struct fileDetails
{
    string gid;
    unordered_map<string, vector<string>> fileOwners;          //[filename]->->"uid$01010101" (vector)
    unordered_map<string, pair<string, string>> sha_filenames; //[filename]->sha
};
struct filepaths
{
    string uid;
    unordered_map<string, string> filenames_paths;
};

struct clientDetails
{
    int socketId;
    int port;
    string ip;
};
unordered_map<string, string> users;                  //[uid]->[passwd]
unordered_map<string, vector<string>> groups;         //[grpid]->owner,users
unordered_map<string, string> fileHash;               //[filename]->hashval
unordered_map<string, pair<string, int>> portIpUsers; //user->[ip,port]
unordered_map<string, int> SocIdsUsers;               //[users]->socketId
unordered_map<string, vector<string>> pendingReq;
unordered_map<string, vector<string>> downloading;
unordered_map<string, vector<string>> completed;

vector<struct filepaths> filenameWithPaths;
vector<struct fileDetails> filedetails;

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
    return "-1";
}
bool create_user(string user, string password)
{
    if (users.find(user) != users.end() && users[user] == password)
    {
        return false;
    }
    users[user] = password;
    return true;
}
bool login(string u, string password, string ip, int port, int socid)
{
    if (users.find(u) != users.end() && users[u] == password)
    {
        if (portIpUsers.find(u) != portIpUsers.end())
            return false;
        portIpUsers[u] = make_pair(ip, port);
        SocIdsUsers[u] = socid;
        return true;
    }
    else
        return false;
}
void create_group(string gid, string ip, int port)
{
    string userId = searchUser(port, ip);
    groups[gid].push_back(userId);
}
int join_group(string gid, string ip, int port)
{
    string owner = groups[gid][0];
    int ownerSocId = SocIdsUsers[owner];
    string uidReq = searchUser(port, ip);
    for (string uid : groups[gid])
    {
        if (uid == uidReq)
            return -1;
    }
    pendingReq[gid].push_back(uidReq);
    return ownerSocId;
}
string accept_request(string gid, string uid, string uidOwn)
{
    string owner = groups[gid][0];
    vector<string>::iterator it;
    if (owner == uidOwn)
    {
        for (it = pendingReq[gid].begin(); it != pendingReq[gid].end(); ++it)
        {
            if (*it == uid)
            {
                pendingReq[gid].erase(it);
                break;
            }
        }
        groups[gid].push_back(uid);
        return "accepted";
    }
    else
    {
        return "you are not owner of the group";
    }
}
bool leave_group(string gid, string ip, int port)
{
    string userId = searchUser(port, ip);
    vector<string>::iterator it;
    for (it = groups[gid].begin(); it != groups[gid].end(); ++it)
    {
        if (*it == userId)
        {
            groups[gid].erase(it);
            return true;
        }
    }
    return false;
}
string list_groups()
{
    string groupIds = "";
    for (auto m : groups)
    {
        groupIds = groupIds + " " + m.first;
    }
    return groupIds;
}
void logout(string ip, int port)
{
    unordered_map<string, pair<string, int>>::iterator it;
    for (it = portIpUsers.begin(); it != portIpUsers.end(); ++it)
    {
        if ((*it).second.first == ip && (*it).second.second == port)
        {
            portIpUsers.erase(it);
            break;
        }
    }
}
string list_requests(string gid, string ip, int port)
{
    vector<string> reqlist = pendingReq[gid];
    string userId = searchUser(port, ip);
    string requests = "";
    if (userId == groups[gid][0])
    {
        for (auto pai : reqlist)
        {
            requests = pai + " " + requests;
        }
        return requests;
    }
    else
        return "You are not owner of group";
}
string getChunks(string len)
{
    long long int size = convertToInt(len);
    long long int no_of_chunks = size / CHUNK_SIZE;
    string bitmap = "";
    for (int i = 0; i < no_of_chunks; i++)
        bitmap += '1';
    if (size % CHUNK_SIZE != 0)
        bitmap += '1';
    return bitmap;
}
void upload_file(string gid, string ip, int port, string filename, string shaval, string len, string filepath)
{
    string userId = searchUser(port, ip);
    string chunkmap = "";
    string bitmap = getChunks(len);
    chunkmap = userId + "$" + bitmap;

    bool flag = 0;
    vector<struct fileDetails>::iterator it;
    for (it = filedetails.begin(); it != filedetails.end(); it++)
    {
        if ((*it).gid == gid)
        {
            flag = 1;
            (*it).fileOwners[filename].push_back(chunkmap);
            (*it).sha_filenames[filename].first = shaval;
            (*it).sha_filenames[filename].second = len;
        }
    }

    if (!flag)
    {

        struct fileDetails filed;
        filed.gid = gid;

        unordered_map<string, vector<string>> unmap;
        vector<string> v;
        v.push_back(chunkmap);
        unmap[filename] = v;
        filed.fileOwners = unmap;

        unordered_map<string, pair<string, string>> shamap;
        pair<string, string> p;
        p.first = shaval;
        p.second = len;
        shamap[filename] = p;
        filed.sha_filenames = shamap;

        filedetails.push_back(filed);
    }
    vector<struct filepaths>::iterator it2;
    bool flag2 = 0;
    for (it2 = filenameWithPaths.begin(); it2 != filenameWithPaths.end(); it2++)
    {
        if ((*it2).uid == userId)
        {
            flag2 = 1;
            cout << "line 324"
                 << "inserting";
            (*it2).filenames_paths[filename] = filepath;
        }
    }
    if (!flag2)
    {
        struct filepaths fpath;
        fpath.uid = userId;

        unordered_map<string, string> fmap;
        fmap[filename] = filepath;

        fpath.filenames_paths = fmap;
        filenameWithPaths.push_back(fpath);
    }
}
string getPath(string uid, string fname)
{
    for (struct filepaths p : filenameWithPaths)
    {
        if (p.uid == uid)
        {
            return p.filenames_paths[fname];
        }
    }
}
string download_file(string gid, string filename)
{
    string shaval = "";
    string len = "";
    string port;
    string ip;
    string allfiles = "";
    string fullpath = "";
    vector<string> ownermap;
    for (auto p : filedetails)
    {
        if (p.gid == gid)
        {
            shaval = p.sha_filenames[filename].first;
            len = p.sha_filenames[filename].second;
            for (auto chunkmap : p.fileOwners[filename])
            {
                ownermap.push_back(chunkmap);
            }
        }
    }

    for (string chunk : ownermap)
    {
        string owner = "";
        string uid = "";
        int i = 0;
        for (i = 0; i < chunk.size(); i++)
        {
            if (chunk[i] == '$')
                break;

            owner = owner + chunk[i];
        }
        if (portIpUsers.find(owner) != portIpUsers.end())
        {
            ip = portIpUsers[owner].first;
            port = to_string(portIpUsers[owner].second);
            string fpath = getPath(owner, filename);
            allfiles = fpath + "#" + ip + ":" + port + chunk.substr(i, chunk.length() - 1) + " " + allfiles;
        }
    }

    allfiles = allfiles + " " + shaval + " " + len; //uid#ip:port$01010101 ip:port$01010101 ip:port$01010101 shaval sizeoffileinbytes
    return allfiles;
}
void detailsOfChunk(string ip, string port, string gid, string filename, string chunkmap)
{
    string uid = searchUser(convertToInt(port), ip);
    bool flag = 0;
    vector<struct fileDetails>::iterator it;
    for (it = filedetails.begin(); it != filedetails.end(); it++)
    {
        if ((*it).gid == gid)
        {
            vector<string>::iterator fown;
            for (fown = (*it).fileOwners[filename].begin(); fown != (*it).fileOwners[filename].end(); ++fown)
            {
                std::string::size_type pos = (*fown).find('$');
                string up = (*fown).substr(0, pos);
                if (up == uid)
                {
                    flag = 1;
                    (*it).fileOwners[filename].erase(fown);
                    break;
                }
            }
            string val = uid + "$" + chunkmap;
            (*it).fileOwners[filename].push_back(val);
        }
    }
}
void putD(string ip, string port, string gid, string filename, string filepath)
{

    bool flag = 0;
    downloading[gid].push_back(filename);

    string userId = searchUser(convertToInt(port), ip);
    struct filepaths fpath;
    fpath.uid = userId;
    fpath.filenames_paths[filename] = filepath;
    filenameWithPaths.push_back(fpath);
}
void putC(string ip, string port, string gid, string filename)
{
    for (auto it = downloading[gid].begin(); it != downloading[gid].end(); ++it)
    {
        if ((*it) == filename)
        {
            downloading[gid].erase(it);
            break;
        }
    }
    completed[gid].push_back(filename);
}
void stop_share(string ip, string port, string gid, string filename)
{

    string userId = searchUser(convertToInt(port), ip);
    vector<struct fileDetails>::iterator it;
    for (it = filedetails.begin(); it != filedetails.end(); it++)
    {
        if ((*it).gid == gid)
        {
            vector<string>::iterator fown;
            for (fown = (*it).fileOwners[filename].begin(); fown != (*it).fileOwners[filename].end(); ++fown)
            {
                std::string::size_type pos = (*fown).find('$');
                string up = (*fown).substr(0, pos);
                if (up == userId)
                {
                    (*it).fileOwners[filename].erase(fown);
                    break;
                }
            }
        }
    }
}
string showdownloads()
{
    string val = "";
    string g = "";
    for (auto gid : downloading)
    {
        for (auto fname : gid.second)
            val = "D [" + gid.first + "] " + fname + "\n" + val;
    }
    for (auto gid : completed)
    {
        for (auto fname : gid.second)
            val = "C [" + gid.first + "] " + fname + "\n" + val;
    }
    return val;
}
string list_files(string gid, string ip, string port)
{
    string uid = searchUser(convertToInt(port), ip);
    for (auto p : groups[gid])
    {
        if (p == uid)
        {
            string val = "";
            for (auto p : filedetails)
            {
                if (gid == p.gid)
                {
                    for (auto g : p.fileOwners)
                    {
                        val = g.first + "\n" + val;
                    }
                }
            }
            return val;
        }
    }
    return "-1";
}
void *acceptConnection(void *arguments)
{
    struct clientDetails *args = (struct clientDetails *)arguments;
    int clientSocD = args->socketId;
    send(clientSocD, "client connection success\n", 25, 0);
    while (1)
    {
        char data[4096] = {
            0,
        };
        int nRet = recv(clientSocD, data, 4096, 0);
        if (nRet == 0)
        {
            cout << "connection with client is lost" << endl;
            close(clientSocD);
            pthread_exit(NULL);
        }
        vector<string> command;
        istringstream ss(data);
        string intermediate;

        while (ss >> intermediate)
        {
            command.push_back(intermediate);
        }
        if (command[0] == "create_group" || command[0] == "join_group" || command[0] == "accept_request" || command[0] == "leave_group" ||
            command[0] == "logout" || command[0] == "list_groups" || command[0] == "list_requests" || command[0] == "show_downloads" ||
            command[0] == "list_files" || command[0] == "stop_share" || command[0] == "upload_file")
        {
            int size = command.size() - 1;
            int port = convertToInt(command[size]);
            string ip = command[size - 1];
            if (searchUser(port, ip) == "-1")
            {
                send(clientSocD, "you are not logged in", 4096, 0);
                continue;
            }
        }
        if (command[0] == "create_user")
        {
            // create_user  userid  password
            if (create_user(command[1], command[2]))
                send(clientSocD, REGISTERED, 4096, 0);
            else
                send(clientSocD, "user already exists", 4096, 0);
        }
        else if (command[0] == "login")
        {
            // [0]login    [1]userid  [2]password    [3]ip  [4]port
            int port = convertToInt(command[4]);
            if (login(command[1], command[2], command[3], port, clientSocD))
                send(clientSocD, LOGGEDIN, 4096, 0);
            else
                send(clientSocD, "user doesn't exists or user already loggedin in another port", 4096, 0);
        }
        else if (command[0] == "create_group")
        {
            // [0]create_group  [1]grpid    [2]ip   [3]port
            int port = convertToInt(command[3]);
            string ip = command[2];

            create_group(command[1], ip, port); //command[1] = gid
            send(clientSocD, "create group successfully", 4096, 0);
        }
        else if (command[0] == "join_group")
        {
            // [0]join_group    [1]grpid    [2]ip   [3]port
            int port = convertToInt(command[3]);
            string ip = command[2];
            int ownSocId = join_group(command[1], ip, port);
            string uidReq = searchUser(port, ip);
            string req = "";
            if (ownSocId == -1)
            {
                char reqData[4096] = {
                    0,
                };
                req = "you are already in the group";
                strcpy(reqData, req.c_str());
                send(clientSocD, reqData, 4096, 0);
            }
        }
        else if (command[0] == "accept_request")
        {
            // [0]accept_request    [1]gid  [2]uid  [3]ip   [4]port
            int port = convertToInt(command[4]);
            string ip = command[3];
            string gid = command[1];
            string uid = command[2];
            string uiown = searchUser(port, ip);
            string dataToSend = accept_request(gid, uid, uiown);
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            if (dataToSend == "accepted")
            {
                int reqSocId = SocIdsUsers[uid];
                send(reqSocId, reqData, 4096, 0);
            }
            else
            {
                send(clientSocD, reqData, 4096, 0);
            }
        }
        else if (command[0] == "leave_group")
        {
            // [0]leave_group   [1]grpid    [2]ip   [3]port
            int port = convertToInt(command[3]);
            string ip = command[2];
            if (!(leave_group(command[1], ip, port)))
                send(clientSocD, "you are not in group", 4096, 0);
        }
        else if (command[0] == "logout")
        {
            // [0]logout
            int port = convertToInt(command[2]);
            string ip = command[1];
            logout(ip, port);
        }
        else if (command[0] == "list_groups")
        {
            string dataToSend = list_groups();
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "list_requests")
        {
            // [0]list_requests [1]gid  [2]ip   [3]port
            int port = convertToInt(command[3]);
            string ip = command[2];
            string dataToSend = list_requests(command[1], ip, port);
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "upload_file")
        {
            // [0]upload_file   [1]filename [2]gid  [3]shaval   [4]len  [5]filepath [6]ip   [7]port
            int port = convertToInt(command[7]);
            string ip = command[6];
            upload_file(command[2], ip, port, command[1], command[3], command[4], command[5]);
            char reqData[4096] = {
                0,
            };
        }
        else if (command[0] == "download_file")
        {
            // [0]download_file [1]gid  [2]filename [3]despath
            string dataToSend = download_file(command[1], command[2]);
            char reqData[4096] = {
                0,
            };
            if (dataToSend.size() == 0)
                dataToSend = "no file found";
            else
                dataToSend = "d " + dataToSend + " " + command[1] + " " + command[2] + " " + command[3]; //d uid#ip:port$111000 uid#ip:port$1111111 shaval size gid filename despath
            strcpy(reqData, dataToSend.c_str());
            cout << "sending...   " << dataToSend << endl;
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "chunk")
        {
            cout << data << endl;
            detailsOfChunk(command[1], command[2], command[3], command[4], command[5]);
        }
        else if (command[0] == "downloading")
        {
            cout << data << endl;
            putD(command[1], command[2], command[3], command[4], command[5]);
        }
        else if (command[0] == "completed")
        {
            cout << data << endl;
            putC(command[1], command[2], command[3], command[4]);
        }
        else if (command[0] == "show_downloads")
        {
            string dataToSend = showdownloads();
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "list_files")
        {
            string dataToSend = list_files(command[1], command[2], command[3]);
            if (dataToSend == "-1")
                dataToSend = "you are not in the group";
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "stop_share")
        {
            // [0]stop_share [1]gid [2]filename  [3]ip [4]port
            stop_share(command[3], command[4], command[1], command[2]);
        }
    }
    pthread_exit(NULL);
}

void getPortandIp(char *argv[], vector<string> &trackerdetails)
{
    // to open the file tracker_file.txt and assign port and ip
    FILE *fp;

    fp = fopen(argv[1], "r");
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

    check(setsockopt(server_socd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)), "setsockopt eeror");
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip1.c_str());
    address.sin_port = htons(port);

    check(bind(server_socd, (struct sockaddr *)&address, sizeof(address)), "bind failed");
    check(listen(server_socd, 3), "listen error");

    cout << "waiting for connection....." << endl;
}
unsigned int change_endian(unsigned int x)
{
    unsigned char *ptr = (unsigned char *)&x;
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}
int main(int argc, char *argv[])
{
    int port1, port2, tracker_no;
    string ip1, ip2;

    if (argc < 3)
    {
        cout << "Insufficiet command line arguments" << endl;
        exit(-1);
    }

    tracker_no = convertToInt(argv[2]);
    vector<string> tracker_details;
    getPortandIp(argv, tracker_details);
    ip1 = tracker_details[0];
    port1 = convertToInt(tracker_details[1]);
    ip2 = tracker_details[2];
    port2 = convertToInt(tracker_details[3]);

    pthread_t tid[50];
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