#include "IOLibrary.hpp"
#include <iostream> //处理普通流
#include <fstream> //处理文件流
#include <sstream> //处理string流


void TestIOLibrary()
{
    EnterFunc(__FUNCTION__);

    //Testiostream();

    Testiofstream();

    Teststringstream();

    ExitFunc(__FUNCTION__);
}

void Testiostream()
{
    EnterFunc(__FUNCTION__);

    readFromIstream(cin);

    ExitFunc(__FUNCTION__);
}

istream& readFromIstream(istream& in )
{
    EnterFunc(__FUNCTION__);
    //注意不同系统的文件结束符是不一样的
    cout << "Please input some words and end with (ctl + z) in windows or (ctl + d) in linux:" << endl;
    vector<string> vs;
    string word;
    while ( in >> word )
    {
        vs.push_back(word);
    }
    if ( in.eof() ) {
        cout << "Reached EOF!" << endl;
    }

    printContainer("Readed words: ", vs.begin(), vs.end());

    in.clear();//对流进行置位，使其处于有效状态

    ExitFunc(__FUNCTION__);
    return in;
}


//文件流
//创建 fstream 对象时，如果提供了文件名，则 open 会自动被调用
//当一个 fstream 对象被销毁时，close会自动被调用
void Testiofstream()
{
    EnterFunc(__FUNCTION__);

    //创建一个文件流，并写入几行数据
    //与ofstream关联的文件默认以 out 和 trunc的模式打开，文件的内容会被丢弃
    //阻止ofstream清空给定文件内容的方法是同时指定 app 模式
    string fn = "output.txt";
    ofstream ofs(fn);
    if ( ofs )
    {   //检查打开文件流是否成功，因为open调用可能失败
        ofs << "hunk\njack\nbob" << endl;
        ofs.close();
    }

    //一旦一个文件流已经打开，它就保持与对应文件的关联
    //对一个已经打开的文件流调用会失败，failbit被置位
    //调用close关闭文件后，流对象可以重新打开的新的文件
    ofs.open( fn + ".copy");
    if ( ofs )
    {   //打开文件流成功
        ofs << "hunk\njack\nbob" << endl;
        ofs.close();
    }

    //从文件流读取数据
    //与ifstream关联的文件默认以 in 模式打开
    ifstream ifs( fn + ".copy");
    if ( ifs )
    {
        vector<string> vs;
        string buf;
        while ( getline( ifs, buf ) ) //每次读取一行
        {
            vs.push_back(buf);
        }
        ifs.close();
        printContainer("Read data from file: ", vs.cbegin(), vs.cend());
    }
    
    ExitFunc(__FUNCTION__);
}

void Teststringstream()
{
    EnterFunc(__FUNCTION__);

    string fn("persons.record");
    ofstream ofs( fn );
    if ( ofs )
    {
        ofs << "jack 1234567 12323543543\n";
        ofs << "hunk 34436456 12323543\n";
        ofs << "bob 39444567 1233543\n";
        ofs.close(); //关闭文件并将缓存刷到文件
    }

    ifstream ifs( fn );
    if ( ifs )
    {
        string line, word;
        vector<PersonInfo> people;
        while ( getline(ifs, line) )
        {
            PersonInfo info;
            istringstream record(line); //将记录绑定到刚读入的行
            record >> info.name;
            while ( record >> word )
            {
                info.phones.push_back(word);
            }
            people.push_back(info);
        }
        cout << "read people nmber: " << people.size() << endl; //3
    }

    ExitFunc(__FUNCTION__);
}