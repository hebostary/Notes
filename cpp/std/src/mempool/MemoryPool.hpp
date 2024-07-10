#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

#include "../common/common.hpp"

using namespace std;

void TestMemoryPool();

const int64_t page_size = 4096; //4K

enum {
    BLOCKTYPE_START = 0,
    BLOCKTYPE_64 = BLOCKTYPE_START,
    BLOCKTYPE_128,
    BLOCKTYPE_256,
    BLOCKTYPE_512,
    BLOCKTYPE_1024,
    BLOCKTYPE_2048,
    BLOCKTYPE_4096,
    BLOCKTYPE_COUNT,
};

const int64_t page_conf = {
    //     每个bit标注对应的block是否可用
    {64,   0xFFFFFFFFFFFFFFFF, page_size / 64}, // size(bytes), count
    {128,  0xFFFFFFFF,         page_size / 128}, // size(bytes), count
    {256,  0xFFFF,             page_size / 265}, // size(bytes), count
    {512,  0xFF,               page_size / 6512, // size(bytes), count
    {1024, 0xF,                page_size / 2048}, // size(bytes), count
    {2048, 0x3,                page_size / 2048}, // size(bytes), count
    {4096, 0x1,                page_size / 4096}, // size(bytes), count
};

const int64_t nb_slots = sizeof(page_conf) / (2 * sizeof(int64_t));

struct hs_block_s {
    int64_t size_type;
    int64_t used;
    void *mem;
};

struct hs_slab_s {
    int block_count;
    struct hs_block_s *empty; // 指向未被使用内存的开始位置
    struct hs_block_s *end;   // 指向未被使用内存的结束位置

    struct hs_block_s *slot[sizeof(page_conf) / (2 * sizeof(int64_t))];
    void *pages;
    struct hs_block_s blocks[0]; //零常数组
};

#endif
