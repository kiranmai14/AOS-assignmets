#include "headers.h"
using namespace std;

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
// int main(int argc, char *argv[])
int main()
{
    int argc  = 3;
    string argv[] = {"./tracker","tracker_file.txt","1"};
    int port1, port2, tracker_no;
    string ip1, ip2;
    // for checking the arguments
    if (argc < 3)
    {
        cout << "Insufficiet arguments" << endl;
        exit(-1);
    }
    // tracker_no = convertToInt(argv[2]); //for command line arguments
    tracker_no = convertToInt(argv[2]); // for debugging purpose



    // to open the file tracker_file.txt and assign port and ip
    FILE *fp;
    // fp = fopen(argv[1], "r"); //for command line arguments
    fp = fopen((const char *)argv[1].c_str(), "r"); // for debugging purpose
    if (fp)
    {
        char c;
        string p = "";
        vector<string> v;
        for (char c = getc(fp); c != EOF; c = getc(fp))
        {
            if (c == ' ')
            {
                v.push_back(p);
                p = "";
                continue;
            }
            p = p + c;
        }
        v.push_back(p);
        p = "";
        fclose(fp);
        ip1 = v[0];
        port1 = convertToInt(v[1]);
        ip2 = v[2];
        port2 = convertToInt(v[3]);
    }
    else
    {
        cout << "Unable top open file" << endl;
        exit(-1);
    }
    // cout<<ip1<<" "<<port1<<" "<<ip2<<" "<<port2;
    return 0;
}