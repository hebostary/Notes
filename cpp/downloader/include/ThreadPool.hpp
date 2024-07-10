/*
    下载器的线程池 worker，线程负责从全局队列里拿取下载 chunk 的任务，下载对应的 chunk 后根据偏移将数据写入文件正确的位置
*/

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Chunk.hpp"
#include "Downloader.hpp"

class ThreadPool {
public:
    ThreadPool(size_t numThreads) : m_stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            m_threads.emplace_back([this, i] {
                threadFunc(i);
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_stop = true;
        }
        m_condition.notify_all();

        for (auto& thread : m_threads) {
            thread.join();
        }
    }

    void submitChunk(Chunk chunk) {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_chunkQueue.push(chunk);

        m_condition.notify_one();
    }

    void setDownloader( Downloader * downloader ) {
        m_downloader = downloader;
    }

private:
    void threadFunc(int id) {
        std::vector<char> buffer;
        while (true) {
            Chunk chunk;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_condition.wait(lock, [this] { return m_stop || !m_chunkQueue.empty(); });
                if (m_stop && m_chunkQueue.empty()) {
                    std::cout << "[DEBUG] thread " << id << " exiting" << std::endl;
                    return;
                }
                chunk = m_chunkQueue.front();
                m_chunkQueue.pop();
            }
            buffer.clear(); // Clear buffer before executing task
            m_downloader->downloadChunk(chunk);
        }
    }

    Downloader* m_downloader;
    std::vector<std::thread> m_threads;
    std::queue<Chunk> m_chunkQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    bool m_stop;
};