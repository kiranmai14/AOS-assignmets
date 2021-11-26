#include "header.h"
#include "file_read_write.h"

void create_disk(string diskname)
{
    // creating the super block
    struct super_block sb;

    // creating an empty disk
    vector<char> empty(BLOCK_SIZE, 0);
    ofstream ofs(diskname.c_str(), ios::out | ios::binary);
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
    cout << GREEN("created disk successfully") << endl;
}
bool mount(string diskname)
{

    disk_ptr.open(diskname.c_str(), ios::in | ios::out | ios::binary);
    if (!disk_ptr.is_open())
    {
        cout << RED("unable to open the disk") << endl;
        return false;
    }
    disk_ptr.seekg(0, ios::beg);
    disk_ptr.read((char *)&superBlock, sizeof(superBlock));
    disk_ptr.seekg(superBlock.inode_starting_index, ios::beg);
    disk_ptr.read(reinterpret_cast<char *>(&in), sizeof(in));
    mounted_disk_name = diskname;

    for (int i = 0; i < NO_OF_INODES; i++)
    {
        if (superBlock.bitmap_inode[i])
        {
            file_inode_mp[in[i].filename] = i;
        }
    }
    cout << GREEN("mounted disk successfully") << endl;
    return true;
}
bool unmount(string diskname)
{
    for (int i = 0; i < FILE_DESCRIPTORS_COUNT; i++)
    {
        if (file_descriptors[i])
        {
            cout << RED("Please close opened files") << endl;
            return false;
        }
    }

    file_inode_mp.clear();
    des_ino_mode_mp.clear();
    desc_file_mp.clear();
    file_mode_desc_mp.clear();
    file_descriptors.clear();

    if (!disk_ptr.is_open())
    {
        cout << RED("disk is not opened") << endl;
        return false;
    }
    // writing the data into super block
    disk_ptr.seekp(0, ios::beg);
    disk_ptr.write((char *)&superBlock, sizeof(superBlock));

    // writing the inodedata into inode
    disk_ptr.seekp(superBlock.inode_starting_index, ios::beg);
    disk_ptr.write(reinterpret_cast<const char *>(&in), sizeof(in));
    disk_ptr.close();

    // making inode bittmap as false
    for (int i = 0; i < NO_OF_INODES; i++)
        superBlock.bitmap_inode[i] = false;

    // making data bitmap as false
    for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
        superBlock.bitmap_data[i] = false;

    memset(in, 0, sizeof(in));
    cout << GREEN("unmounted disk successfully") << endl;
    return true;
}
int get_free_data_block()
{
    for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
    {
        if (!superBlock.bitmap_data[i])
        {
            superBlock.bitmap_data[i] = true;
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
        cout << RED("Filename already exists!!") << endl;
        return;
    }
    int inode_num, data_block_num;
    if ((inode_num = get_free_inode()) == -1)
    {
        cout << RED("Free inodes are not available!!") << endl;
        return;
    }
    if ((data_block_num = get_free_data_block()) == -1)
    {
        cout << RED("Free data blocks are not available!!") << endl;
        return;
    }

    in[inode_num].filesize = 0;
    strcpy(in[inode_num].filename, filename.c_str());
    in[inode_num].pointers_to_data_blocks[0] = data_block_num;
    file_inode_mp[filename] = inode_num;
    superBlock.bitmap_data[data_block_num] = true;
    superBlock.bitmap_inode[inode_num] = true;
    cout << GREEN("created file successfully") << endl;
    for (int i = 0; i < NO_OF_INODES; i++)
    {
        if (superBlock.bitmap_inode[i])
        {
            file_inode_mp[in[i].filename] = i;
        }
    }
}
void open_file(string filename, int mode)
{
    if (file_inode_mp.find(filename) == file_inode_mp.end())
    {
        cout << RED("File is not present in the disk!!") << endl;
        return;
    }
    if (file_mode_desc_mp.find(filename) != file_mode_desc_mp.end())
    {
        if (file_mode_desc_mp[filename].first == mode)
        {
            cout << RED("File is already opened with descriptor ") << GREEN(file_mode_desc_mp[filename].second) << endl;
            return;
        }
        else
        {
            cout << RED("File is already opened with mode ") << GREEN(file_mode_desc_mp[filename].first) << endl;
            return;
        }
    }
    int file_descriptor;
    if ((file_descriptor = get_free_filedescriptor()) == -1)
    {
        cout << RED("File descriptors are not available!!") << endl;
        return;
    }
    int inode_num = file_inode_mp[filename];
    file_mode_desc_mp[filename] = make_pair(mode, file_descriptor);
    des_ino_mode_mp[file_descriptor] = make_pair(inode_num, mode);
    desc_file_mp[file_descriptor] = filename;
    cout << GREEN("opened file with descriptor ") << YELLOW(file_descriptor) << GREEN(" mode ") << mode << endl;
}
void list_files()
{
    if (file_inode_mp.size() == 0)
    {
        cout << GREEN("No files are there in the disk") << endl;
        return;
    }
    for (auto name : file_inode_mp)
    {
        cout << name.first << endl;
    }
}
void list_opened_files()
{
    if (file_mode_desc_mp.size() == 0)
    {
        cout << GREEN("No opened files") << endl;
        return;
    }
    for (auto name : file_mode_desc_mp)
    {
        cout << GREEN(name.first) << YELLOW(" mode: ") << GREEN(name.second.first)
             << YELLOW(" descriptor: ") << GREEN(name.second.second) << endl;
    }
}
void close_file(int file_descriptor)
{
    if (desc_file_mp.find(file_descriptor) == desc_file_mp.end())
    {
        cout << RED("No file is opened with that descriptor") << endl;
        return;
    }
    string filename = desc_file_mp[file_descriptor];
    desc_file_mp.erase(file_descriptor);
    file_mode_desc_mp.erase(filename);
    des_ino_mode_mp.erase(file_descriptor);
    file_descriptors[file_descriptor] = false;
    cout << GREEN("closed file successfully") << endl;
}
void show_options_for_disk()
{
    while (1)
    {
        cout << BLUE("1.create file 2.open file 3.read file 4.write file 5.append file 6.close file 7.delete file 8.list of files 9.list opened files 10.unmount") << endl;
        cout << YELLOW("Enter your choice: ");
        int choice, mode, file_desc;
        string filename, diskname;
        cin >> choice;
        switch (choice)
        {
        case 1:
            cout << YELLOW("Enter filename: ");
            cin >> filename;
            create_file(filename);
            break;
        case 2:
            cout << YELLOW("Enter filename: ");
            cin >> filename;
            cout << YELLOW("0.read\n1.write\n2.append") << endl;
            cout << YELLOW("Enter mode: ");
            cin >> mode;
            open_file(filename, mode);
            break;
        case 3:
            cout << YELLOW("Enter file descriptor: ");
            cin >> file_desc;
            read_data(file_desc);
            break;
        case 4:
            cout << YELLOW("Enter file descriptor: ");
            cin >> file_desc;
            write_data(file_desc);
            break;
        case 5:
            cout << YELLOW("Enter file descriptor: ");
            cin >> file_desc;
            append_data(file_desc);
            break;
        case 6:
            cout << YELLOW("Enter file descriptor: ");
            cin >> file_desc;
            close_file(file_desc);
            break;
        case 7:
            cout << YELLOW("Enter filename: ");
            cin >> filename;
            delete_file(filename);
            break;
        case 8:
            list_files();
            break;
        case 9:
            list_opened_files();
            break;
        case 10:
            if (unmount(mounted_disk_name))
                return;
            break;
        default:
            cout << RED("choose correct choice") << endl;
            break;
        }
    }
}
int main()
{

    while (1)
    {
        cout << BLUE("1.createdisk 2.mount 3.Quit") << endl;
        cout << YELLOW("Enter your choice: ");
        int choice;
        string diskname;
        cin >> choice;
        switch (choice)
        {
        case 1:
            cout << BLUE("Enter diskname: ");
            cin >> diskname;
            create_disk(diskname);
            break;
        case 2:
            cout << BLUE("Enter diskname: ");
            cin >> diskname;
            if (mount(diskname))
                show_options_for_disk();
            break;
        case 3:
            exit(0);
            break;
        default:
            cout << RED("choose correct choice") << endl;
            break;
        }
    }
    return 0;
}
