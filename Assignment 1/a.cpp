#include <iostream>
#include <unistd.h>
#include <dirent.h>
using namespace std;

// to get the current working directory
char *get_cwd()
{
    char *tmp = (char *)malloc(256 * sizeof(char));
    getcwd(tmp, 256);
    return tmp;
}
int main()
{
    char *wd = get_cwd();
    printf("%s", wd);
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(wd)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
        {
            printf("%s\n", ent->d_name);
        }
        closedir(dir);
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    // cout << "\033[H\033[2J\033[3J"; // step1: clearing terminal screen
    // string key;
    // while (1)
    // {
    //     cout << "\033[H\033[2J\033[3J";
    //     cin >> key;
    //     if (key == "p")
    //         break;
    // }
    return 0;
}