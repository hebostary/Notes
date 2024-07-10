#include "HttpDownloader.hpp"

HttpDownloader::HttpDownloader(int numThreads, size_t chunkSize, std::string &url, std::string &file) 
    : Downloader(numThreads, chunkSize, url, file)
{
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL); 
}

HttpDownloader::HttpDownloader(std::string &url, std::string &file) 
    : Downloader(THREAD_NUM_DEF, CHUNK_SIZE_DEF, url, file)
{
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL); 
}


HttpDownloader::~HttpDownloader()
{
    curl_global_cleanup();
}

bool HttpDownloader::prepare()
{
    // Create curl handle
    m_curl = curl_easy_init();
    if (!m_curl) {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return false;
    }

    // Get file size
    CURLcode res;
    curl_easy_setopt(m_curl, CURLOPT_URL, getUrl().c_str());
  
    /* Perform the request */
    res = curl_easy_perform(m_curl);
    if(!res) {
      /* check the size */
      double cl;
      res = curl_easy_getinfo(m_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &cl);
      if(!res) {
        printf("Size: %.0f\n", cl);
      }
    }

    // curl_easy_cleanup(m_curl);
}

bool HttpDownloader::downloadChunk(Chunk &chunk) 
{
    curl_easy_setopt(m_curl, CURLOPT_URL, getUrl().c_str());
    curl_easy_setopt(m_curl, CURLOPT_RANGE, (std::to_string(chunk.offset) + "-" + std::to_string(chunk.length)).c_str());
    ChunkWriter cw{getFilePtr(), chunk};
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &cw);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeChunk);

    CURLcode res = curl_easy_perform(m_curl);

    if(res != CURLE_OK) {
        std::cerr << "Failed to download chunk: " << curl_easy_strerror(res) << std::endl;
    }

}