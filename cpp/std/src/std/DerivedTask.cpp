#include "DerivedTask.hpp"

DerivedTask::DerivedTask()
{
    cout << "DerivedTask::DerivedTask()" << endl;
}

DerivedTask::~DerivedTask()
{
    cout << "DerivedTask::~DerivedTask()" << endl;
}

string DerivedTask::serialize()
{
    cout << "DerivedTask::serialize()" << endl;
    return string("m_id:"+getID()+", m_type:"+getType()+", m_start_offset:"+to_string(m_start_offset));
}


void TestDerivedTask()
{
    cout<<"***********[Begin] TestDerivedTask**********"<<endl;
    DerivedTask task1;
    task1.printTask();

    BaseTask task2;
    string task2Str(task2);
    cout << "task2Str: " << task2Str << endl;
    int total = 10 + task2;
    cout << "task2Int: " << total << endl;
    cout<<"***********[End] TestDerivedTask**********"<<endl;
}
