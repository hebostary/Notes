#include <iostream>
#include "Downloader.hpp"
#include "HttpDownloader.hpp"

/* 设计思路：
    1. 多线程下载单个网络文件，将文件切割成多个 chunk，每个线程每次执行只负责根据 offste 和 length 下载一个 chunk，并将 chunk 写入文件合适位置。
    2. 关于 chunk 的 size，最好是文件页的整数倍，以实现最好的文件IO性能，这里设计为 4MiB。
    3. 关于下载线程的数量 threadNum，支持使用者临时指定，也提供默认值。（threadNum * 4MiB）一般不会等于文件实际大小，因此将所有的 chunk 下载任务用队列组织起来。
    4. 理论上，下载线程按照 chunk 在文件里的位置顺序写入本地文件会最好，考虑以下因素：
        如果中间某个 chunk 下载任务因网络连接不稳定而变得很慢，迟迟无法将数据写入文件，那么其它下载任务会因为文件I/O被阻塞住。
        实际中，本地文件I/O的性能和稳定性一般是要好于网络I/O（尤其是SSD），所以不对 chunk 的写入顺序做强制控制，即各个下载线程在下载完 chunk 后直接写入文件。

*/

int main() {
    std::string url = "http://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm";
    std::string file = "/tmp/epel-release-latest-8.noarch.rpm";

    Downloader *downloder = new HttpDownloader(url, file);
    downloder->run();

    std::cout << "Download completed successfully" << std::endl;
    return 0;
}
