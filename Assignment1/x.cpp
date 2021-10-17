#include "headers.h"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define gotoxy(x, y) printf("\033[%d;%dH", (x), (y))
#define ERASE cout << "\033[2K"
#define BACK_SPACE cout << "\033[" << 1 << "D"
using namespace std;
string root;
string cwd;
string home;
string comwd;
stack<string> lef, rig;
struct termios orig_termios;
// to get home of the system
void getHome()
{
    struct passwd *pw = getpwuid(getuid());
    string homedir = pw->pw_dir;
    home = homedir;
}
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
    char *permval = (char *)malloc(sizeof(char) * 10 + 1);
    permval[0] = (S_ISDIR(perm)) ? 'd' : '-';
    permval[1] = (perm & S_IRUSR) ? 'r' : '-';
    permval[2] = (perm & S_IWUSR) ? 'w' : '-';
    permval[3] = (perm & S_IXUSR) ? 'x' : '-';
    permval[4] = (perm & S_IRGRP) ? 'r' : '-';
    permval[5] = (perm & S_IWGRP) ? 'w' : '-';
    permval[6] = (perm & S_IXGRP) ? 'x' : '-';
    permval[7] = (perm & S_IROTH) ? 'r' : '-';
    permval[8] = (perm & S_IWOTH) ? 'w' : '-';
    permval[9] = (perm & S_IXOTH) ? 'x' : '-';
    permval[10] = '\0';
    return permval;
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
    int div = 0;
    static const char *SIZES[] = {"B", "KB", "MB", "GB"};
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
// to get details of the file
bool getDetailsOfFiles(vector<string> files, vector<vector<string>> &filesWithdetails)
{
    struct stat fileInfo;
    for (int i = 0; i < files.size(); i++)
    {
        string relPath = files[i];
        string absPath;
        if ((root > cwd) || (root < cwd))
        {
            absPath = cwd;
            absPath = absPath + "/";
        }
        else
        {
            absPath = relPath;
        }
        if (stat(absPath.c_str(), &fileInfo) != 0) // Use stat() to get the info
        {
            std::cerr << "Error: " << strerror(errno) << '\n';
            return (EXIT_FAILURE);
        }
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
string preProcess(string path)
{
    string absPath = "";
    if (path[0] == '~') ///\directory from where the application started
    {
        absPath = home + path.substr(1, path.size() - 1);
    }
    else if (path == ".") // current directory
    {
        absPath = comwd;
        absPath = absPath + path.substr(1, path.size() - 1);
    }
    else if (path[0] == '/') ///\directory from where the application started
    {
        absPath = home + path;
    }
    else
    {
        absPath = comwd;
        absPath = absPath + "/" + path;
    }
    return absPath;
}
void create_file(vector<string> cmd)
{
    string filename = cmd[0];
    string abspath = preProcess(cmd[1]);
    filename = abspath + "/" + filename;
    if (checkDir(abspath))
    {
        int fd1 = open(filename.c_str(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd1 < 0)
        {
            perror("c1");
            exit(1);
        }
        if (close(fd1) < 0)
        {
            perror("c1");
            exit(1);
        }
    }
    else
    {
        cout << "Given destination is not a directory";
    }
}
void create_dir(vector<string> cmd)
{
    string dirname = cmd[0];
    string abspath = preProcess(cmd[1]);
    dirname = abspath + "/" + dirname;
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
    string old = preProcess(cmd[0]);
    string newf = preProcess(cmd[21]);
    if ((rename(old.c_str(), newf.c_str())))
    {
        perror("Error");
    }
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
bool search(string cur, string filetoSearch)
{
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
                if (search(nextD, filetoSearch))
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
void copyFile(string SfilePath, string DfilePath)
{
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
void copyDirectory(string SfilePath, string DfilePath)
{

    DIR *dir;
    struct dirent *diread;
    if (mkdir(DfilePath.c_str(), 0755) == -1)
    {
        perror("");
        return;
    }
    struct stat stat_source;
    if (stat((const char *)SfilePath.c_str(), &stat_source) != 0) // Use stat() to get the info
    {
        std::cerr << "Error: " << strerror(errno) << '\n';
    }
    chmod(DfilePath.c_str(), stat_source.st_mode);
    chown(DfilePath.c_str(), stat_source.st_uid, stat_source.st_gid);
    if ((dir = opendir(SfilePath.c_str())) != nullptr)
    {
        while ((diread = readdir(dir)) != nullptr)
        {
            string SnextD = SfilePath + "/" + diread->d_name;
            string DnextD = DfilePath + "/" + diread->d_name;
            if (!(strcmp(diread->d_name, ".")) || !(strcmp(diread->d_name, "..")))
            {
                cout << "inside"
                     << " " << diread->d_name << endl;
                continue;
            }
            if (!checkDir(SnextD))
            {
                cout << "copying"
                     << " " << diread->d_name << endl;

                // if (remove(nextD.c_str()) != 0)
                //     printf("Error: unable to delete the file");
                copyFile(SnextD, DnextD);
            }
            else
            {
                copyDirectory(SnextD, DnextD);
            }
        }
        closedir(dir);
    }
    else
    {
        perror("opendir");
    }
}
void copyFileandDir(vector<string> files)
{
    string desPath = preProcess(files[files.size() - 1]);
    // cout << "des: " << desPath << endl;
    if (!checkDir(desPath))
    {
        cout << "Directory does not exists\n";
        return;
    }
    for (int i = 0; i < files.size() - 1; i++)
    {
        string SfilePath = preProcess(files[i]);
        int j = 0;
        for (j = SfilePath.size() - 1; j >= 0; j--)
        {
            if (SfilePath[j] == '/')
                break;
        }
        int len = files[j].size() - j - 1;
        string DfilePath = desPath + "/" + SfilePath.substr(j + 1, len);
        // cout << "filenewpath "
        //      << " " << i << " " << DfilePath << endl;
        if (checkDir(SfilePath))
        {
            copyDirectory(SfilePath, DfilePath);
        }
        else
        {
            copyFile(SfilePath, DfilePath);
        }
    }
}
void moveFiles(vector<string> files)
{
    copyFileandDir(files);
    for (int i = 0; i < files.size() - 1; i++)
    {
        removeFileandDir(files[i]);
    }
}
void executeCommand(vector<string> cmds)
{
    string cmd = cmds[0];
    vector<string> files;
    for (int i = 1; i < cmds.size(); i++)
        files.push_back(cmds[i]);
    if (cmd == "cf")
    {
        if (files.size() < 2)
        {
            cout << "2 parameters needed";
        }
        create_file(files);
    }
    else if (cmd == "cd")
    {
        if (files.size() < 2)
        {
            cout << "2 parameters needed";
        }
        create_dir(files);
    }
    else if (cmd == "ren")
    {
        if (files.size() < 2)
        {
            cout << "2 parameters needed";
        }
        renameFile(files);
    }
    else if (cmd == "se")
    {
        if (files.size() < 1)
        {
            cout << "1 parameters needed";
        }
        if (search(get_cwd(), files[0]))
            cout << "Found";
        else
            cout << "Not found";
    }
    else if (cmd == "delete_dir" || cmd == "delete_file")
    {
        if (files.size() < 1)
        {
            cout << "1 parameters needed";
        }
        string path = preProcess(files[0]);
        removeFileandDir(path);
    }

    else if (cmd == "cp")
    {
        copyFileandDir(files);
    }
    else if (cmd == "mv")
    {
        moveFiles(files);
    }
    else if (cmd == "goto")
    {
        string path = preProcess(files[0]);
        comwd = path;
    }
    else
    {
        cout << "Not a valid command";
    }
}
void splitString(string line, vector<string> &cmd)
{
    string word;
    // getline(cin, line);
    istringstream iss(line);
    while (iss >> word)
    {
        cmd.push_back(word);
    }
}
bool commandMode()
{
    comwd = cwd;
    vector<unsigned short> winSize = getWindowSize();
    gotoxy(winSize[0] - 1, 1);
    ERASE;
    cout << "----COMMAND MODE----";
    gotoxy(winSize[0], 1);
    ERASE;
    string command;
    vector<string> cmds;
    while (true)
    {
        fflush(0);
        char inp[3];
        memset(inp, 0, 3 * sizeof(inp[0]));
        if (read(STDIN_FILENO, &inp, 3) == 0)
        {
            continue;
        }
        if ((inp[2] == 66 || inp[2] == 65 || inp[2] == 67 || inp[2] == 68) && inp[0] == 27 && inp[1] == 91)
        {
            continue;
        }
        else if (inp[0] == 'q')
        {
            return true;
        }
        else if (inp[0] == 27)
        {
            ERASE;
            gotoxy(winSize[0] - 1, 1);
            ERASE;
            break;
        }
        else if (inp[0] == 127)
        {
            if (command.size() != 0)
            {
                command.pop_back();
                BACK_SPACE;
                cout << " ";
                BACK_SPACE;
            }
        }
        else if (inp[0] == 10)
        {
            splitString(command, cmds);

            ERASE;
            gotoxy(winSize[0], 1);
            executeCommand(cmds);
            command.clear();
        }
        else
        {
            cout << inp[0];
            command.push_back(inp[0]);
        }
    }
    return false;
}
bool normalMode(string wd)
{
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
        else if (inp[0] == 10 && dupcur >= 1 && dupcur <= capacity) // enter key
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
                if (wd == "/")
                    continue;
                string absPath = wd;
                int i = 0;
                for (i = absPath.size() - 1; i >= 0; i--)
                {
                    if (absPath[i] == '/')
                        break;
                }
                int len = absPath.size() - i;
                absPath.erase(i, len);
                if (absPath.size() == 0)
                    absPath = "/";
                lef.push(string(cwd));
                cwd = absPath;
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
                if (checkDir(absPath))
                {
                    lef.push(string(cwd));
                    cwd = absPath;
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
                        execl("/usr/bin/vi", "vi", (const char *)absPath.c_str(), (char *)0);
                        exit(1);
                    }
                    else
                    {
                        wait(0);
                    }
                }
            }
            dupcur++;
        }
        else if (inp[2] == 'C') // right arrow
        {
            if (rig.empty())
                continue;
            else
            {
                lef.push(string(cwd));
                string pathtoright = rig.top();
                rig.pop();
                cwd = pathtoright;
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
            if (lef.empty())
                continue;
            else
            {
                rig.push(string(cwd));
                string pathtoleft = lef.top();
                lef.pop();
                cwd = pathtoleft;
                cout << pathtoleft << " ";
                if (normalMode(pathtoleft))
                {
                    disableRawMode();
                    cout << "\033[2J\033[1;1H";
                    return true;
                }
            }
        }
        else if (inp[0] == 'h')
        {
            lef.push(cwd);
            cwd = home;
            if (normalMode(cwd))
            {
                disableRawMode();
                cout << "\033[2J\033[1;1H";
                return true;
            }
        }
        else if (inp[0] == 127)
        {
            if (wd == "/")
                continue;
            string absPath = wd;
            int i = 0;
            for (i = absPath.size() - 1; i >= 0; i--)
            {
                if (absPath[i] == '/')
                    break;
            }
            int len = absPath.size() - i;
            absPath.erase(i, len);
            if (absPath.size() == 0)
                absPath = "/";
            lef.push(cwd);
            cwd = absPath;
            if (normalMode(absPath))
            {
                disableRawMode();
                cout << "\033[2J\033[1;1H";
                return true;
            }
        }
        else if (inp[0] == ':')
        {

            if (commandMode())
            {
                disableRawMode();
                cout << "\033[2J\033[1;1H";
                return true;
            }
            else
            {
                gotoxy(winSize[0] + 1, 1);
                cout << "----NORMAL MODE----";
                gotoxy(1, 1);
                // enableRawMode();
            }
        }
        else if (inp[0] == 'q')
        {
            disableRawMode();
            cout << "\033[2J\033[1;1H";
            return true;
        }
    }
}
int main()
{

    enableRawMode();
    string wd = get_cwd();
    getHome();
    root = wd;
    cwd = wd;
    normalMode(wd);
}
