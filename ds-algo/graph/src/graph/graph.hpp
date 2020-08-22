#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

void graphTest();

class graph
{
private:
    int V = 0;//顶点数
    int E = 0;//边数
    vector<vector<int>* >  adj;//邻接表
    bool found = false;//用户dfs的搜索标志

public:
    graph(int v);
    ~graph();

    int getV() { return this->V; }
    int getE() { return this->E; }

    void addEdge(int v, int w);
    vector<int>* getAdj(int v);//返回某个顶点的相邻顶点
    int degree(int v);//返回顶点的度，即临接顶点数

    void dfs(int v, int w);//深度优先搜索两顶点之间的路径
    void bfs(int v, int w);//广度优先搜索两顶点之间的路径

private:
    void recurDfs(int v, int w, bool visited[], int prev[]);
    void validateVertex(int v);//检查是否是有效的顶点
    void print(int prev[], int v, int w);
};


#endif
