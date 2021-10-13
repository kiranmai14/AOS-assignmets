#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <algorithm>
using namespace std;

// to get the current working directory
char *get_cwd()
{
    char *tmp = (char *)malloc(256 * sizeof(char));
    getcwd(tmp, 256);
    return tmp;
}
//  sort the list of directories
void sortFiles(struct dirent **diread, vector<char *> &files)
{
    int n, i;
    n = scandir(".", &diread, 0, versionsort);
    if (n < 0)
        perror("scandir");
    else
    {
        for (i = 0; i < n; ++i)
        {
            files.push_back(diread[i]->d_name);
            // printf("%s\n", diread[i]->d_name);
            // free(diread[i]);
        }
        // free(diread);
    }
}
bool getFiles(char *wd, vector<char *> &files)
{
    DIR *dir;
    struct dirent **diread;

    // if ((dir = opendir(wd)) != nullptr)
    // {
    //     while ((*diread = readdir(dir)) != nullptr)
    //     {
    //         files.push_back((*diread)->d_name);
    //     }
    //     closedir(dir);
    // }
    // else
    // {
    //     perror("opendir");
    //     return EXIT_FAILURE;
    // }
    sortFiles(diread,files);
    return 0;
}

int main()
{
    char *wd = get_cwd();
    vector<char *> files; //directory files are stored in this variable
    getFiles(wd,files);
    for (auto file : files) cout << file << endl;
    return 0;
}