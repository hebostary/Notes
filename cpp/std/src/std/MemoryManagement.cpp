#include "MemoryManagement.hpp"
#include <fstream>
#include <sstream>

void TestMemoryManagement()
{
    EnterFunc(__FUNCTION__);
    
    TestSharedPtr();
    TestUniquePtr();
    TestDynamicArray();
    TestAllocator();

    TestTextQuery();

    //valgrind --tool=memcheck --leak-check=full ./bin/std
    //TestValgrind();

    ExitFunc(__FUNCTION__);
}

void TestSharedPtr()
{
    EnterFunc(__FUNCTION__);

    shared_ptr<string> ptr1 = make_shared<string>("AAAA");
    auto ptr2 = ptr1;
    cout << "ptr1 value: " << *ptr1 << ", ptr1 address: " << &(*ptr1) << endl;
    //ptr1 value: AAAA, ptr1 address: 0x941e30
    cout << "ptr2 value: " << *ptr2 << ", ptr2 address: " << &(*ptr2) << endl;
    //ptr2 value: AAAA, ptr2 address: 0x941e30
    cout << "reference count: " << ptr1.use_count() << endl;//2

    string *ptr3 = new string("BBBB");
    auto ptr4 = ptr3;
    cout << "ptr3 value: " << *ptr3 << ", ptr3 address: " << &(*ptr3) << endl;
    //ptr1 value: AAAA, ptr1 address: 0x941e80
    cout << "ptr4 value: " << *ptr4 << ", ptr4 address: " << &(*ptr4) << endl;
    //ptr2 value: AAAA, ptr2 address: 0x941e80
    delete ptr3;

    //可以看出make_shared和new创建的对象都在堆内存上，区别在于本函数执行结束后：
    //  ptr1和ptr2的生命周期结束，自动被销毁，调用shared_ptr的析构函数时对象引用计数减一，
    //      引用计数为0时，指向的string对象也将被销毁
    //  ptr3和ptr4的生命周期结束，自动被销毁，但是它们指向的string对象却被留在了内存里

    //如果返回智能指针，比如
    //return ptr1;
    //return语句向此函数的调用者返回一个ptr1的拷贝，这回增加所管理对象的引用计数值
    //所以即使离开了作用域，ptr1被销毁，它指向的内存还有其它使用者，所以不会被释放

    //最好对动态内存分配的对象进行初始化
    //int *pi1 = new int;//默认初始化，*pi1的值未定义
    //int *pi2 = new int();//值初始化为0，*pi2为0

    //动态分配const对象，不能修改指针指向的对象的值，但是可以让指针指向其它的对象
    //虽然const对象的值不能改变，但它本身时可以被销毁的
    const int *ptr5 = new const int(1024);//如果分配失败，new抛出std::bad_alloc
    delete ptr5;
    //*ptr5 = 50; //error
    ptr5 = new int(50); 
    //*ptr5 = 10; //error
    cout << "ptr5 value: " << *ptr5 << endl;
    delete ptr5;

    int *ptr6 = new (nothrow) int(100); //如果分配失败，new返回一个空指针
    cout << "ptr6 value: " << *ptr6 << endl;
    delete ptr6;

//不要混合使用普通指针和智能指针，否则很容易产生空悬指针和重复delete问题
    //错误使用场景分析1：
    int *x(new int(1024));
    process(shared_ptr<int>(x));
    //int j = *x; //未定义的，x是一个空悬指针

    //错误使用场景分析2：
    /*
    shared_ptr<int> p(new int(42));
    int *q = p.get();  //1. 用get返回的指针初始化一个普通指针
    {//新程序块
        //1. 这里用普通指针初始化了一个匿名智能智能对象
        //process函数退出时已经把q指向的对象释放掉了，即调用了一次delete
        process(shared_ptr<int>(q));//不注释程序必然crash
    } //程序块结束

    //2.此时p指向的对象已经被销毁，成为空悬指针，下面这个访问的行为时未定义的
    //int foo = *p;
    //cout << "foo value: " << foo << endl;//6692704
    //本函数退出时，p的生命周期结束，并且引用计数从1减为0，
    //将再次调用delete释放p指向的对象，这会导致程序直接crash
    */
    //上面两个错误场景的问题是一样的

//使用智能指针来管理不具有良好定义的析构函数的类
    /* 这是用文件流操作的demo，在成功打开文件流之后我们总是应该close掉它
    但是很可能就忘了调用close，或者程序异常退出，导致ofs没有被正常close掉
    ofstream ofs( "test_files/persons.record.1" );
    if ( ofs )
    {
        ofs << "jack 1234567 12323543543\n";
        //ofs.close(); //关闭文件并将缓存刷到文件
    }
    */
    //这种情况比如还有打开文件句柄，打开网络连接等，这些资源都不是用new显示创建的动态资源
    //而是有自定义的释放操作，使用智能指针管理时需要传递删除器（deleter）
    //1.传递删除器（函数）
    ofstream ofs1( "test_files/persons.record.1" );
    if ( ofs1 ) {
        shared_ptr<ofstream> shared_ofs1(&ofs1, closeFstream);
        ofs1 << "jack 1234567 12323543543\n";
    }
    //2.传递lambda表达式
    ofstream ofs2( "test_files/persons.record.2" );
    if ( ofs2 ) {
        shared_ptr<ofstream> shared_ofs2(&ofs2, [](ofstream *fp) -> void {
            fp->close();
            cout << "Closed ofstream via lambda." << endl;
        }
        );
        ofs2 << "jack 1234567 12323543543\n";
    }
    //在执行下面这句之前，shared_ofs1和shared_ofs2就已经被销毁，并分别调用了各自的删除器释放资源
    cout << "Life of shared_ofs1 and shared_ofs2 is end..." << endl;
    ExitFunc(__FUNCTION__);
}


//在函数被调用时ptr被创建并初始化
void process(shared_ptr<int> ptr)
{
    //process ptr
    cout << "ptr reference count: " << ptr.use_count() << endl;
}//ptr离开作用域，被销毁

void closeFstream(ofstream *fp)
{
    fp->close();
    cout << "Closed ofstream via deleter." << endl;
}

void TestUniquePtr()
{
    EnterFunc(__FUNCTION__);

//unique_ptr的初始化
    unique_ptr<string> uptr1(new string("Hello"));
    //unique_ptr<string> uptr2(uptr1); //编译错误,use of deleted function
    //unique_ptr<string> uptr2(uptr1.get()); //这也是一种非常危险的用法，
    //因为"Hello"这个字符串在uptr1和uptr2销毁时会被delete两次，导致程序crash
    unique_ptr<string> uptr3;
    //uptr3 = uptr1;//编译错误，use of deleted function
    cout << "uptr1 object value: " << *uptr1 << endl;

    uptr3.reset(uptr1.release());//uptr1释放对“Hello”字符串的管理，将其指针控制权交给uptr3
    //uptr1.release();//错误用法，将会丢失指针，无法正常释放其指向的动态内存

//unique_ptr默认也使用delete释放对象内存，因此初始化时需要传入new分配的动态内存
    //但是对于一些特殊资源，也可以和shared_ptr类似传入删除器，但是用法有些不同
    //1. 因为重载一个unique_ptr中的删除器会影响到unique_ptr类型以及如何构造（或者reset）
    //该类型的对象，所以必须在尖括号中对象类型后面提供删除器类型
    //2. 在创建或者reset一个这种unique_ptr类型的对象时，必须提供一个指定类型的可调用对象（删除器）
    ofstream ofs1( "test_files/persons.record.1" );
    if ( ofs1 ) {
        //decltype(closeFstream)返回函数类型，所以必须添加*来指出我们正在使用该类型的一个指针
        unique_ptr<ofstream, decltype(closeFstream)*> unique_ofs1(&ofs1, closeFstream);
        //使用文件输出流，即使由于异常退出，ofs1也会被正常关闭
        ofs1 << "jack 1234567 12323543543\n";
    }

    ExitFunc(__FUNCTION__);
}

//实际上，使用动态数组并不是很方便，释放数组和访问数组都容易出错
//实际开发中，建议直接使用标准库容器，比如vector
void TestDynamicArray()
{
    EnterFunc(__FUNCTION__);

    size_t num = 10;
    //这里new分配一个int数组，并返回指向第一个int的指针
    //所有元素值初始化为0，num的大小必须时整形，但不必是常量
    int * pi = new int[num]();

//零长度动态数组
    //如果num等于0，上面的调用是合法的，new返回一个合法的非空指针
    //此指针保证和其它new返回的任何指针都不相同，对于这种零长度的数组来说，此指针类似于尾后指针
    //但不能对其进行解引用，毕竟它不指向任何元素

//注意：我们这里所说的动态数组并不是数组类型
    //由于分配的内存并不是一个数组类型，因此不能对动态数组调用begin或end,
    //这些函数使用数组维度（数组类型一部分）来返回指向首元素或者尾后元素的指针
    //出于相同原因，也不能使用范围for语句来处理数组中的元素

    for (int * pq = pi; pq != pi + num; ++pq) {
        *pq += num;
        cout << *pq << " ";
    }
    cout << endl;

//释放动态数组
    //当释放一个指向数组的指针时，空方括号是必须的：它指示编译器此指针指向一个对象数组的首元素
    //数组中的元素按逆序销毁，即最后一个元素首先被销毁
    //如果在delete一个指向数组的指针时忽略了方括号，或delete一个指向单一对象的指针时使用了方括号
    //其行为时未定义的
    delete[] pi;

//默认地，unique_ptr支持管理动态数组
    //up指向一个包含10个未初始化int的数组
    unique_ptr<int []> up(new int[10]);
    //当unique_ptr指向的是一个数组而不是单个对象时，不能使用点和箭头成员运算符
    //但是可以使用下标运算符访问数组中的元素：
    for (size_t i = 0; i != 10; ++i)
    {
        up[i] = i;
    }
    
    up.release();//自动调用delete[]销毁其指针

//默认地，shared_ptr不支持管理动态数组
    //如果希望使用shared_ptr管理一个动态数组，必须提供一个删除器
    //如果没有定义删除器，上面的代码是未定义的
    shared_ptr<int> sp(new int[10], [](int *p) { delete[] p; });
    //shared_ptr未定义下标运算符，且智能指针类型不支持指针算术运算
    //因此，为了访问数组中的元素，必须用get获取内置指针，然后用它来访问数组元素
    for (size_t i = 0; i != 10; ++i)
    {
        *(sp.get() + i) = i;
    }

    ExitFunc(__FUNCTION__);
}

void TestAllocator()
{
    EnterFunc(__FUNCTION__);

//一般情况下，将内存分配和对象的构造组合在一起可能会导致不必要的浪费，比如下面这段代码
    //new表达式分配并初始化了100个string，但是实际上只用到了几个
    //即创建了大量永远也用不到的对象，并且每个使用到的元素都被赋值了两次
    size_t num = 100;
    string *const p = new string[num];
    istringstream iss("A B C D E F G");
    string w;
    string *q = p;
    while (iss >> w && q != p + num)
    {
        //只对少数的几个元素进行了赋值
        *q++ = w;
    }
    for (q = p; q != p + num; ++q)
    {
        cout << *q << " ";
    }
    cout << endl;
    delete[] p;
    
//allocator类可以将内存分配和对象构造分离开来:
//它分配的内存是原始的，未构造的
//类似于vector，allocator也是模板
    allocator<string> alloc;//可以分配string的allocator对象
    size_t n = 7;
    auto const pa = alloc.allocate(n);//分配n个未初始化或者说未构造的string

    //通过construct函数在给定的位置构造元素
    auto qa = pa;//q指向最后构造的元素之后的位置
    alloc.construct(qa++);           //*q为空字符串
    alloc.construct(qa++, 10, 'c');  //*q为cccccccccc
    alloc.construct(qa++, "hi");     //*q为hi

    auto xa = qa;
    while (xa != pa)
    {
        cout << *--xa << " "; //hi cccccccccc 
    }
    cout << endl;

    //只能对真正构造了的元素进行destroy操作
    while ( qa != pa)
    {
        alloc.destroy(--qa);
    }

    //一旦元素被销毁后，就可以使用这部分内存保存其它string，也可以将其归还给系统
    //释放内存调用deallocate完成，传递给deallocate的指针不能为空，它必须指向由allocate分配的内存
    //并且元素个数n应该保持一致
    alloc.deallocate(pa, n);
    
    ExitFunc(__FUNCTION__);
}


void TestTextQuery()
{
    EnterFunc(__FUNCTION__);

    string fileName("test_files/words.txt");
    if ( prepareText(fileName) )
    {
        TextQuery tq(fileName);
        string word;
        cout << "Enter a word to query(enter 'q' to quit):"<< endl;
        while ( cin >> word && word != "q" )
        {
            auto qr = tq.query(word);
            printQueryResult(cout, qr);
        }
    }

    ExitFunc(__FUNCTION__);
}

bool prepareText(string file)
{
    EnterFunc(__FUNCTION__);

    ofstream ofs(file);
    if ( !ofs )
    {
        cout << "[Error] Failed to prepare file: " << file << endl;
        return false;
    }

    ofs << "# Set this to 'yes' to enable PAM authentication, account processing\n";
    ofs << "# and session processing. If this is enabled, PAM authentication will\n";
    ofs << "# be allowed through the ChallengeResponseAuthentication and\n";
    ofs << "# PAM authentication via ChallengeResponseAuthentication may bypass\n";
    ofs << "# If you just want the PAM account and session checks to run without\n";
    ofs << "# and ChallengeResponseAuthentication to 'no'.\n";
    ofs << "# The default is to check both .ssh/authorized_keys and .ssh/authorized_keys2\n";
    ofs << "# Lifetime and size of ephemeral version 1 server key\n";
    ofs << "C++ is a MUST for students and working professionals to become a great Software Engineer.\n";
    ofs << "There are 1000s of good reasons to learn C++ Programming.\n";
    ofs.close();
    
    ExitFunc(__FUNCTION__);

    return true;
}

void TestValgrind()
{
    EnterFunc(__FUNCTION__);

    //CASE1，内存泄漏
    vector<string> *vs1 = new vector<string>({"hello", "world"});

    //CASE2，使用未初始化的内存
    vector<string> *vs2;
    cout << vs2->size() << endl;

    //CASE3, 在内存被释放后读写
    vector<string> *vs3 = new vector<string>({"hello", "world"});
    delete vs3;
    vs3->push_back("yeah");

    //CASE4，多次释放内存
    delete vs3;

    ExitFunc(__FUNCTION__);
}