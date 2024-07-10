/*
    下载器基类，抽象公共的方法
*/

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <iostream>
#include <fstream>
#include <mutex>
#include "ThreadPool.hpp"
#include "Chunk.hpp"

const int THREAD_NUM_DEF        = 4;

class Downloader
{
private:
    ThreadPool* m_pool;

    std::string m_url;
    std::string m_file;
    FILE *m_filePtr;

    size_t m_fileSize;
    size_t m_chunkSize;
    size_t m_chunkNum;

    static std::mutex m_fileMtx;

public:
    Downloader(int numThreads, size_t chunkSize, std::string &url, std::string &file);
    ~Downloader();

    void submitChunks();

    // 打开写入文件句柄，并开始下载
    bool run();

    std::string getUrl() const { return m_url; }
    FILE* getFilePtr() const { return m_filePtr; }
    void setFileSize(size_t size) { m_fileSize = size; }

    // 纯虚函数，每个下载器实现都必须实现这些接口
    // 获取下载文件的元数据，主要是 size 和 校验信息
    virtual bool prepare() = 0;

    // 执行下载并写入单个 chunk 的函数
    virtual bool downloadChunk(Chunk &chunk) = 0;

    static size_t writeChunk(void *ptr, size_t size, size_t nmemb, void *stream);
};


#endif DOWNLOADER_H