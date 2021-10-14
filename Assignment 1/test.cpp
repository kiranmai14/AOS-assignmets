// // // #include <iostream>
// // // #include <ctime>
// #include <sys/types.h>
// #include <unistd.h>
// #include <cerrno>
// // // #include <cstring>
// // // #include <pwd.h>
// // // #include <grp.h>

// // // char* permissions(char *file){
// // //     struct stat st;
// // //     char *modeval = (char *)malloc(sizeof(char) * 9 + 1);
// // //     if(stat(file, &st) == 0){
// // //         mode_t perm = st.st_mode;
// // //         modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
// // //         modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
// // //         modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
// // //         modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
// // //         modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
// // //         modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
// // //         modeval[6] = (perm & S_IROTH) ? 'r' : '-';
// // //         modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
// // //         modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
// // //         modeval[9] = '\0';
// // //         return modeval;     
// // //     }
// // //     else{
// // //         return strerror(errno);
// // //     }   
// // // }
// // // int main(int argc, char **argv)
// // // {
// // //     struct stat fileInfo;

// // //     if (argc < 2)
// // //     {
// // //         std::cout << "Usage: fileinfo <file name>\n";
// // //         return (EXIT_FAILURE);
// // //     }

// // //     if (stat(argv[1], &fileInfo) != 0)
// // //     { // Use stat() to get the info
// // //         std::cerr << "Error: " << strerror(errno) << '\n';
// // //         return (EXIT_FAILURE);
// // //     }

// // //     std::cout << "Type:         : ";
// // //     if ((fileInfo.st_mode & S_IFMT) == S_IFDIR)
// // //     { // From sys/types.h
// // //         std::cout << "Directory\n";
// // //     }
// // //     else
// // //     {
// // //         std::cout << "File\n";
// // //     }

// // //     std::cout << "Size          : " << fileInfo.st_size << '\n';              // Size in bytes
// // //     std::cout << "Device        : " << (char)(fileInfo.st_dev + 'A') << '\n'; // Device number
// // //     std::cout << "Created       : " << std::ctime(&fileInfo.st_ctime);        // Creation time
// // //     std::cout << "Modified      : " << std::ctime(&fileInfo.st_mtime);        // Last mod time
// // //     std::cout << "userid          : " << fileInfo.st_mode << '\n';              // Size in bytes
// // // // struct stat info;
// // // // stat(filename, &info);  // Error check omitted
// // // struct passwd *pw = getpwuid(fileInfo.st_uid);
// // // struct group  *gr = getgrgid(fileInfo.st_gid);
// // // printf("%s",gr->gr_name);
// // // printf("%s\n",permissions(argv[1]));
// // // }
// // // // #include <iostream>
// // // // #include <string>
// // // // #include <sstream>
// // // // #include <sys/stat.h>
// // // //  /* for stat() function */

// // // // using namespace std;

// // // // // Utility functions:

// // // // string convertToString(double num) {
// // // //     ostringstream convert;
// // // //     convert << num;
// // // //     return convert.str();
// // // // }

// // // // double roundOff(double n) {
// // // //     double d = n * 100.0;
// // // //     int i = d + 0.5;
// // // //     d = (float)i / 100.0;
// // // //     return d;
// // // // }

// // // // string convertSize(size_t size) {              
// // // //     static const char *SIZES[] = { "B", "KB", "MB", "GB" };
// // // //     int div = 0;
// // // //     size_t rem = 0;

// // // //     while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
// // // //         rem = (size % 1024);
// // // //         div++;
// // // //         size /= 1024;
// // // //     }

// // // //     double size_d = (float)size + (float)rem / 1024.0;
// // // //     string result = convertToString(roundOff(size_d)) + " " + SIZES[div];
// // // //     return result;
// // // // }

// // // // int file_size(const char *path) {
// // // //     struct stat results;

// // // //     if (stat(path, &results) == 0) {
// // // //         return results.st_size;
// // // //     } else {
// // // //         return -1;
// // // //     }
// // // // }

// // // // // This is the function that you will call:
// // // // string getFileSize(string path) {
// // // //     size_t size = file_size((const char *)path.c_str());
// // // //     return convertSize(size);
// // // // }

// // // // // Example program:
// // // // int main() {
// // // //     cout << getFileSize("/home/kiranmai/IIIT/AOS/Assignment 1/a.cpp") << endl;
// // // //     return 0;
// // // // }
// #include <iostream>
// #include <stdio.h>
// #include <termios.h>
// // #define STDIN_FILENO 0
// // #define getch() _getch()
// using namespace std;
// int main(int argc, char **argv)
// {
// // int main()
// // {
// // // Black magic to prevent Linux from buffering keystrokes.
// //     const int KEY_ARROW_CHAR1 = 224;
// // const int KEY_ARROW_UP = 72;
// // const int KEY_ARROW_DOWN = 80;
// // const int KEY_ARROW_LEFT = 75;
// // const int KEY_ARROW_RIGHT = 77;

// // unsigned char ch1 = getc(stdin);
// // if (ch1 == KEY_ARROW_CHAR1)
// // {
// //     // Some Arrow key was pressed, determine which?
// //     unsigned char ch2 = getc(stdin);
// //     switch (ch2) 
// //     {
// //     case KEY_ARROW_UP:
// //         // code for arrow up
// //         cout << "KEY_ARROW_UP" << endl;
// //         break;
// //     case KEY_ARROW_DOWN:
// //         // code for arrow down
// //         cout << "KEY_ARROW_DOWN" << endl;
// //         break;
// //     case KEY_ARROW_LEFT:
// //         // code for arrow right
// //         cout << "KEY_ARROW_LEFT" << endl;
// //         break;
// //     case KEY_ARROW_RIGHT:
// //         // code for arrow left
// //         cout << "KEY_ARROW_RIGHT" << endl;
// //         break;
// //     }
// // }
// // else
// // {
// //     switch (ch1)
// //     {
// //         // Process other key presses if required.
// //     }
// // }
// // char c;
// // int n =3;
// // for (int i=0; i<n; i++){
// //     c = getc(stdin);
// //     // fflushstdin ();
// //     if (c == '\n'){
// //         //do som;ething
// //         cout<<"x";
// //     }
// // }
// int pid = fork();
// if (pid == 0) {
//   execl("/usr/bin/xdg-open", "xdg-open", argv[1], (char *)0);
//   exit(1);
  #include <sys/ioctl.h>
#include <stdio.h>

int main (void)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    printf ("lines %d\n", w.ws_row);
    printf ("columns %d\n", w.ws_col);
    return 0;

}