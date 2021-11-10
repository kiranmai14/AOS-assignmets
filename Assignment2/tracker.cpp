#include "headers.h"


using namespace std;
void *acceptConnection(void *);
template <typename T1, typename T2>
struct clientLogin
{
    T1 port;
    T2 ip;
};
struct fileDetails
{
    string gid;
    unordered_map<string, vector<string>> fileOwners;          //[filename]->->"uid$01010101" (vector)
    unordered_map<string, pair<string, string>> sha_filenames; //[filename]->sha
    // unordered_map<string, vector<string>> filenames_paths;
};
struct filepaths
{
    string uid;
    unordered_map<string, string> filenames_paths;
};
vector<struct filepaths> filenameWithPaths;
// struct downloads
// {
//     string gid;
//     unordered_map<string, vector<string>> downloading; //filename->uid
//     unordered_map<string, vector<string>> completed;   //filename->uid
// };
struct clientDetails
{
    int socketId;
    int port;
    string ip;
};
unordered_map<string, string> users;                  //[uid]->[passwd]
unordered_map<string, vector<string>> groups;         //[grpid]->owner,users
unordered_map<string, string> userGids;               //[user]->gid
unordered_map<string, string> fileHash;               //[filename]->hashval
unordered_map<string, pair<string, int>> portIpUsers; //user->[ip,port]
unordered_map<string, int> SocIdsUsers;               //[users]->socketId
vector<struct fileDetails> filedetails;
// vector<struct downloads> downloadinfo;
unordered_map<string, vector<string>> pendingReq;
unordered_map<string, vector<string>> downloading;
unordered_map<string, vector<string>> completed;

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
    cout << "line 38" << endl;
    for (auto m : portIpUsers)
    {
        s = m.second.first;
        p = m.second.second;
        cout << s << " " << p << endl;
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
        cout << "gid " << x.first << " ";
        for (auto y : x.second)
        {
            cout << y << " ";
        }
        cout << endl;
    }
}
int join_group(string gid, string ip, int port)
{
    string owner = groups[gid][0];
    int ownerSocId = SocIdsUsers[owner];
    string uidReq = searchUser(port, ip);
    pendingReq[gid].push_back(uidReq);
    return ownerSocId;
}
string accept_request(string gid, string uid, string uidOwn)
{
    string owner = groups[gid][0];
    cout << "owner " << owner << endl;
    if (owner == uidOwn)
    {
        for (auto it = pendingReq[gid].begin(); it != pendingReq[gid].end(); ++it)
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
        groupIds = groupIds + " " + m.first;
        cout << m.first << ": ";
        for (auto x : m.second)
        {
            cout << x << " ";
        }
        cout << endl;
        // cout<<"GroupId: "<<x.first<<endl;
        // cout<<"Owner: "<<x.second[0]<<endl;
        // cout<<"Members: "
    }
    return groupIds;
}
void logout(string ip, int port)
{
    for (auto x : portIpUsers)
    {
        cout << x.first << " " << x.second.first << " " << x.second.second << endl;
    }
    unordered_map<string, pair<string, int>> :: iterator it;
    for (it = portIpUsers.begin(); it != portIpUsers.end(); ++it)
    {
        if ((*it).second.first == ip && (*it).second.second == port)
        {
            portIpUsers.erase(it);
            break;
        }
    }
    for (auto x : portIpUsers)
    {
        cout << x.first << " " << x.second.first << " " << x.second.second << endl;
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
            // cout << pai << endl;
        }
        return requests;
    }
    else
        return "You are not owner of group";
}
string getFileName(string path)
{
    string dummy;
    stringstream ss(path);
    string intermediate;
    while (getline(ss, intermediate, '/'))
    {
        dummy = intermediate;
    }
    return dummy;
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
    // fileHash[filename] = hashval;
    string chunkmap = "";
    string bitmap = getChunks(len);
    chunkmap = userId + "$" + bitmap;

    // cout << "line 257 chunkmap"
    //      << " " << chunkmap << endl;

    bool flag = 0;
    // for (struct fileDetails p : filedetails)
    // {
    //     if (p.gid == gid)
    //     {
    //         p.fileOwners[filename].push_back(chunkmap);
    //         p.sha_filenames[filename].first = shaval;
    //         p.sha_filenames[filename].second = len;
    //     }
    // }

    // for (struct fileDetails p : filedetails)
    // {
    //     cout << "printing after adding " << gid << endl;
    //     if (p.gid == gid)
    //     {
    //         for (string x : p.fileOwners[filename])
    //         {
    //             cout << "line 279" << x << endl;
    //         }
    //         cout << "line 281" << p.sha_filenames[filename].first << endl;
    //         cout << "line 282" << p.sha_filenames[filename].second << endl;
    //     }
    // }
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

        cout << "In creating struct " << endl;
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

    // for (struct filepaths p : filenameWithPaths)
    // {
    //     if (p.uid == userId)
    //     {
    //         flag2 = 1;
    //         p.filenames_paths[filename] = filepath;
    //     }
    // }
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
        // if (p->ip == ip && p->port == port)
        // {
        //     return p->filenames_paths[fname];
        // }
        cout << p.uid << " ";
        if (p.uid == uid)
        {
            return p.filenames_paths[fname];
        }
    }
    cout << "Cannot find path" << endl;
    exit(1);
    return "";
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
        cout << "gid" << p.gid << " ";
        if (p.gid == gid)
        {
            shaval = p.sha_filenames[filename].first;
            len = p.sha_filenames[filename].second;
            // fullpath = p.filenames_paths[filename];
            for (auto chunkmap : p.fileOwners[filename])
            {
                cout << "line 313 in download" << chunkmap << endl;
                ownermap.push_back(chunkmap);
            }
            // break;
        }
        cout << endl;
    }

    for (string chunk : ownermap)
    {
        string owner = "";
        string uid = "";
        int i = 0;
        cout << "line 326 " << chunk.size();
        for (i = 0; i < chunk.size(); i++)
        {
            if (chunk[i] == '$')
                break;

            owner = owner + chunk[i];
            cout << "chunk[i] " << chunk[i] << "owner  " << owner << endl;
        }
        if (portIpUsers.find(owner) != portIpUsers.end())
        {
            ip = portIpUsers[owner].first;
            port = to_string(portIpUsers[owner].second);
            cout << "line 293" << endl;
            cout << ip << port << "  " << chunk.substr(i, chunk.length() - 1);
            cout << endl;
            string fpath = getPath(owner, filename);
            allfiles = fpath + "#" + ip + ":" + port + chunk.substr(i, chunk.length() - 1) + " " + allfiles;
        }
    }

    allfiles = allfiles + " " + shaval + " " + len; //uid#ip:port$01010101 ip:port$01010101 ip:port$01010101 shaval sizeoffileinbytes
    cout << allfiles << endl;
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
            vector<string>:: iterator fown;
            for (fown = (*it).fileOwners[filename].begin(); fown != (*it).fileOwners[filename].end(); ++fown)
            {
                cout << "in details of chunk"
                     << " " << endl;
                std::string::size_type pos = (*fown).find('$');
                string up = (*fown).substr(0, pos);
                if (up == uid)
                {
                    flag = 1;
                    (*it).fileOwners[filename].erase(fown);
                    cout << "line 333 erasing" << endl;
                    break;
                }
            }
            string val = uid + "$" + chunkmap;
            cout << "line 338   " << val << endl;
            (*it).fileOwners[filename].push_back(val);
        }
    }
    for (auto p : filedetails)
    {
        cout << "gid" << p.gid << " ";
        if (p.gid == gid)
        {
            for (auto it = p.fileOwners[filename].begin(); it != p.fileOwners[filename].end(); ++it)
            {
                cout << "in details of chunk"
                     << " " << endl;
                std::string::size_type pos = (*it).find('$');
                string up = (*it).substr(0, pos);
                if (up == uid)
                {
                    flag = 1;
                    p.fileOwners[filename].erase(it);
                    cout << "line 333 erasing" << endl;
                    break;
                }
            }
            string val = uid + "$" + chunkmap;
            cout << "line 338   " << val << endl;
            p.fileOwners[filename].push_back(val);
        }
    }
}
void putD(string ip, string port, string gid, string filename, string filepath)
{
    // string uid = searchUser(convertToInt(port), ip);
    bool flag = 0;
    downloading[gid].push_back(filename);

    string userId = searchUser(convertToInt(port), ip);
    struct filepaths fpath;
    fpath.uid = userId;
    fpath.filenames_paths[filename] = filepath;
    filenameWithPaths.push_back(fpath);

    // string chunk = "111111111111111111";
    // vector<struct fileDetails> :: iterator it;
    // string chunkmap = userId+"$"+chunk;
    // for (it = filedetails.begin(); it != filedetails.end(); it++)
    // {
    //     if ((*it).gid == gid)
    //     {
    //         flag = 1;
    //         (*it).fileOwners[filename].push_back(chunkmap);
    //     }
    // }
}
void insertIntocom(string gid, string uid, string filename)
{
    // bool flag = 0;
    // for (struct downloads p : downloadinfo)
    // {
    //     cout << "gid" << p.gid << " ";
    //     if (p.gid == gid)
    //     {
    //         cout << "line 355" << endl;
    //         flag = 1;
    //         p.completed[filename].push_back(uid);
    //         break;
    //     }
    // }
    // if (!flag)
    // {
    //     struct downloads comd;
    //     comd.gid = gid;
    //     comd.completed[filename].push_back(uid);
    //     cout<<"line 366"<<endl;
    //     downloadinfo.push_back(comd);
    // }
}
void stop_share(string ip, string port, string gid, string filename)
{

    string userId = searchUser(convertToInt(port),ip);
    vector<struct fileDetails>::iterator it;
    for (it = filedetails.begin(); it != filedetails.end(); it++)
    {
        if ((*it).gid == gid)
        {
            vector<string>:: iterator fown;
            for (fown = (*it).fileOwners[filename].begin(); fown != (*it).fileOwners[filename].end(); ++fown)
            {
                cout << "in details of chunk"
                     << " " << endl;
                std::string::size_type pos = (*fown).find('$');
                string up = (*fown).substr(0, pos);
                if (up == userId)
                {
                    (*it).fileOwners[filename].erase(fown);
                    cout << "line 333 erasing" << endl;
                    break;
                }
            }
        }
    }
}
void putC(string ip, string port, string gid, string filename)
{
    // downloading[gid].erase(downloading[gid].find(filename));

    // cout << "Before " << endl;
    // string uid = searchUser(convertToInt(port), ip);
    for (auto it = downloading[gid].begin(); it != downloading[gid].end(); ++it)
    {
        if ((*it) == filename)
        {
            downloading[gid].erase(it);
            break;
        }
    }
    completed[gid].push_back(filename);
    //         {
    //             if ((*it) == uid)
    //             {
    //                 cout << "line 382 erasing the down" << endl;
    //                 p.downloading[filename].erase(it);
    //                 if (p.downloading[filename].size() == 0)
    //                 {
    //                     cout << "line 386 erasing..." << endl;
    //                     p.downloading.erase(filename);
    //                 }
    //                 insertIntocom(gid, uid, filename);
    //                 break;
    //             }
    //         }
    // for (auto p : downloadinfo)
    // {
    //     cout << p.gid << endl;
    //     for (auto d : p.downloading)
    //     {
    //         cout << "Downloading" << d.first << " ";
    //         for (auto x : d.second)
    //         {
    //             cout << x << " ";
    //         }
    //         cout << endl;
    //     }
    //     for (auto d : p.completed)
    //     {
    //         cout << "completed" << d.first << " ";
    //         for (auto x : d.second)
    //         {
    //             cout << x << " ";
    //         }
    //         cout << endl;
    //     }
    // }
    // for (struct downloads p : downloadinfo)
    // {
    //     if (p.gid == gid)
    //     {
    //         cout << "gid"
    //              << "line 373" << endl;
    //         for (auto it = p.downloading[filename].begin(); it != p.downloading[uid].end(); ++it)
    //         {
    //             if ((*it) == uid)
    //             {
    //                 cout << "line 382 erasing the down" << endl;
    //                 p.downloading[filename].erase(it);
    //                 if (p.downloading[filename].size() == 0)
    //                 {
    //                     cout << "line 386 erasing..." << endl;
    //                     p.downloading.erase(filename);
    //                 }
    //                 insertIntocom(gid, uid, filename);
    //                 break;
    //             }
    //         }
    //     }
    // }
    // cout << "After: " << endl;
    // for (auto p : downloadinfo)
    // {
    //     cout << p.gid << endl;
    //     for (auto d : p.downloading)
    //     {
    //         cout << "Downloading" << d.first << " ";
    //         for (auto x : d.second)
    //         {
    //             cout << x << " ";
    //         }
    //         cout << endl;
    //     }
    //     for (auto d : p.completed)
    //     {
    //         cout << "completed" << d.first << " ";
    //         for (auto x : d.second)
    //         {
    //             cout << x << " ";
    //         }
    //         cout << endl;
    //     }
    // }
}
string showdownloads()
{
    string val = "";
    string g = "";
    // for (auto p : downloadinfo)
    // {
    //     string gid = p.gid;
    //     string s = "";
    //     for (auto d : p.downloading)
    //     {
    //         cout << "line 411 in downloading" << endl;
    //         s = "D [" + gid + "] " + d.first + "\n";
    //     }
    //     g = g + s;
    //     s = "";
    //     for (auto d : p.completed)
    //     {
    //         cout << "line 411 in completed" << endl;
    //         s = "C [" + gid + "] " + d.first + "\n";
    //     }
    //     g = g + s;
    //     cout << g << "line 415" << endl;
    //     val = val + g;
    //     g = "";
    // }
    for (auto gid : downloading)
    {
        // cout << "line 411 in downloading" << endl;
        for (auto fname : gid.second)
            val = "D [" + gid.first + "] " + fname + "\n" + val;
    }
    for (auto gid : completed)
    {
        cout << "line 411 in downloading" << endl;
        for (auto fname : gid.second)
            val = "C [" + gid.first + "] " + fname + "\n" + val;
    }
    return val;
}
string list_files(string gid)
{
    string val = "";
    for (auto p : filedetails)
    {
        if (gid == p.gid)
        {
            p.fileOwners["test.pdf"].push_back("abc$10000");
            cout << "line 380" << endl;
            for (auto g : p.fileOwners)
            {
                for (auto st : g.second)
                {
                    cout << st << endl;
                }
                val = g.first + "\n" + val;
                cout << "line 617  " << g.first << endl;
            }
        }
    }
    cout << "line 388" << endl;
    return val;
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
            cout << "something happened closing connection" << endl;
            close(clientSocD);
            pthread_exit(NULL);
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
                send(clientSocD, REGISTERED, 4096, 0);
            else
                send(clientSocD, "user already exists", 4096, 0);
        }
        else if (command[0] == "login")
        {
            // uid,pwd
            int port = convertToInt(command[4]);
            if (login(command[1], command[2], command[3], port, clientSocD))
                send(clientSocD, LOGGEDIN, 4096, 0);
            else
                send(clientSocD, "user doesn't exists", 4096, 0);
        }
        else if (command[0] == "getport")
        {
            send(clientSocD, "2001", 4096, 0);
        }
        else if (command[0] == "create_group")
        {
            int port = convertToInt(command[3]);
            string ip = command[2];
            create_group(command[1], ip, port); //command[1] = gid
            send(clientSocD, "create group successfully", 4096, 0);
        }
        else if (command[0] == "join_group")
        {
            int port = convertToInt(command[3]);
            string ip = command[2];
            int ownSocId = join_group(command[1], ip, port);
            string uidReq = searchUser(port, ip);
            string req = "peer " + uidReq + " " + "wants to join group" + " " + command[1];
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, req.c_str());
            cout << "line 220" << endl
                 << reqData << endl;
            send(ownSocId, reqData, 4096, 0);
        }
        else if (command[0] == "accept_request")
        {
            int port = convertToInt(command[4]);
            string ip = command[3];
            string gid = command[1];
            string uid = command[2];
            string uidown = searchUser(port, ip);
            string dataToSend = accept_request(gid, uid, uidown);
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
            int port = convertToInt(command[3]);
            string ip = command[2];
            leave_group(command[1], ip, port); //command[1] = gid
        }
        else if (command[0] == "logout")
        {
            int port = convertToInt(command[2]);
            string ip = command[1];
            logout(ip, port);
        }
        else if (command[0] == "list_groups")
        {
            string dataToSend = list_groups(); //command[1] = gid
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "list_requests")
        {
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
            int port = convertToInt(command[4]);
            string ip = command[3];
            //command[2] = gid command[1]=filename command[5]= hashvaloffile command[6]=sizeoffile
            upload_file(command[2], ip, port, command[1], command[5], command[6], command[7]);
            char reqData[4096] = {
                0,
            };
            // string uid = searchUser(port, ip);
            // string dataToSend = "upload " + uid + " " + command[1] + " " + command[7]; //u uid filename path
            // strcpy(reqData, dataToSend.c_str());
            // cout << "sending...   " << dataToSend << endl;
            // send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "download_file")
        {
            //command[1] = gid command[2]=filename commad[5] = destpath
            // cout << "line 327" << command[1] << command[2] << endl;
            string dataToSend = download_file(command[1], command[2]);
            char reqData[4096] = {
                0,
            };
            if (dataToSend.size() == 0)
                dataToSend = "no file found";
            else
                dataToSend = "d " + dataToSend + " " + command[1] + " " + command[2] + " " + command[5]; //d uid#ip:port$111000 uid#ip:port$1111111 shaval size gid filename despath
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
            cout << data << endl;
            string dataToSend = showdownloads();
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            cout << "sending...   " << dataToSend << endl;
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "list_files")
        {
            string dataToSend = list_files(command[1]);
            char reqData[4096] = {
                0,
            };
            strcpy(reqData, dataToSend.c_str());
            // cout << "sending...   " << dataToSend << endl;
            send(clientSocD, reqData, 4096, 0);
        }
        else if (command[0] == "stop_share")
        {
           // command[1] = gid command[1] = filename  command[3] = ip command[4] = port 
           stop_share(command[3],command[4],command[1],command[2]);
        }
        // cout << "line 79" << endl;
        cout << "line 252" << endl;
    }
    pthread_exit(NULL);
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
    pthread_t tid[120];
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
        if (i >= 120)
        {
            i = 0;
            while (i < 120)
            {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
    }
    return 0;
}