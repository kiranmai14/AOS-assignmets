#include <bits/stdc++.h>
#include <unistd.h>
#include <math.h>
using namespace std;

#define BLOCK_SIZE 4096
#define NO_OF_BLOCKS 131072 // (512MB/4KB = 131072)
#define NO_OF_INODES 65536  //just wanted to be a multiple of 2. so we can have 65536 files in our filesystem
#define NO_OF_DATA_BLOCKS 128976   // NO_OF_BLOCKS - NO_OF_INODES - NO_OF_BLOCKS USEDBY_SUPER_BLOCK = 131072 - 2048 - 48 = 128976


struct inode //totalsize = 12*4 + 56 + 8 = 128
{
    int pointers_to_data_blocks[16]; //12 direct 2 indirect 2 double indirect
    char filename[56]; //just wanted size to be a multiple of 2 so gave 56
    long long filesize;
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
