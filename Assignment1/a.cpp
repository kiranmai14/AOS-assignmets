

#include "headers.h"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define gotoxy(x, y) printf("\033[%d;%dH", (x), (y))
using namespace std;
char root[4096];
char cwd[4096];

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
    {
        cout << "line 30";
        perror("scandir");
    }

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
        char *relPath = (char *)files[i].c_str();
        char *absPath = new char[256];
        if ((strcmp(root, cwd) > 0) || (strcmp(root, cwd) < 0))
        {
            strcpy(absPath, cwd);
            strcat(absPath, "/");
        }
        else
        {
            strcpy(absPath, relPath);
        }
        if (stat(absPath, &fileInfo) != 0) // Use stat() to get the info
        {
            cout << "line 151";
            std::cerr << "Error: " << strerror(errno) << '\n';
            return (EXIT_FAILURE);
        }
        //  cout<<files[i]<<" ";
        // string fileName = files[i];
        vector<string> vect;
        vect.push_back(relPath);
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
        cout << "line 30";
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
    // cout << cwd << " ";
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
    winSize[0] = winSize[0] - 1;
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
    printf("\033[%d;%dH", winSize[0] + 1, 0);
    cout << "----NORMAL MODE----";
    printf("\033[%d;%dH", 0, 0);
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
                printf("\033[%d;%dH", winSize[0] + 1, 0);
                cout << "----NORMAL MODE----";
                printf("\033[%d;%dH", 0, 0);
                printDetails(x - 1, y - 1, filesWithdetails);
                printf("\033[%d;%dH", 0, 0);
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
                printf("\033[%d;%dH", winSize[0] + 1, 0);
                cout << "----NORMAL MODE----";
                printf("\033[%d;%dH", 0, 0);
                printDetails(x - 1, y - 1, filesWithdetails);
                printf("\033[%d;%dH", 0, 0);
                cursor++;
                gotoxy(cursor, 1);
            }
        }
        else if (inp[0] == 'l' && overflow && cursor == capacity && y < filesWithdetails.size())
        {
            x++;
            y++;
            cout << "\033[2J\033[1;1H";
            printf("\033[%d;%dH", winSize[0] + 1, 0);
            cout << "----NORMAL MODE----";
            printf("\033[%d;%dH", 0, 0);
            printDetails(x - 1, y - 1, filesWithdetails);
            printf("\033[%d;%dH", 0, 0);
            gotoxy(cursor, 1);
        }
        else if (inp[0] == 'k' && overflow && cursor == 1 && x > 1)
        {
            x--;
            y--;
            cout << "\033[2J\033[1;1H";
            printf("\033[%d;%dH", winSize[0] + 1, 0);
            cout << "----NORMAL MODE----";
            printf("\033[%d;%dH", 0, 0);
            printDetails(x - 1, y - 1, filesWithdetails);
            printf("\033[%d;%dH", 0, 0);
            gotoxy(cursor, 1);
        }
        else if (inp[0] == 10 && dupcur >= 1 && dupcur <= capacity)
        {

            int index;
            if (offset == 1)
            {
                index = cursor - 1;
            }
            else if (offset > 1 && cursor % offset == 1)
            {
                index = x - 1 + (cursor / offset);
            }
            else
                continue;
            if (files[index] == ".")
                continue;
            else if (files[index] == "..")
            {
                string absPath = wd;
                int i = 0;
                for (i = absPath.size() - 1; i >= 0; i--)
                {
                    if (absPath[i] == '/')
                        break;
                }
                int len = absPath.size() - i;
                // cout << wd;
                absPath.erase(i, len);
                // rig.push(lef.top());
                // lef.pop();
                rig.push(absPath);
                strcpy(cwd, absPath.c_str());
                if (normalMode(absPath))
                {
                    disableRawMode();
                    cout << "\033[2J\033[1;1H";
                    return true;
                }
            }
            else
            {
                string absPath = wd;
                absPath = absPath + "/" + files[index];
                cout << absPath << " ";
                if (checkDir(absPath))
                {

                    rig.push(absPath);
                    strcpy(cwd, absPath.c_str());
                    if (normalMode(absPath))
                    {
                        disableRawMode();
                        cout << "\033[2J\033[1;1H";
                        return true;
                    }
                }
                else
                {
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        // cout << files[index];
                        execl("/usr/bin/xdg-open", "xdg-open", (const char *)absPath.c_str(), (char *)0);
                        exit(1);
                    }
                }
            }
            dupcur++;
        }
        else if (inp[2] == 'C') // right arrow
        {
            if (lef.empty() || lef.size() <= 1)
                continue;
            else
            {
                // rig.push(lef.top());
                rig.push(lef.top());
                lef.pop();
                char *pathtoright = (char *)lef.top().c_str();
                strcpy(cwd, pathtoright);
                cout << pathtoright << " ";
                if (normalMode(pathtoright))
                {
                    disableRawMode();
                    cout << "\033[2J\033[1;1H";
                    return true;
                }
            }
        }
        else if (inp[2] == 'D') // left arrow
        {
            if (rig.empty() || rig.size() <= 1)
                continue;
            else
            {
                // rig.push(lef.top());
                lef.push(rig.top());
                rig.pop();
                char *pathtoright = (char *)rig.top().c_str();
                cout << pathtoright << " ";
                strcpy(cwd, pathtoright);
                if (normalMode(pathtoright))
                {
                    disableRawMode();
                    cout << "\033[2J\033[1;1H";
                    return true;
                }
            }
        }
        else if (inp[0] == 'h')
        {
            rig.push(root);
            strcpy(cwd, (char *)root);
            if (normalMode(cwd))
            {
                disableRawMode();
                cout << "\033[2J\033[1;1H";
                return true;
            }
        }
        else if (inp[0] == 127)
        {

            string absPath = wd;
            int i = 0;
            for (i = absPath.size() - 1; i >= 0; i--)
            {
                if (absPath[i] == '/')
                    break;
            }
            int len = absPath.size() - i;
            // cout << wd;
            absPath.erase(i, len);
            // rig.push(lef.top());
            // lef.pop();
            rig.push(absPath);
            strcpy(cwd, absPath.c_str());
            if (normalMode(absPath))
            {
                disableRawMode();
                cout << "\033[2J\033[1;1H";
                return true;
            }
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
    strcpy(cwd, root);
    lef.push(wd);
    rig.push(wd);
    normalMode(wd);
}
