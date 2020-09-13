#ifndef AdjecentMatrixWDigraph_H
#define AdjecentMatrixWDigraph_H

#include <vector>
#include <assert.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_set>

using namespace std;

// T为权类型, 如果为无权图, T为bool类型
template<typename T>
class AdjecentMatrixWDigraph
{
private:
    int n_; // 顶点数
    T noEdge_; 
    int e_; // 边数
    vector<vector<T>> g_; 

public:
    AdjecentMatrixWDigraph(int n, const T& noEdge) 
    : n_(n), 
      noEdge_(noEdge),
      e_(0)
    {
        for (int i = 0; i < n+1; ++i) {
            g_.push_back(vector<T>(n+1, noEdge_)); // (n+1)*(n+1)矩阵, 假设顶点数字表示 n > 0
        }
    }

    int numberOfVertices() { return n_; }
    int numberOfEdges() { return e_; }
    bool existEdge(int v1, int v2) const 
    {
        if (0 < v1 && v1 <= n_ && 0 < v2 && v2 <= n_ && g_[v1][v2] != noEdge_)
            return true;
        else
            return false;
    }
    
    void output(ostream& out) const
    {// Output the adjacency matrix.
        for (int i = 1; i <= n_; i++)
        {
            copy(g_[i].begin()++, g_[i].end(), ostream_iterator<T>(out, "  "));
            out << endl;
        }
    } 

    // update if the edge is already there
    void insertEdge(int v1, int v2, const T& weight = 1) 
    {
        assert(0 < v1 && v1 <= n_);
        assert(0 < v2 && v2 <= n_);
        assert(v1 != v2);
        
        if(g_[v1][v2] == noEdge_) 
            ++e_;
        g_[v1][v2] = weight;
    }

    void eraseEdge(int v1, int v2)
    {
        assert(0 < v1 && v1 <= n_);
        assert(0 < v2 && v2 <= n_);
        assert(v1 != v2);
        
        if(g_[v1][v2] != noEdge_) {
            g_[v1][v2] = noEdge_;
            --e_;
        }
    }

    void bfs(int v, vector<int>& reach) 
    {
        unordered_set<int> hash;
        queue<int> que;
        que.push(v);

        while(!que.empty()) {
          int curVertex = que.front();
          que.pop();

          for(int i = 1; i < n_+1; ++i) {
            if (g_[curVertex][i] != noEdge_ && hash.find(i) == hash.end()) {
                que.push(i);
                hash.insert(i);
                reach.push_back(i);
            }  
          }
        }
    }

    void rdfs(int v, vector<int>& reach, unordered_set<int>& hash) 
    {
        for (int i = 1; i < n_+1; ++i) {
          if (g_[v][i] != noEdge_ && hash.find(i) == hash.end()) {
              hash.insert(i);
              reach.push_back(i);
              rdfs(i, reach, hash);
          }
        }
    }

    void dfs(int v, vector<int>& reach) 
    {
        unordered_set<int> hash;
        rdfs(v, reach, hash);
    }


};

template <class T>
ostream& operator<<(ostream& out, const AdjecentMatrixWDigraph<T>& g)
{
  g.output(out); 
  return out;
}

#endif
