#include "headers.h"
#include <openssl/sha.h>
using namespace std;

void *FromTracker(void *);
void *ToTracker(void *);

// struct fileDetails
// {
//      int port;
//     string ip;
//     unordered_map<string, string> filenames_paths;
// };
// vector<struct fileDetails *> filenameWithPaths;
unordered_map<string, string> file_chunks;

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
struct downloadinfo
{
    int port;
    int port_of_me;
    int sockForserv;
    long long chunkno;
    string ip;
    string ip_of_me;
    string filename;
    string size;
    string gid;
    string despath;
    string srcpath;
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
long long convertToInt(string st)
{
    stringstream con(st);
    long long x = 0;
    con >> x;
    return x;
}
// string getChunks(string filename)
// {
//     struct stat statbuf;
//     check((stat(filename.c_str(), &statbuf) == -1), "could not open file");
//     intmax_t len = (intmax_t)statbuf.st_size;
//     unsigned long chunks = (len / CHUNK_SIZE);
//     cout << len << "   " << chunks << endl;
//     string bitmap = "";
//     for (int i = 0; i < chunks; i++)
//         bitmap += '1';
//     if (len % CHUNK_SIZE != 0)
//         bitmap += '1';
//     return bitmap;
// }
// string getPath(string ip,int port, string fname)
// {
//     for (struct fileDetails *p : filenameWithPaths)
//     {
//         if(p->ip == ip && p->port == port)
//         {
//             return p->filenames_paths[fname];
//         }
//         // cout<<p.uid<<" ";
//         // if (p.uid == uid)
//         // {
//         //     return p.filenames_paths[fname];
//         // }
//     }
//     cout << "Cannot find path" << endl;
//     exit(1);
//     return "";
// }
string getChunks(string len)
{
    long long int size = convertToInt(len);
    long long int no_of_chunks = size / CHUNK_SIZE;
    string bitmap = "";
    for (int i = 0; i < no_of_chunks; i++)
        bitmap += '0';
    if (size % CHUNK_SIZE != 0)
        bitmap += '0';
    return bitmap;
}
string getSHA(string filepath)
{
    string piecewiseSHA = "";
    FILE *fp = NULL;
    fp = fopen(filepath.c_str(), "r+");
    if (fp == NULL)
    {
        perror("");
        cout << "ERROR" << endl;
        exit(-1);
    }

    // getting file size
    struct stat statbuf;
    stat(filepath.c_str(), &statbuf);
    intmax_t len = (intmax_t)statbuf.st_size;

    unsigned char sha_of_file[20];
    unsigned char *file_binary = new unsigned char[len];

    bzero(file_binary, sizeof(file_binary));
    bzero(sha_of_file, sizeof(sha_of_file));

    char encryptedText[40];
    int n = 0;

    int j = 1;
    n = fread(file_binary, 1, sizeof(file_binary), fp);

    SHA1(file_binary, n, sha_of_file);

    for (int i = 0; i < 20; i++)
    {
        sprintf(encryptedText + 2 * i, "%02x", sha_of_file[i]);
    }
    // cout << encryptedText << " " << j << "\n";
    j++;
    piecewiseSHA = piecewiseSHA + encryptedText;
    // bzero(encryptedText, sizeof(encryptedText));
    // bzero(file_binary, CHUNK_SIZE);
    // bzero(sha_of_file, sizeof(sha_of_file));
    free(file_binary);
    return piecewiseSHA;
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
string getpiecesha(string piece, long long piecesize)
{
    unsigned char sha_of_file[20];
    unsigned char *file_binary = new unsigned char[piecesize];

    bzero(file_binary, sizeof(file_binary));
    bzero(sha_of_file, sizeof(sha_of_file));

    char encryptedText[40];
    SHA1(file_binary, piecesize, sha_of_file);
    for (int i = 0; i < 20; i++)
    {
        sprintf(encryptedText + 2 * i, "%02x", sha_of_file[i]);
    }
    string piecewiseSHA = "";
    piecewiseSHA = piecewiseSHA + encryptedText;
    free(file_binary);
    return piecewiseSHA;
}

string recvsha(string filepath, long long piecesize, off_t offset)
{
    unsigned char sha_of_file[20];
    unsigned char *file_binary = new unsigned char[piecesize];

    // cout<<"In sha 221 "<<endl;

    bzero(file_binary, sizeof(file_binary));
    bzero(sha_of_file, sizeof(sha_of_file));

    char encryptedText[40];
    // cout<<"In sha 227 "<<endl;

    int fp = 0;
    check(fp = open(filepath.c_str(), O_RDWR), "error in opening file");

    // off_t offset = i * CHUNK_SIZE;
    long long bytesReadFromFile = pread(fp, file_binary, CHUNK_SIZE, offset);
    //  cout<<"In sha 234 "<<bytesReadFromFile<<filepath<<" "<<offset<<endl;
    SHA1(file_binary, bytesReadFromFile, sha_of_file);
    close(fp);

    for (int i = 0; i < 20; i++)
    {
        sprintf(encryptedText + 2 * i, "%02x", sha_of_file[i]);
    }
    string piecewiseSHA = "";
    piecewiseSHA = piecewiseSHA + encryptedText;
    free(file_binary);
    // cout<<"In sha 244 "<<endl;
    return piecewiseSHA;
}
void *getConnection(void *args)
{

    struct downloadinfo *dinfo = (struct downloadinfo *)args;
    int port = dinfo->port;
    int port_of_me = dinfo->port_of_me;
    int sockForserv = dinfo->sockForserv;
    string ip = dinfo->ip;
    string ip_of_me = dinfo->ip_of_me;
    string filename = dinfo->filename;
    string size = dinfo->size;
    string gid = dinfo->gid;
    string despath = dinfo->despath;
    string srcpath = dinfo->srcpath;
    long long chunkno = dinfo->chunkno;

    int client_socd = 0;
    struct sockaddr_in address;
    int opt = 1;

    check((client_socd = socket(AF_INET, SOCK_STREAM, 0)), "socket of peer failed");
    check(setsockopt(client_socd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)), "setsockopt eeror");
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);
    memset(&address.sin_zero, 0, sizeof(address.sin_zero));
    check((connect(client_socd, (struct sockaddr *)&address, sizeof(address))), "connection with peer failed");

    // cout << "clientSocD    " << client_socd << endl;
    char data[1024] = {
        0,
    };

    recv(client_socd, data, 1024, 0);
    //  cout << data;
    // string tosend = "d " + gid + " " + filename;
    // data[1024] = {
    //     0,
    // };
    // string tosend = "";
    // tosend="I am from chunk " + chunkno;
    // strcpy(data, tosend.c_str());
    // // cout << data<<endl;
    // sleep(5);
    // send( soc, data, 1024, 0);

    // data[1024] = {
    //     0,
    // };
    //
    // recv(client_socd, data, 1024, 0);

    // sleep(1);

    // data[1024] = {
    //     0,
    // };
    string filepath = despath + filename;
    // long long len = convertToInt(size);

    // long long piece_begin = chunkno * CHUNK_SIZE;
    // long long pieceSize = 0;
    // if (len > piece_begin + CHUNK_SIZE - 1)
    // {
    //     pieceSize = CHUNK_SIZE;
    // }
    // else
    // {
    //     pieceSize = len - piece_begin;
    // }

    // cout << "requesting " << chunkno << "from   " << ip << ":" << to_string(port) << endl;
    data[1024] = {
        0,
    };
    string tosend = "requesting " + to_string(chunkno) + " " + gid + " " + filename + " " + srcpath;
    strcpy(data, tosend.c_str());
    send(client_socd, data, 1024, 0);
    data[1024] = {
        0,
    };
    // receiving
    recv(client_socd, data, 32, 0);
    long long pieceSize = convertToInt(data);

    // cout << "piecesize    " << pieceSize<<"  " <<chunkno<< endl;

    char sha_of_piece[40];
    recv(client_socd, sha_of_piece, 40, 0);
    string sha = sha_of_piece;

    int fp = 0;
    check(fp = open(filepath.c_str(), O_RDWR), "error in opening file");
    // FILE *fp = fopen(filepath.c_str(), "r+");
    // if (fp == NULL)
    // {
    //     cout << "line 947" << endl;
    //     perror("file does not exist");
    //     pthread_exit(NULL);
    // }
    // fseek(fp, chunkno * CHUNK_SIZE, SEEK_SET);
    long long totBytesRead = 0;
    char *file_chunk = new char[pieceSize];
    bzero(file_chunk, sizeof(file_chunk));
    long long off = chunkno * CHUNK_SIZE;

    // cout << "offset " << chunkno << " " << off << endl;

    while (totBytesRead < pieceSize)
    {
        bzero(file_chunk, sizeof(file_chunk));
        long bytesRead = recv(client_socd, file_chunk, pieceSize - totBytesRead, 0);
        totBytesRead += bytesRead;
        pwrite(fp, file_chunk, bytesRead, off);
        // fwrite(file_chunk, sizeof(char), bytesRead, fp);
        // fseek(fp, bytesRead, SEEK_CUR);
        off = off + bytesRead;
    }
    close(fp);
    // fclose(fp);

    // cout << "bytes received " << chunkno << " " << (totBytesRead) << endl;

    off_t offset = chunkno * CHUNK_SIZE;
    string sha_of_received = recvsha(filepath, pieceSize, offset);
    // sleep(1);

    // cout << "ca sha " << chunkno << " " << sha << endl;
    // cout << "my sha   " << chunkno << " " << sha_of_received << endl;
    // // sleep(2);
    if (sha.compare(sha_of_received) == 0)
    {
        cout << "sha verified for chunk " << chunkno << endl;
        
    }
    else
    {
        cout << "sha not matching for chunk " << chunkno << endl;
    }

    // cout << "received chunk" << chunkno << endl;
    // sleep(2);

    // file_chunks[filename][chunkno] = '1';
    // string mychunkmap = file_chunks[filename];
    // data[1024] = {
    //     0,
    // };
    // // sleep(1);
    // string toserver = "chunk " + ip_of_me + " " + to_string(port_of_me) + " " + gid + " " + filename + " " + mychunkmap;
    // // cout << "line 350 " << toserver << endl;
    // strcpy(data, toserver.c_str());
    // send(sockForserv, data, 1024, 0);

    // data[1024] = {
    //     0,
    // };
    // string end = "exit " + to_string(chunkno);
    // strcpy(data, end.c_str());
    // send(client_socd, data, 1024, 0);
    // cout << "exiting from  " << client_socd << endl;
    // fflush(stdout);
    free(file_chunk);
    close(client_socd);
    pthread_exit(NULL);
}
// void getConnection(string ip, int port, string filename, string size, string gid, string despath, string srcpath, int sockForserv, string ip_of_me, int port_of_me)
// {
//     int client_socd;
//     struct sockaddr_in address;
//     int opt = 1;
//     check((client_socd = socket(AF_INET, SOCK_STREAM, 0)), "socket of peer failed");

//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = inet_addr(ip.c_str());
//     address.sin_port = htons(port);
//     memset(&address.sin_zero, 0, sizeof(address.sin_zero));
//     check((connect(client_socd, (struct sockaddr *)&address, sizeof(address))), "connection with peer failed");
//     char data[1024] = {
//         0,
//     };
//     string tosend = "d " + gid + " " + filename;
//     strcpy(data, tosend.c_str());
//     send(client_socd, data, 1024, 0);

//     data[1024] = {
//         0,
//     };
//     //
//     recv(client_socd, data, 1024, 0);
//     cout << data << endl;
//     string mychunkmap = getChunks(size);

//     data[1024] = {
//         0,
//     };
//     string toserverd = "downloading " + ip_of_me + " " + to_string(port_of_me) + " " + gid + " " + filename;
//     strcpy(data, toserverd.c_str());
//     send(sockForserv, data, 1024, 0);
//     sleep(1);
//     data[1024] = {
//         0,
//     };

//     string filepath = despath + filename;
//     int fd = 0;
//     mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
//     if ((fd = creat(filepath.c_str(), mode)) < 0)
//     {
//         perror("File creation error.");
//         //return 1;
//     }
//     // FILE *temp = fopen(filepath.c_str(), "a");
//     // fclose(temp);
//     long long len = convertToInt(size);
//     ftruncate(fd, len);
//     long long off = 0;
//     for (int i = 0; i < mychunkmap.size(); i++)
//     {
//         cout << "Requesting chunk " << i << endl;
//         data[1024] = {
//             0,
//         };
//         string tosend = "r " + to_string(i) + " " + gid + " " + filename + " " + srcpath;
//         strcpy(data, tosend.c_str());
//         send(client_socd, data, 1024, 0);
//         data[1024] = {
//             0,
//         };
//         // receiving
//         recv(client_socd, data, 32, 0);

//         long long pieceSize = convertToInt(data);
//         // cout<<"piecesize" <<pieceSize<<endl;

//         char sha_of_piece[40];
//         recv(client_socd, sha_of_piece, 40, 0);
//         // FILE *fd = fopen(filepath.c_str(), "r+");
//         // fseek(fd, i * CHUNK_SIZE, SEEK_SET);
//         // writing to file
//         int fp = 0;
//         check(fp = open(filepath.c_str(), O_RDWR), "error in opening file");

//         long long totBytesRead = 0;
//         char file_chunk[CHUNK_SIZE];
//         char file_chunk2[CHUNK_SIZE];
//         //  string fck="",fullp="";

//         while (totBytesRead < pieceSize)
//         {
//             bzero(file_chunk, sizeof(file_chunk));
//             long bytesRead = recv(client_socd, file_chunk, pieceSize - totBytesRead, 0);
//             // cout << "bytes read =" << bytesRead << endl;
//             totBytesRead += bytesRead;
//             // cout << off << endl;
//             pwrite(fp, file_chunk, bytesRead, off);
//             off = off + bytesRead;
//             // fwrite(file_chunk, sizeof(char), bytesRead, fd);
//             // fseek(fd, bytesRead, SEEK_CUR);
//             // cout<<readBuffer<<endl;
//         }
//         close(fp);
//         // check(fp = open(filepath.c_str(), O_RDWR), "error in opening file");
//         off_t offset = i * CHUNK_SIZE;
//         // long long bytesReadFromFile = pread(fp, file_chunk, pieceSize, offset);
//         // cout<<file_chunk<<endl;
//         string sha_of_received = recvsha(filepath, pieceSize, offset);
//         sleep(1);
//         string sha = sha_of_piece;
//         cout << "ca sha   " << sha << endl;
//         cout << "my sha   " << sha_of_received << endl;
//         sleep(2);
//         if (sha.compare(sha_of_received) == 0)
//         {
//             cout << "sha verified for chunk" << i << endl;
//         }
//         else
//         {
//             cout << "sha not matching for chunk" << i << endl;
//         }
//         close(fp);
//         // fclose(fd);
//         cout << "received chunk" << i << endl;
//         mychunkmap[i] = '1';

//         char datax[1024] = {
//             0,
//         };
//         // sleep(1);
//         string toserver = "chunk " + ip_of_me + " " + to_string(port_of_me) + " " + gid + " " + filename + " " + mychunkmap;
//         strcpy(datax, toserver.c_str());
//         send(sockForserv, datax, 1024, 0);
//         // cout << datax << endl;
//         // sleep(1);

//         // off_t offset = i * CHUNK_SIZE;
//         // recv(client_socd, file_chunk, sizeof(file_chunk), 0);
//     }
//     data[1024] = {
//         0,
//     };
//     string end = "exit";
//     strcpy(data, end.c_str());
//     send(client_socd, data, 1024, 0);
//     close(client_socd);
//     // cout << "ouside loop line 291" << endl;
//     data[1024] = {
//         0,
//     };
//     string toserverc = "completed " + ip_of_me + " " + to_string(port_of_me) + " " + gid + " " + filename;
//     strcpy(data, toserverc.c_str());
//     send(sockForserv, data, 1024, 0);
//     // cout << data << endl;
// }
bool comparesort(vector<pair<string, int>> a, vector<pair<string, int>> b)
{
    return (a.size() < b.size());
}
string searchUser(unordered_map<string, pair<string, int>> portIpUsers, int port, string ip)
{
    string s;
    int p;
    // cout << "line 38" << endl;
    for (auto m : portIpUsers)
    {

        s = m.second.first;
        p = m.second.second;
        // cout << s << " " << p << endl;
        if (port == p && ip == s)
        {
            // cout << "line 39" << endl;
            return m.first;
        }
    }
    return "";
}
vector<pair<string, int>> givepeerinfo(vector<string> peerinfo, vector<string> &userinfo, string size_of_file, string filename)
{
    long long int len = convertToInt(size_of_file);
    long long int no_of_chunks = len / CHUNK_SIZE;
    if (len % CHUNK_SIZE != 0)
        no_of_chunks = no_of_chunks + 1;
    vector<vector<pair<string, int>>> chunkpeerinfo(no_of_chunks);
    vector<pair<string, int>> selectedpeers(no_of_chunks);
    unordered_map<string, pair<string, int>> uidportIp;
    int size = peerinfo.size() - 6;
    for (int i = 1; i <= size; i++)
    {
        string peer = peerinfo[i];
        string peerip = "";
        int peerport = 0;
        string uid = "";
        string temp = "";
        int j = 0;
        for (j = 0; j < peer.size(); j++)
        {
            if (peer[j] == '#')
            {
                uid = temp;
                temp = "";
                continue;
            }
            else if (peer[j] == ':')
            {
                peerip = temp;
                temp = "";
                continue;
            }
            else if (peer[j] == '$')
            {
                peerport = convertToInt(temp);
                temp = "";
                break;
            }
            temp = temp + peer[j];
        }
        // cout<<uid <<" "<<peerip<<" "<<peerport<<endl;
        uidportIp[uid] = make_pair(peerip, peerport);
        // cout<<"j "<<j<<endl;
        for (int k = j + 1, l = 0; k < peer.size(); k++, l++)
        {
            // cout<<k<<" "<<l<<endl;
            if (peer[k] == '1')
            {
                chunkpeerinfo[l].push_back(make_pair(peerip, peerport));
            }
        }
    }
    // if (chunkpeerinfo.size() > 1)
    sort(chunkpeerinfo.begin(), chunkpeerinfo.end(), comparesort);
    for (int i = 0; i < chunkpeerinfo.size(); i++)
    {
        int len_of_peerscount = chunkpeerinfo[i].size();
        int random = rand() % len_of_peerscount;
        // cout<<"random "<<random<<" "<<len_of_peerscount<<endl;
        selectedpeers[i] = chunkpeerinfo[i][random];
        pair<string, int> p = chunkpeerinfo[i][random];
        string user = searchUser(uidportIp, p.second, p.first);
        // cout<<"line 590 "<<user<<endl;
        userinfo.push_back(user);
    }
    // cout<<"returning"<<endl;
    return selectedpeers;
}
void *FromTracker(void *arguments)
{
    struct clientDetails *args = (struct clientDetails *)arguments;
    int client_socd = args->socketId;
    int port_of_me = args->port;
    string ip_of_me = args->ip;
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
        if (received[0] == "upload")
        {
            // cout<<data<<" ";
            // bool flag = 0;
            // for (auto user : filenameWithPaths)
            // {
            //     // if (user.uid == received[1])
            //     // {
            //     //     flag = 1;
            //     //     string filename = received[2];
            //     //     user.filenames_paths[filename] = received[3];
            //     //     break;
            //     // }
            // }
            // if (!flag)
            // {

            //     struct fileDetails filed;
            //     // filed.uid = received[1];
            //     string filename = received[2];
            //     filed.filenames_paths[filename] = received[3];
            //     filenameWithPaths.push_back(&filed);
            //     // cout<<getPath(filenameWithPaths[0].uid,filename)<<endl;
            //     cout<<"pushed successfully"<<endl;
            // }
        }
        else if (received[0] == "d")
        {
            vector<string> userinfo;
            // string dumm = data;
            // cout << "result" << dumm << endl;
            // int i = 0;
            // string ip = "";
            // for (i = 0; i < received[1].size(); i++)
            // {
            //     if (received[1][i] == ':')
            //         break;
            //     ip = ip + received[1][i];
            // }
            // string portstr = "";
            // for (i = i + 1; i < received[1].size(); i++)
            // {
            //     if (received[1][i] == '$')
            //         break;
            //     portstr = portstr + received[1][i];
            // }

            pthread_t tid[60];
            // string chunkmap = received[1].substr(i + 1, received[1].size() - 1);
            string filename = received[received.size() - 2];
            string gid = received[received.size() - 3];
            string size = received[received.size() - 4];
            string shaval = received[received.size() - 5];
            string despath = received[received.size() - 1];
            // string srcpath = received[received.size() - 4];
            // int port = convertToInt(portstr);
            cout << "line 672" << filename << endl;
            string mychunkmap = getChunks(size);
            file_chunks[filename] = mychunkmap;
            vector<pair<string, int>> selectedpeers = givepeerinfo(received, userinfo, size, filename);
            struct downloadinfo dinfo[selectedpeers.size()];
            for (int i = 0; i < selectedpeers.size(); i++)
            {
                dinfo[i].port = selectedpeers[i].second;
                dinfo[i].port_of_me = port_of_me;
                dinfo[i].sockForserv = client_socd;
                dinfo[i].ip = selectedpeers[i].first;
                dinfo[i].ip_of_me = ip_of_me;
                dinfo[i].filename = filename;
                dinfo[i].size = size;
                dinfo[i].gid = gid;
                dinfo[i].despath = despath;
                dinfo[i].srcpath = userinfo[i];
                dinfo[i].chunkno = i;
                // cout<<userinfo[i]<<despath<<endl;
                // cout << selectedpeers[i].first << " " << selectedpeers[i].second << endl;
            }
            string filepath = despath + filename;
            int fd = 0;
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if ((fd = creat(filepath.c_str(), mode)) < 0)
            {
                perror("File creation error.");
            }
            long long len = convertToInt(size);
            // ftruncate(fd, len);

            string toserverd = "downloading " + ip_of_me + " " + to_string(port_of_me) + " " + gid + " " + filename + " " + filepath;
            data[1024] = {
                0,
            };
            strcpy(data, toserverd.c_str());
            send(client_socd, data, 1024, 0);
            cout << selectedpeers.size() << endl;
            for (int i = 0; i < selectedpeers.size(); i++)
            {
                // cout << i << endl;
                check(pthread_create(&tid[i], NULL, getConnection, (void *)&dinfo[i]), "Failed to create thread");
            }
            int t = 0;
            while (t < selectedpeers.size())
            {
                pthread_join(tid[t++], NULL);
            }
            data[1024] = {
                0,
            };
            string toserverc = "completed " + ip_of_me + " " + to_string(port_of_me) + " " + gid + " " + filename;
            strcpy(data, toserverc.c_str());
            send(client_socd, data, 1024, 0);
            // getConnection(ip, port, filename, size, gid, despath, srcpath, client_socd, ip_of_me, port_of_me);
        }
    }
    pthread_exit(NULL);
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
             << setw(15) << left << "upload_file"
             << "<filepath> <gid>" << endl
             << setw(15) << left << "download_file"
             << "<gid> <filename> <des_path>" << endl
             << setw(15) << left << "list_files"
             << "<gid>" << endl
             << setw(15) << left << "show_downloads" << endl
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
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        else if (command[0] == "accept_request")
        {
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        else if (command[0] == "logout")
        {
            inpFromUser = inpFromUser + " " + args->ip + " " + to_string(args->port);
        }
        else if (command[0] == "upload_file")
        {
            string shavalue = getSHA(command[1]);

            // for file size
            struct stat statbuf;
            check((stat(command[1].c_str(), &statbuf) == -1), "could not open file");
            intmax_t len = (intmax_t)statbuf.st_size;
            // long chunks = (len / (512 * 1024));
            // cout << len << "   " << chunks << endl;
            // if (len % CHUNK_SIZE != 0)
            //     chunks = chunks + 1;

            string filename = getFileName(command[1]);

            // struct fileDetails filed;
            // filed.gid = command[2];
            // filed.filenames_paths[filename] = command[1];
            // filenameWithPaths.push_back(filed);

            // cout << filed.gid << "  " << filed.filenames_paths[filename] << endl;
            //    string filepath = command[1];
            //    for(int i=filepath.size()-1;i>=0;i--)
            //    {
            //        if(filepath[i] == '/')
            //         break;
            //    }
            //     pathname
            // struct fileDetails filed;
            // filed.ip = args->ip;
            // filed.port = args->port;
            // filed.filenames_paths[filename] = command[1];
            // filenameWithPaths.push_back(&filed);
            inpFromUser = command[0] + " " + filename + " " + command[2] + " " + args->ip + " " + to_string(args->port) + " " + shavalue + " " + to_string(len) + " " + command[1];
        }
        else if (command[0] == "download_file")
        {
            string des_path = command[3];
            inpFromUser = command[0] + " " + command[1] + " " + command[2] + " " + args->ip + " " + to_string(args->port) + " " + command[3];
        }
        else if (command[0] == "show_downloads")
        {
            cout << "In show_downloads" << endl;
        }
        else if (command[0] == "list_files")
        {
            cout << "In list_files" << endl;
        }
        else if (command[0] == "getpath")
        {
            // cout<<"line 860 "<<getPath("127.0.0.1",2000,"test.pdf")<<endl;
            continue;
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

// string getSHAofP(string piece, long long piecesize)
// {

//     unsigned char sha_of_file[20];
//     unsigned char *file_binary = new unsigned char[piecesize];
//     bzero(file_binary, sizeof(file_binary));
//     bzero(sha_of_file, sizeof(sha_of_file));

//     SHA1(file_binary, piecesize, sha_of_file);
//     char encryptedText[40];
//     for (int i = 0; i < 20; i++)
//     {
//         sprintf(encryptedText + 2 * i, "%02x", sha_of_file[i]);
//     }
//     // cout << encryptedText << " " << j << "\n";
//     string piecewiseSHA = "";
//     piecewiseSHA = piecewiseSHA + encryptedText;
//     // bzero(encryptedText, sizeof(encryptedText));
//     // bzero(file_binary, CHUNK_SIZE);
//     // bzero(sha_of_file, sizeof(sha_of_file));
//     free(file_binary);
//     return piecewiseSHA;
// }
void *acceptConnection(void *arguments)
{
    struct arg_struct_cli *args = (struct arg_struct_cli *)arguments;
    int port = args->arg1;
    int clientSocD = args->arg2;
    // cout << "clientSocD    " << clientSocD << endl;
    // fflush(stdout);
    sleep(2);
    // while (1)
    // {
    char data[1024] = {
        0,
    };
    send(clientSocD, "ok I will send you", 1024, 0);
    int nRet = recv(clientSocD, data, 1024, 0);
    // cout << "clientSocD    " << clientSocD << "  nret " << nRet << endl;
    sleep(2);
    if (nRet == 0)
    {
        cout << "something happened closing connection" << endl;
        close(clientSocD);
        // break;
        pthread_exit(NULL);
    }
    // cout << data << endl;
    // fflush(stdout);
    // coverting string into words
    // data[1024] = {
    //     0,
    // };
    // recv(clientSocD, data, 1024, 0);
    vector<string> received;
    istringstream sst(data);
    string inter;
    while (sst >> inter)
    {
        received.push_back(inter);
    }
    if (received[0] == "requesting")
    {
        //     send(clientSocD, "ok I will send you", 1024, 0);
        // }
        // else if (received[0] == "rgive")
        // {
        long long chunkno = convertToInt(received[1]);
        string gid = received[2];
        string filename = received[3];
        string filepath = received[4];
        // long long piece_size = 0;
        // cout << piece_size << endl;
        // cout << chunkno << endl;
        // int fp = 0;

        FILE *fp = fopen(filepath.c_str(), "r+");
        if (fp == NULL)
        {
            cout << "line 947" << endl;
            perror("file does not exist");
            pthread_exit(NULL);
        }

        long piece_size = 0;
        fseek(fp, chunkno * CHUNK_SIZE, SEEK_SET);
        long piece_begin = ftell(fp);
        fseek(fp, 0, SEEK_END);
        long file_end = ftell(fp);
        fclose(fp);

        if (file_end > piece_begin + CHUNK_SIZE - 1)
        {
            piece_size = CHUNK_SIZE;
        }
        else
        {
            piece_size = file_end - piece_begin;
        }
        // cout<<"piecesize" <<piece_size <<"of chunk "<<chunkno<<endl;
        send(clientSocD, to_string(piece_size).c_str(), 32, 0);

        off_t offset = chunkno * CHUNK_SIZE;
        string sha_of_piece = recvsha(filepath, piece_size, offset);
        char shachar[40] = {
            0,
        };
        strcpy(shachar, sha_of_piece.c_str());
        // cout << "SHA   " << shachar << endl;
        send(clientSocD, shachar, 40, 0);




        int fd = 0;
        check(fd = open(filepath.c_str(), O_RDWR), "error in opening file");
        char *file_chunk = new char[piece_size];
        bzero(file_chunk, sizeof(file_chunk));

        // long long offset  = 0;

        // fseek(fp, chunkno * CHUNK_SIZE, SEEK_SET);
        // cout << offset << endl;
        // cout << "sending " << chunkno << " " << piece_size << endl;
        long long bytesReadFromFile = pread(fd, file_chunk, CHUNK_SIZE, offset);
        close(fd);

        // string sha_of_piece = getpiecesha(file_chunk, bytesReadFromFile);

        // long bytesReadFromFile = fread(file_chunk, sizeof(char), piece_size, fp);
        // cout << "bytes read " << bytesReadFromFile << " " << chunkno << endl;
        //      << "sending chunk " << chunkno << endl;
        // offset = offset + bytesReadFromFile;
        sleep(2);
        send(clientSocD, file_chunk, bytesReadFromFile, 0);

        // sleep(1);

        // sleep(1);
        data[1024] = {
            0,
        };
        cout << "exiting from  "
             << " " << chunkno << endl;
        close(clientSocD);

        free(file_chunk);
        pthread_exit(NULL);
        // pread(fp, file_chunk, sizeof(file_chunk), offset);
    }
    // send(clientSocD, file_chunk, sizeof(file_chunk), 0);
    // close(fd);
    // }
    // if (received[0] == "exit")
    // {
    pthread_exit(NULL);
    // break;
    // }
    // }
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
    struct arg_struct_cli args[50];
    while (1)
    {
        int clientSocD = 0;
        socklen_t len = sizeof(struct sockaddr);
        // cout<<"line 210"<<endl;
        check((clientSocD = accept(server_socd, (struct sockaddr *)&address, &len)), "accept error");
        args[i].arg1 = cliport;
        args[i].arg2 = clientSocD;
        // cout<<"clientSocD    " <<clientSocD<<" "<<i<<endl;
        // cout<<"creating thread "<<i<<endl;
        check(pthread_create(&tid[i], NULL, acceptConnection, (void *)&args[i]), "Failed to create thread");
        i++;
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
    // sleep(2);
    return 0;
}
