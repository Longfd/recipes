#include "AdjecentMatrixWDigraph.h"

int main()
{
    AdjecentMatrixWDigraph<int> g(5, 0);
    g.insertEdge(1, 2);
    g.insertEdge(2, 3);
    g.insertEdge(3, 4);
    g.insertEdge(5, 4);
    cout << g;

    vector<int> v1;
    int begin = 1;
    g.bfs(begin, v1);
    cout << "bfs from vertex " << begin << ": ";
    copy(v1.begin(), v1.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    v1.clear();
    g.dfs(begin, v1);
    cout << "dfs from vertex " << begin << ": ";
    copy(v1.begin(), v1.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    return 0;
}