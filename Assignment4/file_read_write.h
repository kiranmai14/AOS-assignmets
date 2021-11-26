void die(const char *s)
{
    perror(s);
    exit(1);
}
void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        die("tcsetattr");
    }
}
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
string get_input()
{
    enableRawMode();
    string data;
    cout << "Enter data: " << endl;
    while (true)
    {
        fflush(0);
        char inp[3];
        memset(inp, 0, 3 * sizeof(inp[0]));
        if (read(STDIN_FILENO, &inp, 3) == 0)
        {
            continue;
        }
        if ((inp[2] == 66 || inp[2] == 65 || inp[2] == 67 || inp[2] == 68) && inp[0] == 27 && inp[1] == 91 || inp[0] == 127)
        {
            continue;
        }
        else if (inp[0] == 10) //enter key
        {
            disableRawMode();
            return data;
            break;
        }
        else
        {
            cout << inp[0];
            data.push_back(inp[0]);
        }
    }
    disableRawMode();
    return data;
}

void write_mode(int inode_num)
{
    string data = get_input();
    int data_block_num = in[inode_num].pointers_to_data_blocks[0];
    long long data_size = data.size();
    in[inode_num].filesize = data_size;
    cout << endl
         << "Data size using size()" << data_size << endl;
    int no_of_blocks = data_size / BLOCK_SIZE;
    if (no_of_blocks < 12)
    {
        int string_offset = 0;
        for (int i = 0; i <= no_of_blocks; i++)
        {
            cout << "Block number " << data_block_num << endl;
            // updating inode
            in[inode_num].pointers_to_data_blocks[i] = data_block_num;

            // updating superblock bitmap
            superBlock.bitmap_data[data_block_num] = true;

            // getting offset for next free block
            int offset = superBlock.data_starting_index + data_block_num * BLOCK_SIZE;
            char buff[BLOCK_SIZE] = {
                0,
            };

            // need to get the data of required size
            long long len_of_piece = min((long long)BLOCK_SIZE, data_size);
            string piece_data = data.substr(string_offset, len_of_piece);

            memcpy(buff, piece_data.c_str(), len_of_piece);

            cout << "offset " << offset << endl;

            // writing the data into disk
            disk_ptr.seekp(offset, ios::beg);
            disk_ptr.write(buff, sizeof(buff));
            cout << "disk pointer pos: " << disk_ptr.tellg() << endl;

            // getting next free block
            data_block_num = get_free_data_block();
            data_size = data_size - len_of_piece;
            string_offset = string_offset + len_of_piece;
        }
        // because the for loop exists but it becomes true
        superBlock.bitmap_data[data_block_num] = false;
    }
    cout << GREEN("Data written to file successfully") << endl;
}
void append_mode(int inode_num)
{
    string data = get_input();
    long long data_size = data.size();
    int data_block_num;
    long long file_size = in[inode_num].filesize;
    if (file_size < 12 * BLOCK_SIZE)
    {
        for (int i = 0; i < 12; i++)
        {
            if (in[inode_num].pointers_to_data_blocks[i] == -1)
            {
                data_block_num = i - 1;
                break;
            }
        }
        int offset = superBlock.data_starting_index + data_block_num * BLOCK_SIZE;
        offset = offset + (file_size % BLOCK_SIZE);
        int buffer_size = BLOCK_SIZE - (file_size % BLOCK_SIZE);
        if (data_size < buffer_size)
        {
            char buff[buffer_size] = {
                0,
            };
            strcpy(buff, data.c_str());
            cout << endl
                 << "Buffer size " << buffer_size << endl;
            cout << "offset " << offset << endl;

            // writing the data into disk
            disk_ptr.seekp(offset, ios::beg);
            disk_ptr.write(buff, sizeof(buff));
            cout << "disk pointer pos: " << disk_ptr.tellg() << endl;
        }
    }
    in[inode_num].filesize = in[inode_num].filesize + data_size;
    cout << GREEN("Data appended to file successfully") << endl;
}
void append_data(int file_descriptor)
{
    if (des_ino_mode_mp.find(file_descriptor) == des_ino_mode_mp.end())
    {
        cout << RED("File descriptor is not valid") << endl;
        return;
    }
    if (des_ino_mode_mp[file_descriptor].second == 2)
    {
        append_mode(des_ino_mode_mp[file_descriptor].first);
    }
    else
    {
        cout << RED("File is not opened in correct mode") << endl;
        return;
    }
}
void write_data(int file_descriptor)
{

    if (des_ino_mode_mp.find(file_descriptor) == des_ino_mode_mp.end())
    {
        cout << RED("File descriptor is not valid") << endl;
        return;
    }
    if (des_ino_mode_mp[file_descriptor].second == 1)
    {
        write_mode(des_ino_mode_mp[file_descriptor].first);
    }
    else
    {
        cout << RED("File is not opened in correct mode") << endl;
        return;
    }
}
void read_mode(int inode_num)
{
    long long data_size = in[inode_num].filesize;
    cout << "Data size " << data_size << endl;
    int data_block_num = in[inode_num].pointers_to_data_blocks[0];
    string data;
    int no_of_blocks = data_size / BLOCK_SIZE;
    cout << GREEN("Data present in the file is:") << endl;
    if (no_of_blocks < 12)
    {
        int string_offset = 0;
        for (int i = 0; i <= no_of_blocks; i++)
        {
            data_block_num = in[inode_num].pointers_to_data_blocks[i];
            // cout << "Block number " << data_block_num << endl;

            // getting offset
            int offset = superBlock.data_starting_index + data_block_num * BLOCK_SIZE;
            char buff[BLOCK_SIZE] = {
                0,
            };
            long long len_of_piece = min((long long)BLOCK_SIZE, data_size);

            // cout << "offset " << offset << endl;

            // reading the data from disk
            disk_ptr.seekg(offset, ios::beg);
            disk_ptr.read(buff, sizeof(buff));

            for (int i = 0; i < len_of_piece; i++)
                cout << buff[i];
            data_size = data_size - len_of_piece;
        }
    }
}
void read_data(int file_descriptor)
{
    if (des_ino_mode_mp.find(file_descriptor) == des_ino_mode_mp.end())
    {
        cout << RED("File descriptor is not valid") << endl;
        return;
    }
    if (des_ino_mode_mp[file_descriptor].second == 0)
    {
        read_mode(des_ino_mode_mp[file_descriptor].first);
        cout << endl;
    }
    else
    {
        cout << RED("File is not opened in correct mode") << endl;
        return;
    }
}
void delete_file(string filename)
{
    if (file_inode_mp.find(filename) == file_inode_mp.end())
    {
        cout << RED("File is not present in the disk") << endl;
        return;
    }
    if (file_mode_desc_mp.find(filename) != file_mode_desc_mp.end())
    {
        cout << RED("cannot delete an opened file") << endl;
        return;
    }

    int inode_num = file_inode_mp[filename];
    long long file_size = in[inode_num].filesize;
    int no_of_blocks = file_size / BLOCK_SIZE;
    if (no_of_blocks < 12)
    {
        for (int i = 0; i <= no_of_blocks; i++)
        {
            if (in[inode_num].pointers_to_data_blocks[i] > -1)
            {
                int data_block_num = in[inode_num].pointers_to_data_blocks[i];
                int offset = superBlock.data_starting_index + data_block_num * BLOCK_SIZE;
                char buff[BLOCK_SIZE] = {
                    0,
                };
                disk_ptr.seekp(offset, ios::beg);
                disk_ptr.write(buff, sizeof(buff));
                superBlock.bitmap_data[data_block_num] = false;
            }
        }
    }

    // resetting inode
    for (int j = 0; j < 16; j++)
        in[inode_num].pointers_to_data_blocks[j] = -1;
    in[inode_num].filesize = 0;
    memset(in[inode_num].filename, 0, sizeof(in[inode_num].filename));

    // making super block clear
    superBlock.bitmap_inode[inode_num] = false;

    // erasing from the map
    file_inode_mp.erase(filename);

    cout << GREEN("Deleted file successfully") << endl;
}