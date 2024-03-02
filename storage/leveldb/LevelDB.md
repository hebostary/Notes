# Introduction

# Architecture

# Demonstration

## How to get libleveldb.a

Refer to [Building](https://github.com/google/leveldb) to build leveldb project on a Linux system then we can get following static library in build directory:

```
[root@net146-host122 leveldb]# mkdir -p build && cd build
[root@net146-host122 build]# cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
[root@net146-host122 leveldb]# ls build/ | grep liblevel
libleveldb.a
```

After copied whole [include](https://github.com/google/leveldb/tree/main/include/leveldb) ditrectory to our own project, then we can write our application code based on the leveldb library.
