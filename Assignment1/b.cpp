#include "headers.h"
using namespace std;
char *get_cwd()
{
    char *tmp = (char *)malloc(256 * sizeof(char));
    getcwd(tmp, 256);
    return tmp;
}
string preProcess(string path)
{
    string absPath = "";
    if (path[0] == '~')
    {
        absPath = path.substr(1, path.size() - 1);
    }
    else if (path[0] == '.' && path.size() == 1)
    {
        absPath = get_cwd();
    }
    else if (path[0] == '.' && path.size() >= 1)
    {
        absPath = get_cwd();
        for (int i = 1; i < path.size(); i++)
        {
            absPath = absPath + path[i];
        }
    }
    return absPath;
}
void create_file(vector<string> cmd)
{
    string filename = cmd[1];
    string abspath = preProcess(cmd[2]);
    filename = abspath + "/" + filename;
    cout << filename;
    // cheach if it is a valid path
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
void create_dir(vector<string> cmd)
{
    string dirname = cmd[1];
    string abspath = preProcess(cmd[2]);
    dirname = abspath + "/" + dirname;
    cout << dirname;
    // cheach if it is a valid path
    if (mkdir(dirname.c_str(), 0755) == -1)
    {
        perror("");
        return;
    }
}
void commandMode()
{
    vector<string> cmd(3);
    cin >> cmd[0] >> cmd[1] >> cmd[2];
    if (cmd[0] == "cf")
    {
        create_file(cmd);
    }
    else if(cmd[0] == "cd")
    {
        create_dir(cmd);
    }
}
int main()
{
    commandMode();
}