#ifndef CHUNK_H
#define CHUNK_H

#include <iostream>

struct Chunk
{
    size_t offset;
    size_t length;
};

struct ChunkWriter {
  FILE  *file;
  Chunk chunk;
};

constexpr size_t CHUNK_SIZE_DEF = 4 * 1024 * 1024;

#endif