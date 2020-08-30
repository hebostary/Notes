#include "BaseTask.hpp"

BaseTask::BaseTask()
{
    cout << "BaseTask::BaseTask()" << endl;
    m_id = createUUID();
    m_type = "Base";
}

BaseTask::BaseTask(string id, string type)
{
    cout << "BaseTask::BaseTask(" << id <<", "<<type<< ")" << endl;
    m_id = id;
    m_type = type;
}

BaseTask::~BaseTask()
{
    cout << "BaseTask::~BaseTask()" << endl;
}

void BaseTask::printTask()
{
    cout << "BaseTask::printTask()" << endl;
    cout << serialize() << endl;
}

string BaseTask::serialize()
{
    return string("[m_id:" + m_id + ", m_type: " + m_type +"]");
}
