#include "header.h"

struct super_block superBlock;
struct inode in[NO_OF_INODES];
unordered_map<string, int> file_inode_mp;
unordered_map<int, pair<int, int>> des_ino_mode_mp;
unordered_map<string, int> file_mode_mp;
unordered_map<int, string> desc_file_mp;
vector<bool> file_descriptors(FILE_DESCRIPTORS_COUNT);

void create_disk(string diskname)
{
    // creating the super block
    struct super_block sb;

    // making inode bittmap as false
    for (int i = 0; i < NO_OF_INODES; i++)
        sb.bitmap_inode[i] = false;

    // making data bitmap as false
    for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
        sb.bitmap_data[i] = false;

    // creating an empty disk
    vector<char> empty(BLOCK_SIZE, 0);
    ofstream ofs(diskname.c_str(), ios::out);
    for (int i = 0; i < NO_OF_BLOCKS; i++)
    {
        if (!ofs.write(&empty[0], empty.size()))
        {
            cerr << "problem writing to file" << endl;
            return;
        }
    }
    ofs.seekp(0, ios::beg);
    ofs.write((char *)&sb, sizeof(sb));
    ofs.seekp(sb.inode_starting_index, ios::beg);
    ofs.write(reinterpret_cast<const char *>(&in), sizeof(in));
    ofs.close();
}
void mount(string diskname)
{
    ifstream ifs;
    ifs.open(diskname.c_str(), ios::in);
    ifs.read((char *)&superBlock, sizeof(superBlock));
    ifs.seekg(superBlock.inode_starting_index, ios::beg);
    ifs.read(reinterpret_cast<char *>(&in), sizeof(in));
    ifs.close();
}
void unmount(string diskname)
{
    //openeing the disk
    ofstream ofs(diskname.c_str(), ios::out);

    // writing the data into super block
    ofs.seekp(0, ios::beg);
    ofs.write((char *)&superBlock, sizeof(superBlock));

    // writing the inodedata into inode
    ofs.seekp(superBlock.inode_starting_index, ios::beg);
    ofs.write(reinterpret_cast<const char *>(&in), sizeof(in));
    ofs.close();
}
int get_free_data_block()
{
    for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
    {
        if (!superBlock.bitmap_inode[i])
        {
            superBlock.bitmap_inode[i] = true;
            return i;
        }
    }
    return -1;
}
int get_free_inode()
{
    for (int i = 0; i < NO_OF_INODES; i++)
    {
        if (!superBlock.bitmap_inode[i])
        {
            superBlock.bitmap_inode[i] = true;
            return i;
        }
    }
    return -1;
}
int get_free_filedescriptor()
{
    for (int i = 0; i < FILE_DESCRIPTORS_COUNT; i++)
    {
        if (!file_descriptors[i])
        {
            file_descriptors[i] = true;
            return i;
        }
    }
    return -1;
}
void create_file(string filename)
{
    if (file_inode_mp.find(filename) != file_inode_mp.end())
    {
        cout << "Filename already exists!!" << endl;
        exit(-1);
    }
    int inode_num, data_block_num;
    if ((inode_num = get_free_inode()) == -1)
    {
        cout << "Free inodes are not available!!" << endl;
        exit(-1);
    }
    if ((data_block_num = get_free_data_block()) == -1)
    {
        cout << "Free data blocks are not available!!" << endl;
        exit(-1);
    }
    
    in[inode_num].filesize = 0;
    strcpy(in[inode_num].filename, filename.c_str());
    in[inode_num].pointers_to_data_blocks[0] = data_block_num;
    file_inode_mp[filename] = inode_num;
    superBlock.bitmap_data[data_block_num] = true;
    superBlock.bitmap_inode[inode_num] = true;
}
void open_file(string filename, int mode)
{
    int file_descriptor;
    if ((file_descriptor = get_free_filedescriptor()) == -1)
    {
        cout << "File descriptors are not available!!" << endl;
        exit(-1);
    }
    int inode_num = file_inode_mp[filename];
    file_mode_mp[filename] = mode;
    des_ino_mode_mp[file_descriptor] = make_pair(inode_num, mode);
    desc_file_mp[file_descriptor] = filename;
}
void list_files()
{
    for (auto name : file_inode_mp)
    {
        cout << name.first << endl;
    }
}
void list_opened_files()
{
    for (auto name : file_mode_mp)
    {
        cout << name.first << " mode: " << name.second << endl;
    }
}
void close_file(int file_descriptor)
{
    if (desc_file_mp.find(file_descriptor) == desc_file_mp.end())
    {
        cout << "No file is opened with that descriptor" << endl;
        exit(-1);
    }
    string filename = desc_file_mp[file_descriptor];
    desc_file_mp.erase(file_descriptor);
    file_mode_mp.erase(filename);
    des_ino_mode_mp.erase(file_descriptor);
    file_descriptors[file_descriptor] = false;
}
void show_options_for_disk()
{
    while (1)
    {
        cout << "1.create file 2.open file 6.close file 8.list of files 9.list opened files 10.unmount" << endl;
        cout << "Enter your choice: ";
        int choice, mode, file_desc;
        string filename, diskname;
        cin >> choice;
        switch (choice)
        {
        case 1:
            cout << "Enter filename: ";
            cin >> filename;
            create_file(filename);
            break;
        case 2:
            cout << "Enter filename: ";
            cin >> filename;
            cout << "0 read 1 write 2 append" << endl;
            cout << "Enter mode: ";
            cin >> mode;
            open_file(filename, mode);
            break;
        case 6:
            cout << "Enter file descriptor: ";
            cin >> file_desc;
            close_file(file_desc);
            break;
        case 8:
            list_files();
            break;
        case 9:
            list_opened_files();
            break;
        case 10:
            cout << "Enter diskname: ";
            cin >> diskname;
            unmount(diskname);
            return;
            break;
        }
    }
}
int main()
{

    while (1)
    {
        cout << "1.createdisk 2.mount 3.Quit" << endl;
        cout << "Enter your choice: ";
        int choice;
        string diskname;
        cin >> choice;
        switch (choice)
        {
        case 1:
            cout << "Enter diskname: ";
            cin >> diskname;
            create_disk(diskname);
            break;
        case 2:
            cout << "Enter diskname: ";
            cin >> diskname;
            mount(diskname);
            show_options_for_disk();
            break;
        case 3:
            exit(0);
            break;
        }
    }
    return 0;
}
