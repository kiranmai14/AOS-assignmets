#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <string>
#include <pwd.h>
#include <grp.h>
#include <iomanip> //for setw
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
using namespace std;

// to get the current working directory
char *get_cwd()
{
    char *tmp = (char *)malloc(256 * sizeof(char));
    getcwd(tmp, 256);
    return tmp;
}
//  get the list of directories and sort the list of directories
vector<string> getAndSortFiles()
{
    vector<string> files;
    DIR *dir;
    struct dirent **diread;
    int n, i;
    n = scandir(".", &diread, 0, versionsort);
    if (n < 0)
        perror("scandir");
    else
    {
        for (i = 0; i < n; ++i)
        {
            files.push_back(diread[i]->d_name);
            free(diread[i]);
        }
        free(diread);
    }
    return files;
}
// get remissions in human readable form
char *permissions(mode_t perm)
{
    char *modeval = (char *)malloc(sizeof(char) * 10 + 1);
    modeval[0] = (S_ISDIR(perm)) ? 'd' : '-';
    modeval[1] = (perm & S_IRUSR) ? 'r' : '-';
    modeval[2] = (perm & S_IWUSR) ? 'w' : '-';
    modeval[3] = (perm & S_IXUSR) ? 'x' : '-';
    modeval[4] = (perm & S_IRGRP) ? 'r' : '-';
    modeval[5] = (perm & S_IWGRP) ? 'w' : '-';
    modeval[6] = (perm & S_IXGRP) ? 'x' : '-';
    modeval[7] = (perm & S_IROTH) ? 'r' : '-';
    modeval[8] = (perm & S_IWOTH) ? 'w' : '-';
    modeval[9] = (perm & S_IXOTH) ? 'x' : '-';
    modeval[10] = '\0';
    return modeval;
}
// get userid
char *getUserId(uid_t st)
{
    struct passwd *pw = getpwuid(st);
    if (pw != 0)
        return pw->pw_name;
    else
        return nullptr;
}
char *getGroupId(gid_t st)
{
    struct group *gr = getgrgid(st);
    if (gr != 0)
        return gr->gr_name;
    else
        return nullptr;
}
// to get the size in human readable form
string convertToString(double num)
{
    ostringstream convert;
    convert << num;
    return convert.str();
}

double roundOff(double n)
{
    double d = n * 100.0;
    int i = d + 0.5;
    d = (float)i / 100.0;
    return d;
}
string convertSize(size_t size)
{
    static const char *SIZES[] = {"B", "KB", "MB", "GB"};
    int div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES))
    {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    double size_d = (float)size + (float)rem / 1024.0;
    string result = convertToString(roundOff(size_d)) + " " + SIZES[div];
    return result;
}
// get details of single file
bool getFileDetails(string file)
{
    vector<string> vect;
    struct stat fileInfo;
    if (stat((const char *)file.c_str(), &fileInfo) != 0) // Use stat() to get the info
    {
        std::cerr << "Error: " << strerror(errno) << '\n';
        return (EXIT_FAILURE);
    }
    else
    {
        vect.push_back(file);
        vect.push_back(convertSize(fileInfo.st_size));
        vect.push_back(getUserId(fileInfo.st_uid));
        vect.push_back(getGroupId(fileInfo.st_gid));
        vect.push_back(permissions(fileInfo.st_mode));
        vect.push_back(ctime(&fileInfo.st_mtime));
    }
    return true;
} // to get details of the file
bool getDetailsOfFiles(vector<string> files, vector<vector<string>> &filesWithdetails)
{
    struct stat fileInfo;
    for (int i = 0; i < files.size(); i++)
    {
        if (stat((const char *)files[i].c_str(), &fileInfo) != 0) // Use stat() to get the info
        {
            std::cerr << "Error: " << strerror(errno) << '\n';
            return (EXIT_FAILURE);
        }
        string fileName = files[i];
        vector<string> vect;
        vect.push_back(fileName);
        vect.push_back(convertSize(fileInfo.st_size));
        vect.push_back(getUserId(fileInfo.st_uid));
        vect.push_back(getGroupId(fileInfo.st_gid));
        vect.push_back(permissions(fileInfo.st_mode));
        vect.push_back(ctime(&fileInfo.st_mtime));
        filesWithdetails.push_back(vect);
    }
    return true;
}
vector<unsigned short> getWindowSize()
{
    vector<unsigned short> s;
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    s.push_back(w.ws_row);
    s.push_back(w.ws_col);
    return s;
}
int getOffset(int col)
{
    return ceil(125 / (double)col);
}
string formatDisplay(string s)
{
    string s1;
    if (s.size() > 13)
    {
        s1 = s.substr(0, 12);
        s1 = s1 + "...\0";
        return s1;
    }
    return s;
}
void printDetails(int f, int l, vector<vector<string>> file)
{
    for (int i = f; i <= l; i++)
    {
        cout << left << setw(23) << formatDisplay(file[i][0]);
        cout << left << setw(20) << file[i][1];
        cout << left << setw(20) << formatDisplay(file[i][2]);
        cout << left << setw(20) << formatDisplay(file[i][3]);
        cout << left << setw(18) << file[i][4];
        cout << left << setw(24) << file[i][5];
        cout<<"x";
    }
}
struct termios orig_termios;
void die(const char *s)
{
    perror(s);
    exit(1);
}
void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void normalMode()
{

    char *wd = get_cwd();
    vector<string> files; // directory files names are stored in this variable
    files = getAndSortFiles();
    vector<vector<string>> filesWithdetails;
    getDetailsOfFiles(files, filesWithdetails);
    cout << "\033[2J\033[1;1H";
    printDetails(0, 8, filesWithdetails);
    enableRawMode();
    vector<unsigned short> winSize = getWindowSize();
    cout << winSize[1] << " ";
    cout << getOffset(winSize[1]);
    char c;
    while (true)
    {
       
        printf("\033[%d;%dH", 0, 0);
        fflush(0);
        if (read(STDIN_FILENO, &c, 1) == 1 && c == 'q')
        {
            disableRawMode();
            cout << "\033[2J\033[1;1H";
            break;
        }
    }
}
int main()
{
    normalMode();
}