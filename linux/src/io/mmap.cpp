#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "io.hpp"

int mmap_read() {
    printf("\n[INFO] running mmap read demo...\n");
    const char* filename = "./test_files/example.txt";

    // Open the file
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Get the file size
    struct stat fileInfo;
    if (fstat(fileDescriptor, &fileInfo) == -1) {
        perror("Error getting file size");
        close(fileDescriptor);
        return EXIT_FAILURE;
    }

    // Map the file into memory
    char* fileContents = static_cast<char*>(mmap(
        nullptr,
        fileInfo.st_size,
        PROT_READ,
        MAP_PRIVATE,
        fileDescriptor,
        0
    ));

    if (fileContents == MAP_FAILED) {
        perror("Error mapping file into memory");
        close(fileDescriptor);
        return EXIT_FAILURE;
    }

    // File content is now accessible through the fileContents pointer
    std::cout << "File Content:\n" << fileContents << std::endl;

    // Unmap the file from memory
    if (munmap(fileContents, fileInfo.st_size) == -1) {
        perror("Error unmapping file from memory");
    }

    // Close the file descriptor
    if (close(fileDescriptor) == -1) {
        perror("Error closing file");
    }

    return EXIT_SUCCESS;
}
