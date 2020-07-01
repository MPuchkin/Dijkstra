#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <functional>
#include <ctime>
#include "Graph.h"
#include "Dijkstra.h"

using namespace std;

int main()
{
    setlocale(LC_ALL, "RUSSIAN");
    Graph G;
    //G.generateGraph(10);
    G.loadFromFile("Dijkstra.txt");
    //cout << G << endl;
    
    const vertex::size_type startNode = 1;
    const vertex::size_type finishNode = 9563;
    
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
    return 0;
}

