#ifndef IO_LIBRARY_H_
#define IO_LIBRARY_H_

#include "../common/common.hpp"

void TestIOLibrary();

void Testiostream();
istream& readFromIstream(istream& in );

void Testiofstream();

void Teststringstream();

struct PersonInfo
{
    string name;
    vector<string> phones;
};


#endif