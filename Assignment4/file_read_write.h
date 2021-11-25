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
    cout << "Data size using size()" << data_size << endl;
    if (data_size < BLOCK_SIZE)
    {
        int offset = superBlock.data_starting_index + data_block_num * BLOCK_SIZE;

        char buff[BLOCK_SIZE] = {
            0,
        };
        strcpy(buff, data.c_str());

        cout << "offset " << offset << endl;

        // writing the data into disk
        disk_ptr.seekp(offset, ios::beg);
        disk_ptr.write(buff, sizeof(buff));
        cout << "disk pointer pos: " << disk_ptr.tellg() << endl;
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
    if (data_size < BLOCK_SIZE)
    {
        int offset = superBlock.data_starting_index + data_block_num * BLOCK_SIZE;
        char buff[BLOCK_SIZE] = {
            0,
        };
        disk_ptr.seekg(offset, ios::beg);
        disk_ptr.read(buff, sizeof(buff));
        data = buff;
        cout << GREEN("Data present in the file is:") << endl;
        cout << data << endl;
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
    }
    else
    {
        cout << RED("File is not opened in correct mode") << endl;
        return;
    }
}