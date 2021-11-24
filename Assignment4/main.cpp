#include "header.h"

struct super_block superBlock;
struct inode in[NO_OF_INODES];


void create_filesystem()
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
    ofstream ofs("disk", ios::out);
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
void mount()
{
    ifstream ifs;
    ifs.open("disk", ios::in);
    ifs.read((char *)&superBlock, sizeof(superBlock));
    ifs.seekg(superBlock.inode_starting_index, ios::beg);
    ifs.read(reinterpret_cast<char *>(&in), sizeof(in));
    ifs.close();
}
int main()
{
    // create_filesystem();
    mount();
    return 0;
}
