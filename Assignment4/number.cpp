#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    fstream file("numbers.txt",ios::out);
    for (int i = 0; i < 1300; i++)
    {
        file << i << " ";
    }
    file.close();
}