#include <iostream>
#include <ctime>
#include "Graph.h"
#include "Dijkstra.h"

using namespace std;

int main(int argc, char* argv[])
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    system("chcp 65001");
#endif

    const Graph G = [argc, argv]{
        Graph G;
        if (argc == 1) {
            G.generateGraph(100);
        } else {
            G.loadFromFile(argv[1]);
        }
        return G;
    }();

    const vertex::size_type startNode = 1;
    const vertex::size_type finishNode = min(G.size()-1, vertex::size_type(9563));
    
    time_t start = clock();
    Dijkstra<vertex> dk(G);
    auto v = dk.calcPath(startNode, finishNode);
    time_t finish = clock();
    
    cout << "Кратчайший путь: ";
    for (auto x : v.first)
    {
        cout << x << " ";
    }

    cout << "\nДлина пути из " << startNode << " в " << finishNode << " равна " << v.second << "\n";
    cout << "Time: " << double(finish - start) / CLOCKS_PER_SEC << " seconds\n";

    /*G.saveToFile("graph.txt");
    system("pause");
    G.loadFromFile("graph.txt");
    //G.printGraph();
    system("graph.txt");*/

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    system("pause");
#endif

    return 0;
}

