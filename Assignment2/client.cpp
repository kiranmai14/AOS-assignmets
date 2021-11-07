#include "headers.h"
#include <openssl/sha.h>
#define BUFF_SIZE 512 * 1024 * 512
using namespace std;

void *FromTracker(void *);
void *ToTracker(void *);

struct fileDetails
{
    string gid;
    unordered_map<string, string> filenames_paths;
};
vector<struct fileDetails> filenameWithPaths;

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
    unsigned char file_binary[CHUNK_SIZE];

    bzero(file_binary, sizeof(file_binary));
    bzero(sha_of_file, sizeof(sha_of_file));

    char encryptedText[40];
    int n = 0;

    int j = 1;
    while ((n = fread(file_binary, 1, sizeof(file_binary), fp)) > 0)
    {

        SHA1(file_binary, n, sha_of_file);
        for (int i = 0; i < 20; i++)
        {
            sprintf(encryptedText + 2 * i, "%02x", sha_of_file[i]);
        }
        cout << encryptedText << " " << j << "\n";
        j++;
        piecewiseSHA = piecewiseSHA + encryptedText;
        bzero(encryptedText, sizeof(encryptedText));
        bzero(file_binary, CHUNK_SIZE);
        bzero(sha_of_file, sizeof(sha_of_file));
    }
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
void getConnection(string ip, int port, string chunkmap, string filename, string shaval, string size, int sockForserv)
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
    send(client_socd, shaval.c_str(), 1024, 0);
    recv(client_socd, data, 1024, 0);
    cout << data << endl;
    string mychunkmap = getChunks(size);
    for (int i = 0; i < chunkmap.size(); i++)
        mychunkmap = mychunkmap + '0';
    cout << mychunkmap << endl;

    char buffer[CHUNK_SIZE];
    vector<string> pieceWiseSHA;
    bzero(buffer, sizeof(buffer));
    // for(int i=0;i<chunkmap.size();i++)
    // {
    //     cout<<"Requesting chunk 0";
    //     send(client_socd,to_string(i).c_str(),1024,0);
    // recv(client_socd, buffer, sizeof(buffer), 0);
    // bzero(buffer, sizeof(buffer));

    // unsigned char sha_of_piece[20];
    // unsigned char file_binary[sizeof(buffer)];
    // file_binary = buffer;
    // bzero(sha_of_piece, sizeof(sha_of_piece));
    // SHA1(buffer, sizeof(buffer), sha_of_piece);

    // bzero(buffer, sizeof(buffer));

    // recv(client_socd, buffer, sizeof(buffer), 0);
    // string sha(sha_of_file, sha_of_file + 20);
    // string sha_of_received =

    // }
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
        cout << data << sizeof(data) << endl;
        // processing received data
        vector<string> received;
        istringstream sst(data);
        string inter;
        while (sst >> inter)
        {
            received.push_back(inter);
        }
        if (received[0] == "d")
        {
            string dumm = data;
            cout << "result" << dumm << endl;
            int i = 0;
            string ip = "";
            for (i = 0; i < received[1].size(); i++)
            {
                if (received[1][i] == ':')
                    break;
                ip = ip + received[1][i];
            }
            string portstr = "";
            for (i = i + 1; i < received[1].size(); i++)
            {
                if (received[1][i] == '$')
                    break;
                portstr = portstr + received[1][i];
            }
            int port = convertToInt(portstr);
            string chunkmap = received[1].substr(i + 1, received[1].size() - 1);
            string filename = received[received.size() - 1];
            string size = received[received.size() - 2];
            string shaval = received[received.size() - 3];
            getConnection(ip, port, chunkmap, filename, shaval, size, client_socd);
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
             << "<filepath> <gic>" << endl
             << setw(15) << left << "download_file"
             << "<gid> <filename> <des_path>" << endl
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

            struct fileDetails filed;
            filed.gid = command[2];
            filed.filenames_paths[filename] = command[1];

            cout<<filed.gid <<"  "<<filed.filenames_paths[filename]<<endl;

            inpFromUser = command[0] + " " + filename + " " + command[2] + " " + args->ip + " " + to_string(args->port) + " " + shavalue + " " + to_string(len);
        }
        else if (command[0] == "download_file")
        {
            string des_path = command[3];
            inpFromUser = command[0] + " " + command[1] + " " + command[2] + " " + args->ip + " " + to_string(args->port) + " " + command[3];
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
    char data[1024] = {
        0,
    };
    int nRet = recv(clientSocD, data, 1024, 0);
    cout << "got sha checking now" << endl;

    string shaval = data;
    // string filepath = sha_filenames[shaval];

    send(clientSocD, "sending you on the way", 1024, 0);
    cout << "exiting..." << endl;

    // while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    // { //for every 512K piece
    // 	// file_size = file_size-n;
    // 	SHA1(buffer, n, hash1);
    // 	bzero(buffer, BUFFER_SIZE); //reset buffer
    // 	for (int i = 0; i < 20; i++)
    // 	{ //convert to hex rep
    // 		sprintf(partial + 2 * i, "%02x", hash1[i]);
    // 	}
    // 	piecewiseSHA.push_back(partial);
    // 	cout << partial << "\n";
    // 	numOfPieces++;
    // 	// total_chunk_string += partial; //append hashes of all pieces into this
    // }
    // FILE *fp = fopen(filepath.c_str(), "r+");

    // 	if (fp == NULL)
    // 	{
    // 		perror("file does not exist");
    // 		pthread_exit(NULL);
    // 	}
    // cout<<"line 159"<<endl;
    // while (1)
    // {
    //     char data[1024] = {
    //         0,
    //     };
    //     // cout<<"line 165"<<endl;
    //     int nRet = recv(clientSocD, data, 1024, 0);
    //     if (nRet == 0)
    //     {
    //         cout << "something happened closing connection" << endl;
    //         pthread_exit(NULL);
    //         close(clientSocD);
    //     }
    //     cout << data << endl;
    //     string p = "";
    //     vector<string> command;
    //     istringstream ss(data);
    //     string intermediate;
    //     while (ss >> intermediate)
    //     {
    //         command.push_back(intermediate);
    //     }
    //     if(command[0] == "test")
    //     {
    //         send(clientSocD,"sending you test.pdf",20,0);
    //         cout<<"exiting..."<<endl;
    //     }
    // // }
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