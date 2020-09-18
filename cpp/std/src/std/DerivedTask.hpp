#ifndef DERIVEDTASK_H_
#define DERIVEDTASK_H_

#include "../common/common.hpp"
#include "BaseTask.hpp"

class DerivedTask : public BaseTask
{
private:
    int m_start_offset;

public:
    DerivedTask();
    DerivedTask(int offset) : m_start_offset(offset) {}
    ~DerivedTask();

    int getOffset() { return m_start_offset; }

    virtual string serialize();
};

void TestDerivedTask();

#endif