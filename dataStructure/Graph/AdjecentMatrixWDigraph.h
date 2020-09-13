#ifndef AdjecentMatrixWDigraph
#define AdjecentMatrixWDigraph

#include <vector>
#include <assert.h>

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

    int numberOfVertices() { return n; }
    int numberOfEdges() { return e_; }
    bool existEdge(int v1, int v2) const 
    {
        if (0 < v1 && v1 <= n && 0 < v2 && v2 <= n && g_[v1][v2] != noEdge_)
            return true;
        else
            return false;
    }

    // update if the edge is already there
    void insertEdge(int v1, int v2, const T& weight = 1) 
    {
        assert(0 < v1 && v1 <= n);
        assert(0 < v2 && v2 <= n);
        assert(v1 != v2);
        
        if(g_[v1][v2] == noEdge_) 
            ++e_;
        g_[v1][v2] = weight;
    }

    void eraseEdge(int v1, int v2)
    {
        assert(0 < v1 && v1 <= n);
        assert(0 < v2 && v2 <= n);
        assert(v1 != v2);
        
        if(g_[v1][v2] != noEdge_) {
            g_[v1][v2] = noEdge_;
            --e_;
        }
    }



};

#endif
