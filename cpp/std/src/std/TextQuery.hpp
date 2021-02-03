#ifndef TEXT_QUERY_H_
#define TEXT_QUERY_H_

#include "../common/common.hpp"

using namespace std;
using line_no = vector<string>::size_type;

class QueryResult;//重要，为了定义query函数的返回类型，这个定义是必须的
class TextQuery
{
public:
    TextQuery(string file);
    ~TextQuery();
    QueryResult query(const string& word) const;
private:
    shared_ptr<vector<string>>  mTextLines; //保存文本行数据
    map<string, shared_ptr<set<line_no>>> mWordLines; //保存每个单词所出现的行号
};

class QueryResult
{
//友元函数
friend ostream& printQueryResult(ostream& out, const QueryResult& qr);

public:
    QueryResult(string word, 
            shared_ptr<set<line_no>> lines,
            shared_ptr<vector<string>> text) : 
        mWord(word), mLines(lines), mText(text) { };

private:
    string mWord; //查询的单词
    shared_ptr<set<line_no>> mLines; //单词出现的行号，因为和TextQuery共享底层数据，避免了对set和vector的拷贝操作
    shared_ptr<vector<string>> mText; //文件文本
};

ostream& printQueryResult(ostream& out, const QueryResult& qr);

#endif