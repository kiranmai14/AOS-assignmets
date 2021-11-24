#include "header.h"

// creating inodes
struct inode in[NO_OF_INODES];

void create_filesystem()
{
    int X = 1024 * 1024 * 512;
    FILE *fp = fopen("disk", "w");
    fseek(fp, X, SEEK_SET);
    fputc('\0', fp);
    fclose(fp);
    // creating an empty disk
    vector<char> empty(BLOCK_SIZE, 0);
    ofstream ofs;
    // ("disk", ios::binary | ios::out);
    // for (int i = 0; i < NO_OF_BLOCKS; i++)
    // {
    //     cout << i << " ";
    //     if (!ofs.write(&empty[0], empty.size()))
    //     {
    //         cerr << "problem writing to file" << endl;
    //         return;
    //     }
    // }
    // ofs.close();

    // creating the super block
    struct super_block sb;

    // making inode bittmap as false
    for (int i = 0; i < NO_OF_INODES; i++)
        sb.bitmap_inode[i] = false;

    // making data bitmap as false
    for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
        sb.bitmap_data[i] = false;

    for (int i = 0; i < NO_OF_INODES; i++)
    {
        for (int j = 0; j < 52; j++)
            in[i].filename[j] = 0;

        for (int j = 0; j < 16; j++)
            in[i].pointers_to_data_blocks[j] = -1;
    }

    ofs.open("disk", ios::binary | ios::out);
    ofs.write((char *)&sb, sizeof(sb));
    ofs.seekp(sb.blocks_usedby_superblock * BLOCK_SIZE, ios::beg);
    ofs.write((char *)&in, sizeof(in));

    ofs.close();
}
void mount()
{
    struct super_block sb;
    struct inode ino;
    ifstream ifs;
    ifs.open("disk", ios::in);
    ifs.read((char *)&sb, sizeof(sb));
    cout << endl;
    cout << sizeof(sb) << endl;
    cout << "blocks_usedby_superblock " << sb.blocks_usedby_superblock << endl;
    cout << "blocks_usedby_inode " << sb.blocks_usedby_inode << endl;
    cout << "blocks_usedby_data " << sb.blocks_usedby_data << endl;
    cout << "blocks_usedby_inode_bitmap " << sb.blocks_usedby_inode_bitmap << endl;
    cout << "blocks_usedby_data_bitmap " << sb.blocks_usedby_data_bitmap << endl;
    cout << "inode_starting_index " << sb.inode_starting_index << endl;
    cout << "data_starting_index " << sb.data_starting_index << endl;
    cout << "bitmap_inode_starting_index " << sb.bitmap_inode_starting_index << endl;
    cout << "bitmap_data_starting_index " << sb.bitmap_data_starting_index << endl;
    cout << "bitmap_inode " << sb.bitmap_inode[0] << endl;
    cout << "bitmap_data " << sb.bitmap_data[0] << endl;

    ifs.seekg(sb.blocks_usedby_superblock * BLOCK_SIZE, ios::beg);
    ifs.read((char *)&ino, sizeof(ino));

    for (int j = 0; j < 52; j++)
        cout << ino.filename[j];
    cout << endl;

    for (int j = 0; j < 16; j++)
        ino.pointers_to_data_blocks[j];
    ifs.close();
}
int main()
{
    create_filesystem();
    mount();
    return 0;
}
