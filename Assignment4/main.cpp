#include "header.h"

int main()
{
    struct super_block in;
    cout << sizeof(in) << endl;
    cout << "blocks_usedby_superblock " << in.blocks_usedby_superblock << endl;
    cout << "blocks_usedby_inode " << in.blocks_usedby_inode << endl;
    cout << "blocks_usedby_data " << in.blocks_usedby_data << endl;
    cout << "blocks_usedby_inode_bitmap " << in.blocks_usedby_inode_bitmap << endl;
    cout << "blocks_usedby_data_bitmap " << in.blocks_usedby_data_bitmap << endl;
    cout << "inode_starting_index " << in.inode_starting_index << endl;
    cout << "data_starting_index " << in.data_starting_index << endl;
    cout << "bitmap_inode_starting_index " << in.bitmap_inode_starting_index << endl;
    cout << "bitmap_data_starting_index " << in.bitmap_data_starting_index << endl;
    cout << "bitmap_inode " << in.bitmap_inode[0] << endl;
    cout << "bitmap_data " << in.bitmap_data[0] << endl;

    return 0;
}
