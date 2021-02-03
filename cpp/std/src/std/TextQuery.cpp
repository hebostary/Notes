#include "TextQuery.hpp"

TextQuery::TextQuery(string file)
{
    ifstream in(file);
    if( !in ) {
        cout << "[Error] Failed to open file: "<< file << endl;
        return;
    }

    string line, word;
    mTextLines.reset(new vector<string>);//构造函数里首先对shared_ptr进行初始化
    while ( getline(in, line) ) 
    {
        istringstream words(line);
        mTextLines->push_back(line);
        line_no lineNum = mTextLines->size()-1;
        while ( words >> word ) 
        {
            if ( mWordLines.find(word) == mWordLines.end() )
            {
                //第一次读到这个单词，创建一个基于set的share_ptr指针
                mWordLines[word] = make_shared<set<line_no>>();
            }
            mWordLines[word]->insert(lineNum);
        }
    }
}

TextQuery::~TextQuery()
{
}

QueryResult TextQuery::query(const string& word) const
{
    static shared_ptr<set<line_no>> no_data(new set<line_no>);//一定记得初始化

    auto loc = mWordLines.find(word);
    if ( loc == mWordLines.end() )
    {
        //没找到单词，返回空的行号集合
        return QueryResult(word, no_data, mTextLines);
    }

    //找到了这个单词，直接返回共享的文本行和单词行号的集合就可以
    return QueryResult(word, loc->second, mTextLines);
}

ostream& printQueryResult(ostream& out, const QueryResult& qr)
{
    out << "[" << qr.mWord << "] occurs " 
        << qr.mLines->size() << " times." << endl;
    
    for ( auto num : *qr.mLines) {
        out << "(line " << num << ") " << qr.mText.get()->at(num) << endl;
    }

    return out;
}
