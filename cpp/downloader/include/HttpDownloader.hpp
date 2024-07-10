/*
    HTTP 文件下载器，主要负责：
        
*/
#ifndef HTTP_DOWNLOADER_H
#define HTTP_DOWNLOADER_H

#include <curl/curl.h>

#include "Chunk.hpp"
#include "Downloader.hpp"

class HttpDownloader : public Downloader
{
private:
    CURL *m_curl;

public:
    HttpDownloader(int numThreads, size_t m_chunkSize, std::string &url, std::string &file);
    HttpDownloader(std::string &url, std::string &file);
    ~HttpDownloader();

    virtual bool prepare();
    virtual bool downloadChunk(Chunk &chunk);
};


#endif HTTP_DOWNLOADER_H