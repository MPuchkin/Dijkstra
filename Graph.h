#pragma once

#include <list>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <numeric>
#include <initializer_list>
#include <deque>
#include <exception>

//  Ребро
struct edge {
	using size_type = size_t;
	using weight_type = size_t;
	//  Куда ведёт ребро
	size_type dest;
	//  Вес ребра
	weight_type weight;
	edge(size_type Dest, weight_type W) : dest(Dest), weight(W) {}
};

//  Вершина графа
class vertex {
public:
	using size_type = edge::size_type;
	using weight_type = edge::weight_type;
	//  Имя - это просто номер/индекс
	size_type name;
	//  Список смежных вершин
	std::list<edge> adj;

	vertex(size_type Name) : name(Name) {}
	
	//  Добавление ребра
	void addEdge(size_type dest, weight_type w) {
		adj.push_back(edge(dest, w));
	}
	
	//  Константные итераторы на списки смежных рёбер
	std::list<edge>::const_iterator cbegin() const { return adj.cbegin(); }
	std::list<edge>::const_iterator cend() const { return adj.cend(); }
};

//  Представление графа
class Graph {
public:
	using sizetype = vertex::size_type;
	using weighttype = vertex::weight_type;
	//  Количество вершин
	sizetype grSize;

	// Список вершин (со списками смежности)
	std::vector<vertex> vertices;


	sizetype size() const { return grSize; }

	std::vector<vertex>::iterator begin() { return vertices.begin(); }
	std::vector<vertex>::iterator end() { return vertices.end(); }

	std::vector<vertex>::const_iterator cbegin() const { return vertices.cbegin(); }
	std::vector<vertex>::const_iterator cend() const { return vertices.cend(); }


	//  Генерация случайного графа
	void generateGraph(sizetype Size) {
		grSize = Size;
		vertices.clear();
		vertices.reserve(grSize);
		//  Сразу заполняем вектор вершин
		for (sizetype i = 0; i < grSize; ++i)
			vertices.push_back(vertex(i));

		std::default_random_engine generator;
		std::uniform_int_distribution<sizetype> distribution(1, grSize - 1);
		std::uniform_int_distribution<sizetype> distribution2(1, 100000);

		//  Отметки рёбер, которые уже добавлены
		std::vector<sizetype> marks(grSize, std::numeric_limits<sizetype>::max());

		for (sizetype vert = 0; vert < grSize; ++vert) {
			
			//  Случайно добавляем рёбра
			for (size_t i = 0; i < 100; ++i) {
				sizetype dest = distribution(generator);
				weighttype w = distribution2(generator);
				
				//  Если это петля или такое ребро уже добавляли, то пропуск
				if (dest == vert || marks[dest] == vert) continue;

				vertices[vert].addEdge(dest, w);
				//vertices[dest].addEdge(vert, w);
				//  Значит, ребро из vert в dest уже было
				marks[dest] = vert;
			}
		}
	}

	void printGraph() {
		std::cout << "-----------Graph-----------\n  Size : " << grSize << std::endl;

		for (sizetype i = 0; i < grSize; ++i) {
			std::cout << " " << i << " : ";
			for (auto edg : vertices[i].adj) {
				std::cout << edg.dest << "(" << edg.weight << ")  ";
			}
			std::cout << '\n';
		}
		std::cout << "---------------------------------------\n";
	}

	void saveToFile(std::string filename) const {
		std::ofstream out(filename, std::ios_base::out);
		out << grSize << '\n';
		for (sizetype i = 0; i < grSize; i++)
			if(vertices[i].adj.size() > 0)
			{
				out << i << ' ' << vertices[i].adj.size() << '\n';
				for (auto edg : vertices[i].adj)
					out << edg.dest << ' ' << edg.weight << '\n';
			}
		out.close();
	}

	void loadFromFile(std::string filename) {
		std::ifstream in(filename, std::ios_base::in);
		sizetype Size;
		in >> Size;
		grSize = Size;
		vertices.clear();
		vertices.reserve(grSize);
		for (sizetype i = 0; i < grSize; ++i)
			vertices.push_back(vertex(i));

		sizetype vert, N;
		while (in >> vert >> N) {
			for (sizetype i = 0; i < N; ++i) {
				sizetype dest;
				weighttype w;
				in >> dest >> w;
				vertices[vert].addEdge(dest, w);
			}
		}
		in.close();
	}
};

//  Этот метод необходим для тестирования графа - проверяем доступ извне по константным итераторам
inline std::ostream & operator<<(std::ostream & out, const Graph & gr) {
	out << "Graph  (" << gr.size() << ") nodes \n";
	for (auto nodeIt = gr.cbegin(); nodeIt != gr.cend(); ++nodeIt) {
		out << "  Node " << nodeIt->name << '\n';
		for (auto adjIt = nodeIt->cbegin(); adjIt != nodeIt->cend(); ++adjIt) {
			out << nodeIt->name << " -> " << adjIt->dest << "(" << adjIt->weight << ")\n";
		}
	}
	return out;
}

