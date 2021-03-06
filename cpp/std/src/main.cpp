#include "std/std.hpp"
#include "std/Array.hpp"
#include "std/String.hpp"
#include "std/DerivedTask.hpp"
#include "std/Template.hpp"
#include "std/Containers.hpp"
#include "std/Algorithm.hpp"
#include "std/IOLibrary.hpp"
#include "std/MemoryManagement.hpp"
#include "std/StrVec.hpp"

int main() {
    cout<<"[BEGIN] testing..."<<endl;
    //TestArray();

    //测试自定义的string类
    //TestStringClass();

    //TestDerivedTask();

    //TestTemplate();

    //测试标准库容器
    //TestContainers();

    //测试标准库算法
    //TestAlgorithms();

    //测试IO库
    //TestIOLibrary();

    //测试内存管理
    //TestMemoryManagement();

    //测试简化版的vector实现
    TestStrVec();

    cout<<"[END] testing..."<<endl;
}
