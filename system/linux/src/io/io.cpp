#include "io.hpp"

void io_entry_points()
{
    aio_write();

    mmap_read();

    epoll_server();
}