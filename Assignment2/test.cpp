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
// unordered_map<string, vector<string>>::iterator fname;
//  vector<string>::iterator fown;

void leave_group(string gid, string uid)
{
    vector <vector<string>::iterator> v;
    vector<string>::iterator it;
    vector<struct fileDetails>::iterator it2;
    for (it2 = filedetails.begin(); it2 != filedetails.end(); it2++)
    {
        if ((*it2).gid == gid)
        {
            unordered_map<string, vector<string>>::iterator fname;
            
            for (fname = (*it2).fileOwners.begin(); fname != (*it2).fileOwners.end(); ++fname)
            {
                vector<string> fown = (*fname).second;
                vector<string> v;
                for(int i=0;i<fown.size();i++)
                {
                    std::string::size_type pos = fown[i].find('$');
                    string up = fown[i].substr(0, pos);
                    if (up == uid)
                    {
                       
                    }
                    else{
                        v.push_back(fown[i]);
                    }
                }
                (*fname).second = v;
                // for (fown = (*fname).second.begin(); fown != (*fname).second.end(); ++fown)
                // {
                //     std::string::size_type pos = (*fown).find('$');
                //     string up = (*fown).substr(0, pos);
                //     if (up == uid)
                //     {
                //         (*fname).second.erase(fown);
                //     }
                // }
            }
            
        }
    }
}

int main()
{

    struct fileDetails a, b;
    a.gid = "1";
    a.fileOwners["test.cpp"].push_back("user1$100111");
    a.sha_filenames["test.cpp"].first = "sha1";
    a.sha_filenames["test.cpp"].second = "23456";

    a.fileOwners["test.cpp"].push_back("user2$100111");
    filedetails.push_back(a);

    leave_group("1", "user1");

    string gid = "1";
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