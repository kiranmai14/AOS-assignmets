#include <openssl/sha.h>
#include <iostream>
#include <bits/stdc++.h>
#include "headers.h"

using namespace std;
struct fileDetails
{
    string gid;
    unordered_map<string, vector<string>> fileOwners;          //[filename]->->"uid$01010101" (vector)
    unordered_map<string, pair<string, string>> sha_filenames; //[filename]->sha
    // unordered_map<string, vector<string>> filenames_paths;
};
vector<struct fileDetails> filedetails;

void upload_file(string gid, string ip, int port, string filename, string shaval, string len, string filepath)
{
    string userId = "A";
    // fileHash[filename] = hashval;
    string chunkmap = "";
    string bitmap = "123456";
    chunkmap = userId + "$" + bitmap;
    bool flag = 0;

    vector<struct fileDetails> :: iterator it;
    for(it = filedetails.begin();it!=filedetails.end();it++)
    {
        if((*it).gid == gid)
        {
             flag = 1;
            (*it).fileOwners[filename].push_back(chunkmap);
            (*it).sha_filenames[filename].first = shaval;
            (*it).sha_filenames[filename].second = len;
        }
    }

    // for (struct fileDetails p : filedetails)
    // {
    //     if (p.gid == gid)
    //     {
    //         flag = 1;
    //         unordered_map<string, vector<string>> temp = p.fileOwners;
    //         temp[filename].push_back(chunkmap);
    //         p.fileOwners = temp;
    //         // p.fileOwners[filename].push_back(chunkmap);
    //         p.sha_filenames[filename].first = shaval;
    //         p.sha_filenames[filename].second = len;
    //     }
    // }

    if (!flag)
    {
        struct fileDetails filed;
        filed.gid = gid;

        unordered_map<string, vector<string>> unmap;
        vector<string> v;
        v.push_back(chunkmap);
        unmap[filename] = v;
        filed.fileOwners = unmap;

        unordered_map<string, pair<string, string>> shamap;
        pair<string, string> p;
        p.first = shaval;
        p.second = len;
        shamap[filename] = p;
        filed.sha_filenames = shamap;
        filedetails.push_back(filed);
    }
}

int main()
{

    upload_file("1", "127.0.0.1", 4000, "test.pdf", "sh", "23455", "/home/kiran");
    upload_file("1", "127.0.0.1", 4000, "test.pdf", "sh", "23455", "/home/kiran");
    upload_file("1", "127.0.0.1", 4000, "test2.pdf", "sh", "23455", "/home/kiran");
    string gid ="1";
    for (auto p : filedetails)
    {
        if (gid == p.gid)
        {
            for (auto g : p.fileOwners)
            {
                for (auto st : g.second)
                {
                    cout << st << endl;
                }
            }
        }
    }
    return 0;
}