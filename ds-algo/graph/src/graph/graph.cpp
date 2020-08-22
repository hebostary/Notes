#include "graph.hpp"

graph::graph(int v)
{
    if (v < 0)
    {
        cout << "Invalid initial vertex value " + to_string(v) << endl;
        exit(1);
    }

    this->V = v;
    this->E = 0;
    this->adj = vector<vector<int>* >(v);
    for (size_t i = 0; i < v; i++)
    {
        this->adj[i] = new vector<int>;
    }
    cout<<"[INFO] graph vertex size: " << this->V << endl;
}

graph::~graph()
{
    for (size_t i = 0; i < this->V; i++)
    {
        delete this->adj[i];
        this->adj[i] = NULL;
    }
}

//在图中添加一条边
void graph::addEdge(int v, int w)
{
    //无向图需要在边的两个顶点上都加上对端顶点
    validateVertex(v);
    validateVertex(w);
    this->E = (this->E)+1;
    this->adj[v]->push_back(w);
    this->adj[w]->push_back(v);
    cout<<"[INFO] added edge (" << v << " -- " << w << "), edge size: "<< this->E <<", vertex size: "<< this->V<<endl;
}

vector<int>* graph::getAdj(int v)
{
    validateVertex(v);
    return this->adj[v];
}

int graph::degree(int v)
{
    return this->adj[v]->size();
}

/*
* 深度优先搜索
*/
void graph::dfs(int v, int w)
{
    cout<<"\n[bfs] Begin to execute dfs..."<<endl;
    validateVertex(v);
    validateVertex(w);
    bool visited[this->V];
    for (size_t i = 0; i < this->V; i++) visited[i] = false;
    int prev[this->V];
    for (size_t i = 0; i < this->V; i++) prev[i] = -1;
    found = false;

    recurDfs(v, w, visited, prev);
    print(prev, v, w);
}

void graph::recurDfs(int v, int w, bool visited[], int prev[])
{
    if (found) return;
    visited[v] = true;
    
    if (v == w)
    {
        //搜索到目标顶点，开始沿着递归路径返回
        found = true;
        return;
    }
    
    cout<<"[dfs] begin to access vertex "<<v<<endl;
    vector<int>* vAdj = getAdj(v);
    for (vector<int>::iterator ite =  vAdj->begin(); ite != vAdj->end(); ite++)
    {
        if (!visited[*ite])
        {
            prev[*ite] = v;
            recurDfs(*ite, w, visited, prev);
        }
    }
}

/*
* 广度优先搜索： v -> w的路径
*/
void graph::bfs(int v, int w)
{
    cout<<"\n[bfs] Begin to execute bfs..."<<endl;
    validateVertex(v);
    validateVertex(w);
    if(v == w) return;
    bool visited[this->V];
    for (size_t i = 0; i < this->V; i++) visited[i] = false;
    visited[v] = true;
    queue<int> q;
    q.push(v); //把起点顶点首先加入队列进行访问
    int prev[this->V];
    for (size_t i = 0; i < this->V; i++) prev[i] = -1;

    while (q.size() != 0)
    {
        //取出队列中的顶点，然后依次遍历搜索其临接顶点
        int vertex = q.front();//返回队列头部元素的引用
        q.pop();//删除队列首部元素
        cout<<"[bfs] begin to access vertex "<<vertex<<endl;
        vector<int>* vAdj = getAdj(vertex);
        for (vector<int>::iterator ite =  vAdj->begin(); ite != vAdj->end(); ite++)
        {
            cout<<"[bfs] begin to access adjacent vertex "<<*ite <<" of vertex " <<vertex<<endl;
            if (!visited[*ite])//跳过已经访问过的顶点
            {
                prev[*ite] = vertex;
                if (*ite == w)
                {
                    print(prev, v, w);
                    return;
                }
                visited[*ite] = true;
                q.push(*ite);//访问一个顶点时，依次将其临接顶点加入到队列中
                cout<<"[bfs] added vertex "<<*ite<<" to queue"<<endl;
            }
        }
    }
}

void graph::print(int prev[], int v, int w)
{
    if (prev[w] != -1 && v != w)
    {
        print(prev, v, prev[w]);
    }
    cout<<w<<" -> ";
}

void graph::validateVertex(int v)
{
    if (v < 0 || v >= this->V)
    {
        cout << "Invalid vertex value: " +  to_string(v) << endl;
        exit(1);
    }
}

void graphTest() {
    cout<<"***********[Begin] graphTest**********"<<endl;
    /*构造下面的无向图
    0--1
    |  |
    2--4--3
    |  |
    5--6--7
    */
    graph* g = new graph(8);
    g->addEdge(0, 1);
    g->addEdge(0, 2);
    g->addEdge(1, 4);
    g->addEdge(2, 4);
    g->addEdge(3, 4);
    g->addEdge(2, 5);
    g->addEdge(4, 6);
    g->addEdge(5, 6);
    g->addEdge(6, 7);

    g->bfs(0, 7);
    g->dfs(0, 7);
    cout<<"\n***********[End] graphTest**********"<<endl;
}