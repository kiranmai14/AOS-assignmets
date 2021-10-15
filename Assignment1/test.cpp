

#include "headers.h"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define gotoxy(x, y) printf("\033[%d;%dH", (x), (y))
using namespace std;
char root[256];
char cwd[256];
// to get the current working directory
char *get_cwd()
{
    char *tmp = (char *)malloc(256 * sizeof(char));
    getcwd(tmp, 256);
    return tmp;
}
//  get the list of directories and sort the list of directories
vector<string> getAndSortFiles(char *wd)
{
    vector<string> files;
    DIR *dir;
    struct dirent **diread;
    int n, i;
    n = scandir(wd, &diread, 0, versionsort);
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
    gotoxy(20, 4);
    cout << files.size() << endl;
    struct stat fileInfo;
    for (int i = 0; i < files.size(); i++)
    {
        char *x;
        if (strcmp(root, cwd) != 0)
        {
            strcpy(x, cwd);
            strcat(x,"/");
            strcat(x, (const char *)files[i].c_str());
            cout << x << " ";
            if (stat(x, &fileInfo) != 0) // Use stat() to get the info
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
            for (auto x : vect)
            {
                cout << x << endl;
            }
            filesWithdetails.push_back(vect);
            continue;
        }

        // cout << files[i] << " ";
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
        if (!file[i][5].empty() && file[i][5][file[i][5].length() - 1] == '\n')
        {

            file[i][5].erase(file[i][5].length() - 1);
        }
        cout << left << setw(24) << file[i][5];
        if (i != l)
            cout << endl;
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
bool checkDir(string file)
{
    struct stat fileInfo;
    if (stat((const char *)file.c_str(), &fileInfo) != 0)
    { // Use stat() to get the info
        std::cerr << "Error: " << strerror(errno) << '\n';
        return (EXIT_FAILURE);
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
stack<string> lef, rig;
bool normalMode(string wd)
{

    // char *wd = get_cwd();
    vector<string> files; // directory files names are stored in this variable

    files = getAndSortFiles((char *)wd.c_str());
    vector<vector<string>> filesWithdetails;
    getDetailsOfFiles(files, filesWithdetails); // this gets the details of all giles
    vector<unsigned short> winSize = getWindowSize();
    int cursor = 1; // to maintain position ehich is diplayed on the screen
    bool overflow = false;
    int x = 0, y = 0;
    int capacity = 0; // to track how many lines are occupied
    int offset = getOffset(winSize[1]);
    int dirsize = filesWithdetails.size();
    if (filesWithdetails.size() < winSize[0])
    {
        capacity = filesWithdetails.size();
        x = x + 1;
        y = y + capacity;
        if (y * offset > winSize[0])
        {
            y = winSize[0] / offset;
            overflow = true;
            capacity = y * offset;
        }
        else
        {
            capacity = capacity * offset;
        }
    }
    else
    {
        capacity = winSize[0];
        x = x + 1;
        y = y + capacity;
        overflow = true;
        if (y * offset > winSize[0])
        {
            y = winSize[0] / offset;
            overflow = true;
            capacity = y * offset;
        }
    }
    cout << "\033[2J\033[1;1H"; // clearing the screen
    printDetails(x - 1, y - 1, filesWithdetails);
    printf("\033[%d;%dH", 0, 0); // curesor at position (1,1)
    int dupcur = 0;
    while (true)
    {
        char inp[3];
        dupcur = cursor;
        memset(inp, 0, 3 * sizeof(inp[0]));
        fflush(0);
        if (filesWithdetails.size() > winSize[0])
        {
            overflow = true;
        }
        if (read(STDIN_FILENO, &inp, 3) == 0)
        {
            continue;
        }
        else if (inp[2] == 'A')
        {
            if (cursor == 1)
                continue;
            else
            {
                cout << "\033[2J\033[1;1H";
                printDetails(x - 1, y - 1, filesWithdetails);
                cursor--;
                gotoxy(cursor, 1);
            }
        }
        else if (inp[2] == 'B')
        {
            if (cursor == capacity)
                continue;
            else
            {
                cout << "\033[2J\033[1;1H";
                printDetails(x - 1, y - 1, filesWithdetails);
                cursor++;
                gotoxy(cursor, 1);
            }
        }
        else if (inp[0] == 'l' && overflow && cursor == capacity && y < filesWithdetails.size())
        {
            x++;
            y++;
            cout << "\033[2J\033[1;1H";
            printDetails(x - 1, y - 1, filesWithdetails);
            gotoxy(cursor, 1);
        }
        else if (inp[0] == 'k' && overflow && cursor == 1 && x > 1)
        {
            x--;
            y--;
            cout << "\033[2J\033[1;1H";
            printDetails(x - 1, y - 1, filesWithdetails);
            gotoxy(cursor, 1);
        }
        else if (inp[0] == 10 && dupcur >= 1 && dupcur <= capacity)
        {

            int index;
            // cout << capacity<<" "<<cursor<<" "<<offset<<" "<<index;
            if (offset == 1)
            {
                index = cursor - 1;
                
                if (checkDir(files[index]))
                {
                    if (files[index] == ".")
                        continue;
                    else if (files[index] == "..")
                    {
                        string dum = wd;
                        int i = 0;
                        for (i = dum.size() - 1; i >= 0; i--)
                        {
                            if (dum[i] == '/')
                                break;
                        }
                        int len = dum.size() - 1 - i;
                        cout << wd;
                        dum.erase(i, len);
                        rig.push(lef.top());
                        lef.pop();
                        lef.push(dum);
                        strcpy(cwd, (const char *)dum.c_str());
                        cout << dum;
                        if (normalMode(dum))
                        {
                            disableRawMode();
                            cout << "\033[2J\033[1;1H";
                            return true;
                        }
                    }
                    else
                    {
                        string dum = wd;
                        dum = dum + "/" + files[index];
                        strcpy(cwd, (const char *)dum.c_str());
                        cout << dum;
                        rig.push(dum);
                        if (normalMode(dum))
                        {
                            disableRawMode();
                            cout << "\033[2J\033[1;1H";
                            return true;
                        }
                    }
                }
                else
                {
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        cout << files[index];
                        execl("/usr/bin/xdg-open", "xdg-open", (const char *)files[index].c_str(), (char *)0);
                        exit(1);
                    }
                }
            }
            else if (offset > 1 && cursor % offset == 1)
            {
                index = x - 1 + (cursor / offset);
                if (checkDir(files[index]))
                {
                    // normalMode();
                }
                else
                {
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        execl("/usr/bin/xdg-open", "xdg-open", (const char *)files[index].c_str(), (char *)0);
                        exit(1);
                    }
                    // else
                    // {
                    //     int *c;
                    //     wait(c);
                    // }
                }
            }
            else
                continue;
            dupcur++;
        }
        else if (inp[0] == 'q')
        {
            disableRawMode();
            cout << "\033[2J\033[1;1H";
            return true;
            break;
        }
    }
}
int main()
{

    enableRawMode();
    char *wd = get_cwd();
    strcpy(root, wd);
    strcpy(cwd,root);
    lef.push(wd);
    normalMode(wd);
     printf("\033[%d;%dH", winSize[0], 0);
    cout << "----NORMAL MODE----";
    printf("\033[%d;%dH", 0, 0);
}
