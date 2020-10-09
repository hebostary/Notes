#ifndef STRING_H_
#define STRING_H_

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

//如果找到匹配的子串，则返回起始索引，否则返回-1
int BFMatch(const string s, const string parten);

//如果找到匹配的子串，则返回起始索引，否则返回-1
int RKMatch(const string s, const string parten);

int KMPMatch(const string str, const string parten);
vector<int> getNexts(const char* pChar);

void StringMatchTest();

#endif
