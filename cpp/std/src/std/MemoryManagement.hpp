#ifndef MEMORY_MANAGEMENT_H_
#define MEMORY_MANAGEMENT_H_

#include "../common/common.hpp"
#include <memory>
#include <new>
#include "TextQuery.hpp"
#include <fstream>

void TestMemoryManagement();

void TestSharedPtr();
void process(shared_ptr<int> ptr);
void closeFstream(ofstream *fp);

void TestUniquePtr();

void TestDynamicArray();

void TestAllocator();

void TestTextQuery();
bool prepareText(string file);

void TestValgrind();

#endif