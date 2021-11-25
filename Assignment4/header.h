#include <bits/stdc++.h>
#include <unistd.h>
#include <math.h>
#include <fstream>
using namespace std;

#define BLOCK_SIZE 4096
#define NO_OF_BLOCKS 131072      // (512MB/4KB = 131072)
#define NO_OF_INODES 65536       //just wanted to be a multiple of 2. so we can have 65536 files in our filesystem
#define NO_OF_DATA_BLOCKS 128976 // NO_OF_BLOCKS - NO_OF_INODES - NO_OF_BLOCKS USEDBY_SUPER_BLOCK = 131072 - 2048 - 48 = 128976
#define FILE_DESCRIPTORS_COUNT 32

#define BOLD "\033[1m"
#define DEFAULT "\033[0m"
#define RED(x) "\x1B[31;1m" << x << "\033[0m"
#define GREEN(x) "\x1B[32;1m" << x << "\033[0m"
#define BROWN(x) "\x1B[33m" << x << "\033[0m"
#define BROWN_B(x) "\x1B[33;1m" << x << "\033[0m"
#define BLUE(x) "\x1B[34m" << x << "\033[0m"
#define PURPLE(x) "\x1B[35m" << x << "\033[0m"
#define YELLOW(x) "\x1B[93m" << x << "\033[0m"
#define YELLOW_B(x) "\x1B[93;1m" << x << "\033[0m"
#define WHITE(x) "\x1B[37m" << x << "\033[0m"
#define CYAN(x) "\x1B[36m" << x << "\033[0m"
#define MAGENTA(x) "\x1B[35m" << x << "\033[0m"
#define FBG(x) "\033[1;47;35m" << x << "\033[0m";

struct inode //totalsize = 12*4 + 56 + 8 = 128
{
    long long filesize;
    int inode_num;
    char filename[52];               //just wanted size to be a multiple of 2 so gave 52
    int pointers_to_data_blocks[16]; //12 direct 2 indirect 2 double indirect
    inode()
    {
        for (int j = 0; j < 16; j++)
            pointers_to_data_blocks[j] = -1;
    }
};
struct super_block //totalsize = 9*4 + 65536 + 128974 = 194548    ceil(194548/4KB) = 48 so it uses 48 blocks
{
    int blocks_usedby_superblock = ceil((float)sizeof(super_block) / (BLOCK_SIZE));
    int blocks_usedby_inode = ceil(((float)sizeof(inode) * (NO_OF_INODES)) / (BLOCK_SIZE));
    int blocks_usedby_data = NO_OF_BLOCKS - blocks_usedby_superblock - blocks_usedby_inode;
    int blocks_usedby_inode_bitmap = ceil((float)NO_OF_INODES / (BLOCK_SIZE));
    int blocks_usedby_data_bitmap = ceil((float)NO_OF_DATA_BLOCKS / (BLOCK_SIZE));
    int inode_starting_index = blocks_usedby_superblock * BLOCK_SIZE;
    int data_starting_index = inode_starting_index + blocks_usedby_inode * BLOCK_SIZE;
    int bitmap_inode_starting_index = sizeof(int) * 9;
    int bitmap_data_starting_index = bitmap_inode_starting_index + NO_OF_INODES;
    bool bitmap_inode[NO_OF_INODES];
    bool bitmap_data[NO_OF_DATA_BLOCKS];
    super_block()
    {
        // making inode bittmap as false
        for (int i = 0; i < NO_OF_INODES; i++)
            bitmap_inode[i] = false;

        // making data bitmap as false
        for (int i = 0; i < NO_OF_DATA_BLOCKS; i++)
            bitmap_data[i] = false;
    }
};
// 194548
// blocks_usedby_superblock 48
// blocks_usedby_inode 2048
// blocks_usedby_data 128976
// blocks_usedby_inode_bitmap 16
// blocks_usedby_data_bitmap 32 (31.4888)
// inode_starting_index 196608
// data_starting_index 8585216
// bitmap_inode_starting_index 36
// bitmap_data_starting_index 65572
// bitmap_inode[0] 0
// bitmap_data[0] 0
// struct super_block in;
// cout << sizeof(in) << endl;
// cout << "blocks_usedby_superblock " << in.blocks_usedby_superblock << endl;
// cout << "blocks_usedby_inode " << in.blocks_usedby_inode << endl;
// cout << "blocks_usedby_data " << in.blocks_usedby_data << endl;
// cout << "blocks_usedby_inode_bitmap " << in.blocks_usedby_inode_bitmap << endl;
// cout << "blocks_usedby_data_bitmap " << in.blocks_usedby_data_bitmap << endl;
// cout << "inode_starting_index " << in.inode_starting_index << endl;
// cout << "data_starting_index " << in.data_starting_index << endl;
// cout << "bitmap_inode_starting_index " << in.bitmap_inode_starting_index << endl;
// cout << "bitmap_data_starting_index " << in.bitmap_data_starting_index << endl;
// cout << "bitmap_inode " << in.bitmap_inode[0] << endl;
// cout << "bitmap_data " << in.bitmap_data[0] << endl;
// cout << endl;
// cout << sizeof(sb) << endl;
// cout << "blocks_usedby_superblock " << sb.blocks_usedby_superblock << endl;
// cout << "blocks_usedby_inode " << sb.blocks_usedby_inode << endl;
// cout << "blocks_usedby_data " << sb.blocks_usedby_data << endl;
// cout << "blocks_usedby_inode_bitmap " << sb.blocks_usedby_inode_bitmap << endl;
// cout << "blocks_usedby_data_bitmap " << sb.blocks_usedby_data_bitmap << endl;
// cout << "inode_starting_index " << sb.inode_starting_index << endl;
// cout << "data_starting_index " << sb.data_starting_index << endl;
// cout << "bitmap_inode_starting_index " << sb.bitmap_inode_starting_index << endl;
// cout << "bitmap_data_starting_index " << sb.bitmap_data_starting_index << endl;
// cout << "bitmap_inode " << sb.bitmap_inode[0] << endl;
// cout << "bitmap_data " << sb.bitmap_data[0] << endl;
