#include "Downloader.hpp"

Downloader::Downloader(int numThreads, size_t chunkSize, std::string &url, std::string &file) 
    : m_chunkSize(chunkSize), m_url(url), m_file(file), m_fileSize(0)
{
    m_pool = new ThreadPool(numThreads);
    m_pool->setDownloader(this);

    FILE *m_filePtr = fopen(file.c_str(), "wb");
    if (!m_filePtr) {
        std::cerr << "Failed to open file for writing" << std::endl;
    }
}


Downloader::~Downloader()
{
    delete m_pool;

    if (m_filePtr) {
        fclose(m_filePtr);
    }
    
}

void Downloader::submitChunks()
{
    size_t offset = 0;
    while ((offset + m_chunkSize) < m_fileSize) {
        m_pool->submitChunk(Chunk{offset, m_chunkSize});

        offset += m_chunkSize;
    }

    if ( offset < m_fileSize ) {
        m_pool->submitChunk(Chunk{offset, m_fileSize - offset});
    }
}

bool Downloader::run()
{
    if ( prepare()) {
        std::cerr << "Failed to download: get file meta data failed " << std::endl;
        return false;
    }
    
    //submitChunks();

    // 当所有的chunk都下载完成，如何通知 downloader？
}

// Write callback function for libcurl
size_t Downloader::writeChunk(void *ptr, size_t size, size_t nmemb, void *stream) {
    m_fileMtx.lock();
    ChunkWriter* cw = (ChunkWriter*)stream;
    
    // Move file pointer to a specific position
    if (fseek(cw->file, cw->chunk.offset, SEEK_SET) != 0) {
        std::cerr << "Error seeking file to offset " << cw->chunk.offset << std::endl;
        return 0;
    }

    size_t wSize = fwrite(ptr, size, nmemb, cw->file);
    m_fileMtx.unlock();

    std::cout<< "[DEBUG] write a chunk, offset:" << cw->chunk.offset << ", legth:" << wSize << std::endl;

    if ( wSize != cw->chunk.length) {
        std::cerr << "Wrote a uncompleted chunk,  offset:" << cw->chunk.offset << std::endl;
    }
    
    return wSize;
}