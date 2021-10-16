#include "headers.h"
using namespace std;
bool checkDir(string file)
{
    struct stat fileInfo;
    if (stat((const char *)file.c_str(), &fileInfo) != 0)
    { // Use stat() to get the info
      // cout << "line 30";
      // std::cerr << "Error: " << strerror(errno) << '\n';
      // return (EXIT_FAILURE);
    }
    if ((fileInfo.st_mode & S_IFMT) == S_IFDIR)
    {
        return true;
    }
    else
    {
        return false;
    }
}
string getHome()
{
    char *tmp = (char *)malloc(256 * sizeof(char));
    getcwd(tmp, 256);
    return tmp;
}
char *get_cwd()
{
    char *tmp = (char *)malloc(256 * sizeof(char));
    getcwd(tmp, 256);
    return tmp;
}
string preProcess(string path)
{
    string absPath = "";
    if (path[0] == '~') ///\directory from where the application started
    {
        absPath = getHome() + path.substr(1, path.size() - 1);
    }
    else if (path == ".") // current directory
    {
        absPath = get_cwd();
        absPath = absPath + path.substr(1, path.size() - 1);
    }
    else if (path[0] == '/') ///\directory from where the application started
    {
        absPath = getHome() + path;
    }
    else
    {
        absPath = get_cwd();
        absPath = absPath + "/" + path;
    }
    // cout << "inside"
    //      << " " << absPath << endl;
    return absPath;
}
void create_file(vector<string> cmd)
{
    string filename = cmd[1];
    string abspath = preProcess(cmd[2]);
    cout << "outside"
         << " " << abspath << endl;
    filename = abspath + "/" + filename;
    cout << "Final path"
         << " " << filename << endl;
    // cout << filename;
    // cheach if it is a valid path
    if (checkDir(abspath))
    {
        int fd1 = open(filename.c_str(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd1 < 0)
        {
            perror("c1");
            exit(1);
        }
        printf("created the fd = % d\n", fd1);

        // Using close system Call
        if (close(fd1) < 0)
        {
            perror("c1");
            exit(1);
        }
        printf("closed the fd.\n");
    }
    else
    {
        cout << "Given destination is not a directory";
    }
}
void create_dir(vector<string> cmd)
{
    string dirname = cmd[1];
    string abspath = preProcess(cmd[2]);
    cout << abspath;
    dirname = abspath + "/" + dirname;
    // cout << dirname;
    // cheach if it is a valid path
    if (checkDir(abspath))
    {
        if (mkdir(dirname.c_str(), 0755) == -1)
        {
            perror("");
            return;
        }
    }
    else
    {
        cout << "Given destination is not a directory";
    }
}
void renameFile(vector<string> cmd)
{
    string old = preProcess(cmd[1]);
    string newf = preProcess(cmd[2]);
    if (!(rename(old.c_str(), newf.c_str())))
    {
        perror("Error");
    }
}
bool search(string cur, string filetoSearch)
{
    // cout<<
    DIR *dir;
    struct dirent *diread;
    if ((dir = opendir(cur.c_str())) != nullptr)
    {
        while ((diread = readdir(dir)) != nullptr)
        {
            
            if (!(strcmp(diread->d_name, ".")) || !(strcmp(diread->d_name, "..")))
            {
                //  cout << "inside"
                    //  << " " << diread->d_name << endl;
                continue;
            }
            string nextD = cur + "/" + diread->d_name;
            if (!checkDir(nextD))
            {
                if (diread->d_name == filetoSearch)
                {
                    closedir(dir);
                    return true;
                }
            }
            else
            {
                if (diread->d_name == filetoSearch)
                {
                    closedir(dir);
                    return true;
                }
                // cout<<"going to "<<nextD<<endl;
                // closedir(dir);
                if(search(nextD, filetoSearch))
                    return true;

            }
        }
        closedir(dir);
    }
    else
    {
        perror("opendir");
        return EXIT_FAILURE;
    }
    return false;
}
void removeFileandDir(string path)
{
    DIR *dir;
    struct dirent *diread;
    if (!checkDir(path))
    {
        remove(path.c_str());
        return;
    }
    if ((dir = opendir(path.c_str())) != nullptr)
    {
        while ((diread = readdir(dir)) != nullptr)
        {
            string nextD = path + "/" + diread->d_name;
            if (!(strcmp(diread->d_name, ".")) || !(strcmp(diread->d_name, "..")))
            {
                cout << "inside"
                     << " " << diread->d_name << endl;
                continue;
            }

            if (!checkDir(nextD))
            {
                cout << "removing"
                     << " " << diread->d_name << endl;
                if (remove(nextD.c_str()) != 0)
                    printf("Error: unable to delete the file");
            }
            else
            {
                cout << "going to"
                     << " " << diread->d_name << endl;
                removeFileandDir(nextD);
            }
        }
        closedir(dir);
        remove(path.c_str());
    }
    else
    {
        perror("opendir");
    }
}
void copyfiles(vector<string> files)
{
    string desPath = preProcess(files[files.size() - 1]);
    cout << "des: " << desPath << endl;
    if (!checkDir(desPath))
    {
        cout << "Directory does not exists\n";
        return;
    }

    for (int i = 1; i < files.size() - 1; i++)
    {
        string SfilePath = preProcess(files[i]);
        int j=0;
        for (j = SfilePath.size() - 1; j >= 0; j--)
        {
            if (SfilePath[j] == '/')
                break;
        }
        int len = files[j].size() - j - 1;
        string DfilePath = desPath + "/" + SfilePath.substr(j + 1, len);
        cout << "filenewpath " <<" "<<i<<" "<< DfilePath << endl;
        int source = open(SfilePath.c_str(), O_RDONLY, 0);
        int dest = open(DfilePath.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        struct stat stat_source;
        fstat(source, &stat_source);
        sendfile(dest, source, 0, stat_source.st_size);
        chmod(DfilePath.c_str(), stat_source.st_mode);
        chown(DfilePath.c_str(), stat_source.st_uid, stat_source.st_gid);
        close(source);
        close(dest);
    }
}
void moveFiles(vector<string> files)
{
    copyfiles(files);
    for (int i = 1; i < files.size() - 1; i++)
    {
        removeFileandDir(files[i]);
    }
}
void commandMode()
{
    vector<string> cmd;
    string line, word;
    getline(cin, line);
    istringstream iss(line);
    while (iss >> word)
    {
        cmd.push_back(word);
    }
    // cin >> cmd[0];
    if (cmd[0] == "cf")
    {
        // cin >> cmd[1] >> cmd[2];
        create_file(cmd);
    }
    else if (cmd[0] == "cd")
    {
        // cin >> cmd[1] >> cmd[2];
        create_dir(cmd);
    }
    else if (cmd[0] == "ren")
    {
        // cin >> cmd[1] >> cmd[2];
        renameFile(cmd);
    }
    else if (cmd[0] == "se")
    {
        // cin >> cmd[1];
        if (search(get_cwd(), cmd[1]))
            cout << true << endl;
        else
            cout << false << endl;
    }
    else if (cmd[0] == "de")
    {
        // cin >> cmd[1];
        string path = preProcess(cmd[1]);
        // cout << path;
        removeFileandDir(path);
        cout << "removing finally" << endl;
    }
    else if (cmd[0] == "cp")
    {
        copyfiles(cmd);
    }
    else if (cmd[0] == "mv")
    {
        moveFiles(cmd);
    }
}

int main()
{
    commandMode();
}
