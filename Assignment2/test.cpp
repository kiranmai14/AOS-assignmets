#include <openssl/sha.h>
#include <iostream>
#include <bits/stdc++.h>
#include "headers.h"

using namespace std;

void getSHA(string filepath)
{
    vector<string> piecewiseSHA;
    FILE *fp = NULL;
    fp = fopen(filepath.c_str(), "r+");
    if (fp == NULL)
    {
        perror("");
        cout << "ERROR" << endl;
        exit(-1);
    }
    // getting file size
    struct stat statbuf;
    stat(filepath.c_str(), &statbuf);
    intmax_t len = (intmax_t)statbuf.st_size;

    long chunks = (len / (512*1024));
    cout << len << "   " << chunks << endl;
    if (len % CHUNK_SIZE != 0)
        chunks = chunks + 1;

    unsigned char sha_of_file[20];
    unsigned char file_binary[CHUNK_SIZE];

    bzero(file_binary, sizeof(file_binary));
    bzero(sha_of_file, sizeof(sha_of_file));

    char encryptedText[40];
    int n = 0;
    // for (int j = 0; j < chunks; j++)
    // {
        int j = 1;
        while ((n = fread(file_binary, 1, sizeof(file_binary), fp)) > 0)
        {

            SHA1(file_binary, n, sha_of_file);

            for (int i = 0; i < 20; i++)
            { //convert to hex rep
                sprintf(encryptedText + 2 * i, "%02x", sha_of_file[i]);
            }
            cout << encryptedText << " "<<j<<"\n";
            j++;
            bzero(encryptedText, sizeof(encryptedText));
            bzero(file_binary, CHUNK_SIZE); //reset buffer
            bzero(sha_of_file, sizeof(sha_of_file));
        }
    // }
}
int main()
{
    getSHA("/home/kiranmai/IIIT/AOS/Assignment2/node1/test.pdf");

    return 0;
}