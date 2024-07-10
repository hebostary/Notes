#include "MemoryPool.hpp"

const size_t pool_size = 20 * 1024 * 1024;

void TestMemoryPool()
{
    EnterFunc(__FUNCTION__);
    
    // create memory
    void *pool = malloc(pool_size);
    if (pool == NULL)
    {
        return;
    }

    // slab operatinos
    struct hs_slab_s * slab = hs_slab_init(pool, pool_size);

    void *p = hs_slab_malloc(slab, 100);
    hs_slab_free(slab, p);

    // destory memory
    free(pool);

    ExitFunc(__FUNCTION__);
}

// pool, memory addr
// size, memory size
/* 
    每个 block 就是一个固定大小（page_size）的内存块，由一个单独的 hs_block_s 结构体来描述，
    所有的 hs_block_s 是连续存储的，依次紧跟在 hs_block_s 后面

| hs_slab_s | hs_block_s | hs_block_s | ... | hs_block_s | block (page_size) | block (page_size)  |...|
            ^                                            ^                                            ^
            ^                                            s->pages     
            s->empty                                     s->end                                

*/
struct hs_slab_s* hs_slab_init(void *pool, int64_t size) {
    // pool的前面一段空间就用来保存 hs_slab_s 这个结构体
    struct hs_slab_s *s = reinterpret_cast<struct hs_slab_s *>(pool); 
    s->empty = s->blocks;
    // 计算整片内存里的可用 block数量，先减去头部 hs_slab_s 占用的空间
    s->block_count = (size - sizeof(struct hs_slab_s) / (page_size + sizeof(struct hs_block_s)));
    // pages 指向用于存储数据的内存页
    s->pages = pool + sizeof(struct hs_slab_s) + sizeof(struct hs_block_s) * s->block_count;
    s->end = s->blocks + s->block_count;

    int i = 0;
    for (size_t i = 0; i < s->block_count; i++)
    {
        s->blocks[i].size_type = 64 * 2 ^ i;
        s->blocks[i].mem = s->pages + i * page_size;
    }

    for (size_t i = 0; i < nb_slots; i++)
    {
        s->slot[i] = s->end;
    }
    
    return s;
}

// malloc
struct hs_block_s *hs_slab_add_block(struct hs_slab_s* s, int idx) {

}

void *hs_slab_malloc(struct hs_slab_s* s, size_t size) {
    // 最大只能分配 4k 的 block
    if (s == NULL || size > page_size) return NULL;

    int sizetype = BLOCKTYPE_START;
    while ( size > page_conf[sizetype][0]) sizetype++;
    
    struct hs_block_s *b = s->slot[sizetype];
    if ( b == s->end )
    {
        b = hs_slab_add_block(s, sizetype);
    }
    
    int64_t i = 0, n = page_conf[sizetype][1]; //2556, 16
    while(i < n && (b->used & (0x1 << i))) i++;
    b->used |= (0x1 << i);

    void *ptr = b->mem + i * page_conf[sizetype][0];

    return ptr;
}

//free
void *hs_slab_free(struct hs_slab_s* s, void *ptr) {
    // search
    int64_t page = (ptr - s->pages) / page_size;
    struct hs_block_s *b = s->blocks[page];
    int64_t idx = (ptr - s->pages) % (page_size / page_conf[b->size_type][0]);

    // bit
    b->used ^= (0x1 << idx);

    // delete
    
}