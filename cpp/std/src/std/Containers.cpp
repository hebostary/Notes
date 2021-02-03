#include "Containers.hpp"

void TestContainers() 
{
    EnterFunc(__FUNCTION__);

    //测试顺序容器
    TestInit();
    TestSwap();
    TestCompare();
    TestArray();
    TestVector();
    TestList();
    TestForwardList();
    TestString();

    //测试关联容器
    TestMapAndSet();
    TestWordTransform();
    TestUnorderedMapAndSet();
    
    ExitFunc(__FUNCTION__);
}

void TestInit()
{
    EnterFunc(__FUNCTION__);

    // 1. 列表初始化
    //对于除array之外的容器类型，初始化列表还隐含指定了容器大小
    //容器将包含与初始值一样多的元素
    list<string> authors = {"hunk", "jack"};
    vector<const char*> articles = {"an", "ad"};
    //容器嵌套时仍然可以使用列表初始化
    list<deque<int>> ld = { {1, 2}, {4, 5, 7}};

    // 2. 容器拷贝和赋值
    //创建一个容器为另一个容器的拷贝，两个容器的类型及其元素类型必须匹配
    list<string> list2(authors);
    //deque<string> authorsd(authors);//ERROR:没有与参数列表匹配的构造函数
    //deque<string> authorsd = authors; //ERROR:容器类型不匹配
    //vector<string> words(articles);//ERROR:没有与参数列表匹配的构造函数

    // 2.1 不同类型容器之间的转换
    //如果传递迭代器参数来拷贝一个范围时，就不要求容器类型时相同的了
    //新容器和原容器中的元素类型也可以不同，只要能将要拷贝的元素转换为
    //要初始化的容器的元素类型即可，比如这里const char * -> string
    forward_list<string> words(articles.begin(), articles.end());
    cout << "words: ["; for (auto &&w : words) cout << w << ", "; cout << "]\n";

    // 2.2 assign操作也可以实现不同类型容器之间的转换
    // assign操作只可以用于顺序容器（array除外），它允许从一个不同但相同的类型赋值，
    //或者从容器的子序列赋值
    forward_list<string> words1;
    // assign v1：将word1中的元素替换为迭代器指定的范围中的元素的拷贝
    words1.assign(articles.cbegin(), articles.cend());
    cout << "words1: ["; for (auto &&w : words1) cout << w << ", "; cout << "]\n";
    // assign v2：用指定数目且具有相同给定值的元素替换容器中的元素
    list<string> slist1(1); //1个元素，为空元素
    slist1.assign(3, "slist1.name");
    cout << "slist1: ["; for (auto &&w : slist1) cout << w << ", "; cout << "]\n";


    // 3. 初始化指定大小的容器
    //如果元素是内置类型或者是具有默认构造函数的类类型，可以只提供容器大小参数
    //如果元素类型没有默认构造函数，还必须指定一个显式的元素初始值
    vector<int> ivec(10, -1); //10个-1
    list<string> svec(10, "hi"); //10个hi
    forward_list<int> fvec(10); //10个0，初始化为默认值

    //std::array具有固定大小
    //元素类型和大小都是数组类型的一部分
    array<int, 10> a1 = {1, 2, 3, 4, 5}; //剩下的初始化为0
    a1 = {1, 2, 3, 4, 5, 6};
    //为了使用array类型，我们必须同时指定元素类型和大小
    array<int, 10>::size_type i;

    //类型匹配时，std::array允许对象赋值或者拷贝
    array<int, 10> a2 = a1;
    //内置数组就不支持拷贝或赋值
    //int digs[6] = {0, 1, 2, 3, 4, 5};
    //int cpy[6] = digs;

    ExitFunc(__FUNCTION__);
}

void TestSwap()
{
    EnterFunc(__FUNCTION__);
    // 统一使用非成员版本的swap是一个好习惯

    // swap操作交换两个容器的内容
    // 除array外，交换两个容器内容的操作保证会很快，因为元素本身并未交换，
    // swap只是交换了两个容器的内部数据结构
    // 除array外，swap不对任何元素进行拷贝/删除或插入操作，因此可以保证在常数时间内完成
    // swap两个array会真正交换它们的元素，因此所需时间与元素数目成正比
    list<string> slist1(3, "slist1.name");
    list<string> slist2(5, "slist2.name");
    swap(slist1, slist2);
    cout << "slist1: ["; for (auto &&w : slist1) cout << w << ", "; cout << "]\n";
    cout << "slist2: ["; for (auto &&w : slist2) cout << w << ", "; cout << "]\n";

    //除string外，指向容器的迭代器/应用和指针在swap操作之后都不会失效，
    // 它们仍然指向swap操作之前所指向的那些元素，但是swap之后这些元素已经属于不同容器
    ExitFunc(__FUNCTION__);
}

void TestCompare()
{
    EnterFunc(__FUNCTION__);
    //每个容器类型都支持相等运算符（==和!=）
    //除了无序关联容器外的所有容器都支持关系运算符（ >, >=, <, <= ）
    //关系运算符左右两边的运算对象必须是相同类型的容器，且必须保存相同类型的元素
    //1. 比较两个容器实际上是进行元素的逐次比较
    //2. 容器的相等运算符实际上是使用元素的==运算符实现比较的
    //   而其他关系运算符是使用元素的 < 运算符
    //   所以容器可以比较的前提是容器元素支持所需的运算符
    list<int> ilist1 = {1, 2, 6};
    list<int> ilist2 = {1, 2, 4};
    list<int> ilist3 = {1, 2, 6, 9};
    cout << "ilist1 > ilist2: " << (ilist1 > ilist2) << endl;
    cout << "ilist1 > ilist3: " << (ilist1 > ilist3) << endl;

    ExitFunc(__FUNCTION__);
}

void TestArray()
{
    EnterFunc(__FUNCTION__);

    array<int, 3> a1 = { 3, 5, 1 };
    cout << "max size: "<< a1.max_size()<< endl;
    cout << "first element(index 0): " << a1[0] << endl;
    cout << "first element: " << a1.front() << endl;
    cout << "last element: " << a1.back() << endl;

    int* pA1 = a1.data();
    cout << "length of pA1: " << sizeof(pA1) << endl; //capacity: 8

    //at返回位于指定位置 pos 的元素的引用，有边界检查。
    //若 pos 不在容器范围内，则抛出 std::out_of_range 类型的异常。
    a1.at(2) = 100; //根据索引访问元素
    try
    {
        a1.at(3) = 200;
    }
    catch(const std::out_of_range& e)
    {
        cout << e.what() << '\n'; //array::at: __n (which is 3) >= _Nm (which is 3)
    }
    

    for (const auto& e : a1) cout<< e << " "; //3 5 100
    cout << "\n";
    
    std::sort(a1.begin(), a1.end(), std::greater<int>());//降序
    for (const auto& e : a1) cout<< e << " "; //100 5 3
    cout << "\n";

    ExitFunc(__FUNCTION__);
}

void TestVector()
{
    EnterFunc(__FUNCTION__);
    vector<int> vi;
    cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//0, 0
    vi.push_back(1);
    cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//1, 1
    vi.push_back(3); vi.push_back(5); vi.push_back(7);
    cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//4, 4
    vi.push_back(8);
    cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//5, 8
    vi.reserve(12);
    cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//5, 12
    vi.shrink_to_fit();
    cout<<"size: "<<vi.size()<<", capacity: " << vi.capacity() << " \n";//5, 5
    ExitFunc(__FUNCTION__);
}

void TestList()
{
    EnterFunc(__FUNCTION__);
    list<deque<int>> ld = { {1, 2}, {4, 5, 7}};
    deque<int> d1 = {7, 5, 16, 8};
    deque<int> d2 = {7, 5, 16, 88};
    ld.insert(ld.begin(), d1);//在链表头部插入元素
    ld.push_back(d2); //在链表尾部插入元素
    cout << "list size: " << ld.size() << endl;
    for (const auto &d : ld)
    {
        cout << "current deque size: " << d.size() << endl;
        for (const auto &e : d)
        {
            cout << e << " ";
        } 
        cout << '\n';        
    }

    ExitFunc(__FUNCTION__);
}

void TestForwardList()
{
    EnterFunc(__FUNCTION__);
    forward_list<int> flist = {1, 3, 6, 7, 12, 34, 37};
    auto prev = flist.before_begin(); //指向首前元素
    auto curr = flist.begin();
    cout << "flist: ["; for (auto &&w : flist) cout << w << ", "; cout << "]\n";
    while (curr != flist.end())
    {
        //删除奇数元素
        if (*curr % 2) {
            curr = flist.erase_after(prev);
        } else {
            prev = curr;
            ++curr;
        }
    }
    cout << "flist: ["; for (auto &&w : flist) cout << w << ", "; cout << "]\n";
    
    ExitFunc(__FUNCTION__);
}

void TestString()
{
    EnterFunc(__FUNCTION__);

    /*
    字符串的初始化
    */
    const char* cstr = "Hello World";//C style字符串，以'\0'结束
    char noNull[] = {'H', 'i'}; //字符数组，不是以'\0'结束

    //拷贝cstr中的字符直到遇到空字符
    string s1(cstr);
    cout<<"s1: "<< s1 <<endl;
   
    string s2(noNull, 2);//只拷贝两个字符
    cout<<"s2: "<< s2 <<endl;
    
    string s3(noNull);//未定义：noNull不是以空字符结束
    cout<<"s3: "<< s2 <<endl;//Hi

    string s4(s1, 6);
    cout<<"s4: "<< s4 <<endl;//World

    reverse(s4.begin(), s4.end());
    cout<<"s4: "<< s4 <<endl;//dlroW
    
    /*
    截取字串操作
    */
    string subs1 = s1.substr(0, 5);
    string subs2 = s1.substr(6);
    cout<<"subs1: "<< subs1 <<endl;//Hello
    cout<<"subs2: "<< subs2 <<endl;//World


    /*
    修改string的一些操作
    */
    string s11 = "Hello World";
    s11.insert(s11.size(), 5, '!'); //在字符串尾部插入5个感叹号
    cout<<"s11: "<< s11 <<endl;//Hello World!!!!!
    s11.erase(s11.size()-5, 5);//删除尾部5个字符
    cout<<"s11: "<< s11 <<endl;//Hello World

    //string还提供了接受C风格字符串的insert版本和assign版本
    const char* cp = "Stanly, plump Buck";
    string s12 = "Hello World";
    s12.assign(cp, 7);//从cp里拷贝7个字符来替换s12的内容
    cout<<"s12: "<< s12 <<endl;//Stanly,
    s12.insert(s12.size(), cp + 7);
    cout<<"s12: "<< s12 <<endl;//Stanly, plump Buck
    ExitFunc(__FUNCTION__);

    //apend操作是在字符串尾部进行插入操作的一种的简写形式
    string s13("C++ Primer"), s14 = s13;
    s13.insert(s13.size(), " 5th Ed.");
    s14.append(" 5th Ed.");//这两种追加字符串的效果是一样的
    cout<<"s13: "<< s13 <<endl;
    cout<<"s14: "<< s14 <<endl;

    //replace操作是调用erase和insert的一种简写形式
    s13.erase(11, 3);
    cout<<"s13: "<< s13 <<endl;//C++ Primer  Ed.
    s13.insert(11, "6th");
    cout<<"s13: "<< s13 <<endl;//C++ Primer 6th Ed.
    //等价的replace操作
    s14.replace(11, 3, "Fifth");//允许替换不同长度的子串
    cout<<"s14: "<< s14 <<endl;//C++ Primer Fifth Ed.

    /*
    string查找操作
        1.查找操作是大小写敏感的
    */
    string s21 = "Hello World";
    auto pos1 = s21.find("Hunk");
    auto pos2 = s21.find("World");
    cout << "Find Hunk position: " << int(pos1) << endl; //-1
    cout << "Find World position: " << pos2 << endl; //6

    //查找与给定字符串中任何一个字符串匹配的位置
    string numbers("0123456789"), name("aqzwsxr2d2");
    auto pos3 = name.find_first_of(numbers);
    auto pos4 = name.find_first_not_of(name.substr(0, 7));
    cout << "pos3: " << pos3 << endl;//7
    cout << "pos4: " << pos4 << endl;//7

    /*
    数值转换
        1.如果string不能转换为一个数值，这些函数抛出一个invalid_argument异常
        2.如果转换得到的额数值无法用任何类型来表示，则抛出一个out_of_range异常
    */   
    int i = 12345;
    //将整形转换为字符串
    string s31 = to_string(i);
    cout << "s31: " << s31 << endl;// 12345
    //将字符串转为浮点值
    double d = stod(s31);
    cout << "(d + 0.2) = " << d + 0.2 << endl;// 12345.2
    string s32 = "pi = 3.14";
    d = stod(s32.substr(s32.find_first_of("+-0123456789")));
    cout << "d = " << d << endl;//3.14

    ExitFunc(__FUNCTION__);
}

void TestMapAndSet()
{
    EnterFunc(__FUNCTION__);

    string word, words = "Hunk always have his own idea! He need more chance to get success";
    istringstream iss(words);
    
    map<string, size_t> word_count;
    set<string> black_list = {"have", "his", "He"};
    while ( iss >> word)
    {
        if ( black_list.find(word) == black_list.end() )
        {   //确认单词不在黑名单后才对它进行统计
            ++word_count[word];
        }
    }

    for (const auto &w : word_count)
    {
        cout << w.first << " occurs " << w.second
            << ((w.second > 1) ? " times." : " time.") << endl;
    }

    //topic: 迭代器遍历关联容器
    auto map_it = word_count.cbegin();
    while ( map_it != word_count.cend() )
    {
        cout << map_it->first << " occurs " << map_it->second
            << (( map_it->second > 1) ? " times." : " time.") << endl;
        ++map_it;
    }

    //topic: 不同的方式向map中插入元素--pair类型
    //对于map的insert方法，返回类型也是pair：
    //          pair的first成员是一个迭代器，指向具有给定关键字的元素（注意：也是一个pair）
    //          pair的second成员是一个bool值，指出元素是插入成功还是已经存在
    word = "good";
    cout << word_count.insert( { word, 1 } ).second << endl;//1
    cout << word_count.insert( make_pair(word, 1) ).second << endl;//0
    cout << word_count.insert( pair<string, size_t>(word, 1) ).second << endl;//0
    cout << word_count.insert( map<string, size_t>::value_type(word, 1) ).second << endl;//0

    if ( black_list.find(word) == black_list.end() )
    {
        cout << "the " << word << " not int black list" << endl;
    }
    
    
    multiset<string> mss= {"good", "new", "yes"};
    vector<string> vs;
    
    //将顺序容器中的元素拷贝到关联容器中
    copy(mss.begin(), mss.end(), inserter(vs, vs.end()));
    copy(mss.begin(), mss.end(), back_inserter(vs));
    printContainer("Copied vector: ", vs.begin(), vs.end());//[good, new, yes, good, new, yes]

    //将关联容器中的元素拷贝到顺序容器中
    multiset<string> mss2;
    copy(vs.begin(), vs.end(), inserter(mss2, mss2.end()));
    //copy(vs.begin(), vs.end(), back_inserter(mss2));//非法掉用，因为multiset不支持push_back操作
    printContainer("Copied multiset: ", mss2.begin(), mss2.end());//[good, good, new, new, yes, yes]

    //Topic: 关联容器的范围查询
    //因为multimap和multiset允许多个相同键值的元素存在，这些相同键值的元素被排序后是逻辑上连续的
    //如果针对某个键值，想要获取所有的元素，可以用下面的方法达到范围查询的效果
    multimap<string, string> mmss = {
        { "Hunk", "A"}, {"Alice", "A"}, {"Jack", "A"}, {"Bob", "D"},
        {"Hunk", "D"}, {"Alice", "D"}, {"Alice", "C"}, {"Hunk", "B"}
    };
    string tgt_name = "Hunk";
    //方法1：count和find结合
    cout << "\nRange search method 1:" << endl;
    auto cnt = mmss.count(tgt_name);
    auto ite = mmss.find(tgt_name);
    while ( cnt )
    {
        cout << "Found " << tgt_name << " = " << ite->second << endl;
        ++ite;
        --cnt;
    }
    //方法2：使用lower_bound和upper_bound获得查询的迭代器范围
    //  lower_bound返回的迭代器指向第一个具有给定键值的元素
    //  upper_bound返回的迭代器指向最后一个匹配给定键值的元素之后的位置
    cout << "Range search method 2:" << endl;
    for (auto begin = mmss.lower_bound(tgt_name),
              end = mmss.upper_bound(tgt_name); 
        begin != end; ++begin)
    {
        cout << "Found " << tgt_name << " = " << begin->second << endl;
    }
    //方法3：直接调用equal_range
    //equal_range返回一个迭代器pair：
    //  若关键字存在，第一个迭代器指向第一个匹配元素。第二个元素指向最后一个匹配元之后的位置
    //  若关键字不存在，两个迭代器都指向关键字可以插入的相同位置
    cout << "Range search method 3:" << endl;
    for (auto pos = mmss.equal_range(tgt_name); 
        pos.first != pos.second; ++pos.first)
    {
        cout << "Found " << tgt_name << " = " << pos.first->second << endl;
    }

    multimap<string,string> authors = {
        {"Hunk", "Book A"}, {"Alice", "Book C"},
        {"Hunk", "Book B"}, {"Alice", "Book D"},
        {"Bob", "Book E"}
    };
    cout << "Author numbers: " << authors.size() << "\n";//5
    for (auto ite = authors.begin(); ite != authors.end(); ite++)
    {
        //以字典序打印所有元素
        cout << "Author name: " << ite->first << ", book name: " << ite->second << "\n";
    }
    
    string rm_author = "Alice";
    auto res = authors.find(rm_author);
    if ( res != authors.end() )
    {
        //对于multimap，erase会删除所有key匹配的元素
        authors.erase(rm_author);
    }
    rm_author = "Jack";
    res = authors.find(rm_author);
    if ( res != authors.end() )
    {
        authors.erase(rm_author);
    }
    cout << "Author numbers: " << authors.size() << "\n";//3
    

    ExitFunc(__FUNCTION__);
}

void TestWordTransform()
{
    EnterFunc(__FUNCTION__);
    string ruleFileName = "trans_rules.txt";
    string sourceFileName = "source_file.txt";

    //写入一个保存转换规则的文件
    ofstream ruleFile(ruleFileName);
    if ( ruleFile ) {
        ruleFile << "k okay?\ny why\nr are\nu you\npic picture\n";
        ruleFile << "thk thanks!\nl8r later\nme \n";
        ruleFile.close();
    }

    //写入一个需要转换的文件
    ofstream sourceFile(sourceFileName);
    if ( sourceFile )
    {
        sourceFile << "where r u\ny dont u send me a pic\n";
        sourceFile.close();
    }
    
    //正式进入转换程序
    map<string, string> trans_map;
    ifstream mapFile(ruleFileName);
    if ( mapFile )
    {
        trans_map = buildMap(mapFile);
    }  else {
        cout << "Failed to open transform rule file" << endl;
    }

    ifstream transFile(sourceFileName);
    if ( transFile )
    {
        string line;
        while ( getline(transFile, line) )
        {
            cout << "\"" << line << "\" ===> \"";
            istringstream lineBuf(line);
            string word;
            bool firstWord = true;
            while ( lineBuf >> word )
            {
                if( firstWord ) {
                    firstWord = false;
                } else {
                    cout << " ";//打印单词间的空格
                }
                cout << transform(word, trans_map);
            }
            cout << "\"\n"; //完成一行的处理
        }
    } else {
        cout << "Failed to open source file" << endl;
    }

    ExitFunc(__FUNCTION__);
}

map<string, string> buildMap(ifstream &mapFile)
{
    map<string, string> trans_map;
    string key;
    string value;
    while ( mapFile >> key && getline(mapFile, value) )
    {
        if ( value.size() > 1 )
        {
            trans_map[key] = value.substr(1);//跳过前导空格
            //trans_map.insert({key, value.substr(1)});
        } else {
            //没有转换规则
            cout << "[Error] No transform rule for word: " << key << endl;
        }
    }
    return trans_map;
}

string transform(string &word, map<string, string> &map)
{
    auto res = map.find(word);
    if ( res != map.end() )
    {
        return map[word];
    } else {
        return word;//返回原单词
    }
}

void TestUnorderedMapAndSet()
{
    EnterFunc(__FUNCTION__);

    unordered_map<string, string> unorderMap;
    printUnorderMapInfo(unorderMap);
    //bucket_count = 1, max_bucket_count = 230584300921369395, load_factor = 0, max_load_factor = 1

    for (size_t i = 0; i < 5; i++)
    {
        unorderMap.insert({ "AAA" + to_string(i), "BBB" + to_string(i) });
    }
    printUnorderMapInfo(unorderMap);
    //bucket_count = 7, max_bucket_count = 230584300921369395, load_factor = 0.714286, max_load_factor = 1

    for (size_t i = 5; i < 15; i++)
    {
        unorderMap.insert({ "AAA" + to_string(i), "BBB" + to_string(i) });
    }
    printUnorderMapInfo(unorderMap);
    //bucket_count = 17, max_bucket_count = 230584300921369395, load_factor = 0.882353, max_load_factor = 1

    for (size_t i = 15; i < 50; i++)
    {
        unorderMap.insert({ "AAA" + to_string(i), "BBB" + to_string(i) });
    }
    printUnorderMapInfo(unorderMap);
    //bucket_count = 79, max_bucket_count = 230584300921369395, load_factor = 0.632911, max_load_factor = 1


    /*
    测试插入大量元素时，不断自动rehash和提前通过reserve预置足够存储空间的效率：
        CASE1: 插入100万个键值对时，差别不大
            unorderMap1用了4s
            unorderMap2用了2s

        CASE2: 插入1000万个键值对时，区别就和明显了
            unorderMap1用了33.560000s
            unorderMap2用了26.733000s
    */
    unordered_map<string, string> unorderMap1, unorderMap2;
    recordRunTime();
    size_t keyNum = 1000 * 10000;
    for (size_t i = 0; i < keyNum; i++)
    {
        unorderMap1.insert({ "ABCD" + to_string(i), "QAZ" + to_string(i) });
    }
    recordRunTime(true);

    //unorderMap2提前预置足够的存储空间
    recordRunTime();
    unorderMap2.reserve(keyNum);
    for (size_t i = 0; i < keyNum; i++)
    {
        unorderMap2.insert({ "ABCD" + to_string(i), "QAZ" + to_string(i) });
    }
    recordRunTime(true);

    ExitFunc(__FUNCTION__);
}
