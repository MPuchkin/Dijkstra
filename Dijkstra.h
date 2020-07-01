//---------------------------------------------------------------------------------------------------------
//  ���������� ��������� �������� � �������������� ������� � ������������,
//  ������������� �� ������ �������� ���� �� ������� (������������ ��������). 
//
//  �������� ��������� - ���������� ������ DecreaseKey ��� �������, ��� ��� �
//  �������� ���������� ����� ������� ������������ � ������ ������� � ������������,
//  � ��� ���� ���������� ������ �������� ������� (� ������� �� ������������� ���,
//  � ������� ��������� �� �������� �� ��������������). ��������������, ����� ���������
//  ����������� ���� ���������.
//
//  ���������� � ����� ��� ���������� ������:
//    - ���� ������ ������������� ����������� ��������� cbegin � cend ��� ������ ������
//    - ������� ����� ������ ���������� ���� size_type (��� ����������) � weight_type (���� ����);
//    - ������� ����� ������ ������������� ����������� ��������� cbegin � cend ��� ������ ������� ������
//    - ������� ����� ���������� �� 0 �� V-1 (��� V - ���������� ������ �����), ��� "�����"
//    - ������� ����� ����� ���� size_type name, ������������ ����� �������
//---------------------------------------------------------------------------------------------------------

#pragma once
#include <deque>
#include <vector>
#include <functional>
#include <algorithm>

//---------------------------------------------------------------------------------------------------------
//  ������� ������� - ����������� ���������, �� ���� ������ ��������� �� ������ ��� ������

template<typename T>
struct queueElem {
	T* data;
	queueElem(T & elem) { data = &elem; }
	inline void update_index(size_t index) noexcept { data->update_index(index); }
};

//  �������� ��� ���������� ���� (��� ������-�� ������ "������" �����, �.�. ������ ��� ������������ � �������)
template<typename T>
inline bool operator<(const queueElem<T>& lhs, const queueElem<T>& rhs) { return *(lhs.data) < *(rhs.data); }
template<typename T>
inline bool operator==(const queueElem<T>& lhs, const queueElem<T>& rhs) {	return *(lhs.data) == *(rhs.data); }
template<typename T>
inline bool operator>(const queueElem<T>& lhs, const queueElem<T>& rhs) { return (rhs < lhs); }
template<typename T>
inline bool operator!=(const queueElem<T>& lhs, const queueElem<T>& rhs) { return !(lhs == rhs); }

//---------------------------------------------------------------------------------------------------------
//  ������������ ������� � ������������ �� ������ ����, �� � ��������� Decrease_Key,
//  ���-�� ���� Extended Priority Queue
template<class T, class Container = std::vector<queueElem<T> >, class Compare = std::less<typename Container::value_type>>
class ExtPriority_Queue {

	Compare cmp;  //  ���������� - ����� ����� �� �����, ����� ������ ���� �������� > ������������

	Container cont;  //  ��������� - ������.

	//  ����� ������ ���� ������ � ����� ��������� size_type ������� � size_type �� ������� - ���� ���������
	using size_type = typename Container::size_type;
	
	//  ������������ ������������� ������ �������� ���� � ���� ������� - ��� ���������� �������� � ������/������� ��������
	inline size_type parent(size_type i) const { return (i - 1) / 2; }
	inline size_type   left(size_type i) const { return 2 * i + 1; }
	inline size_type  right(size_type i) const { return 2 * i + 2; }

	//  "�����" ������� � ���� �� ��� �����. �������, ��� � �������� index � �������� ����������� �� ������,
	//  � �������� ���� �� ����������� ������ � ����� �������� - ��� ���� "�������" �� ��� ����������� �����,
	//  ����� ������� � ������������ �� �������� �����.
	void heapify(size_type index) noexcept {
		size_type max_index{ index };
		size_type size{ cont.size() };

		while (true) {

			size_type left_index = left(index);
			if (left_index < size && cmp(cont[index], cont[left_index]))
				max_index = left_index;

			size_type right_index = right(index);
			if (right_index < size && cmp(cont[max_index], cont[right_index]))
				max_index = right_index;

			if (index == max_index) break;

			std::swap(cont[index], cont[max_index]);
			//  ������������ ��������� ������ �����, ��� ������� ��������� �� ����� �����
			cont[index].update_index(index);

			index = max_index;
		}
		//  �� � ������ ���������� � ������ "�����������" ���� ����������� �������� � ����� ����� � �������
		cont[max_index].update_index(max_index);
	}

public:

	ExtPriority_Queue() : cont() {}

	template<class Cont>
	ExtPriority_Queue(Cont& vect) noexcept : cont()
	{
		cont.reserve(vect.size());
		for (auto & x : vect)
			push(x);
	}

	void push(T& elem) noexcept {
		//  ��������� ������� � ����� �������, � �������� DecreaseKey
		cont.push_back(queueElem<T>(elem));
		//  ����� ���� ���������! ����������� ������� ����� �������� index, � ��� ������ ��������� �������
		//  decreaseKey - ��� ����������� ���� ������� ���������� �������
		decreaseKey(cont.size() - 1);
	}
	
	//  ���������� �������� �� ������ �������
	void pop() noexcept {
		//  �������� ��������� ������� ���������
		cont[0] = std::move(cont[cont.size() - 1]);
		//  ��������� ����-����� ��������� �� ������ (��� ��� � ������ ���������)
		cont.pop_back();
		//  ������ ���������� ����� update_index
		heapify(0);
	}
	
	//  ��� � ���������� �� ���������
	inline const T& top() const {
		return *(cont[0].data);
	}

	//  �������, ��� ��������� ������ � ��������, � ��� ���� ����������� �� ������ �����
	void decreaseKey(size_type index) noexcept {
		while (index > 0 && cmp(cont[parent(index)], cont[index])) {
			std::swap(cont[index], cont[parent(index)]);
			//  � ��������� ������� ���������� ������
			cont[index].update_index(index);
			index = parent(index);
		}
		cont[index].update_index(index);
	}

	inline bool empty() const { return cont.empty(); }

	//  ���������� ������ - ����� � ��������
	void print() const {
		std::cout << "PQueue size " << cont.size() << std::endl;

		for (size_type i = 0; i < cont.size(); ++i) {
			std::cout << i << " : " << cont[i].data->vertex->name << " ind(";
			std::cout << (cont[i].data->index) << ") w(" << cont[i].data->weight << ");  ";
		}
		std::cout << std::endl;
	}
};

//-------------------------------------------------------------------------------

//  ��������� ������� � ��������� �������� - ����������, �������, �������
enum class vertexState {None, Opened, Finished};

//  ������ ��� ������ �����, �������������� � ��������� ��������
//  ������ ��� ����������� ����, � ����� ��������� � ����� �������� ����
template<typename vertexType>
struct graphVertex {
	using size_type = typename vertexType::size_type;
	using weight_type = typename vertexType::weight_type;

	const vertexType * vertex;
	size_type parent;
	weight_type weight;
	vertexState state;
	size_t index;
	graphVertex(const vertexType& graphVertex, size_t queueIndex = std::numeric_limits<size_type>::max()) noexcept :
		vertex(&graphVertex),
		parent(std::numeric_limits<size_type>::max()),
		weight(std::numeric_limits<weight_type>::max()),
		state(vertexState::None),
		index(queueIndex)
	{}

	inline void update_index(size_t new_index) { index = new_index; }

	inline bool operator<(const graphVertex<vertexType>& other) const { return weight < other.weight; }
	inline bool operator>(const graphVertex<vertexType>& other) const { return weight > other.weight; }

};

//  ������� ��� ��������� ��������
template<typename vertexType>
class Dijkstra {
private:
	
	//  ���� ��� �������� � �����
	using size_type = typename graphVertex<vertexType>::size_type;
	using weight_type = typename graphVertex<vertexType>::weight_type;
	
	//  ���������� �� ��������� ������ �������� � 
	std::vector<graphVertex<vertexType>> nodes;
public:
	//  ����������� - ������ ��������� � ������������� �����
	//  �������, ��� � �������� ����� ���������� � 0, ������� �������������
	template<typename vertexIter>
	Dijkstra(vertexIter & begin, vertexIter & end) {
		for (auto it = begin; it != end; ++it)
			nodes.push_back(graphVertex<vertexType>(*it));
	}

	//  ����������� - ������ ��������� � ������������� �����
	template<typename vertexCont>
	explicit Dijkstra(const vertexCont & cont) {
		for (auto it = cont.cbegin(); it != cont.cend(); ++it)
			nodes.push_back(graphVertex<vertexType>(*it));
	}

	//  ����������, ��� �������� �������� - ���� ������������������ �������� ������, � ��������� ����
	std::pair<std::vector<size_type>, weight_type> calcPath(size_type startIndex, size_type finishIndex) {
		//  ������� ������ ������� � ����������� � ������ ���� ��������� �������
		if (startIndex >= nodes.size() || finishIndex >= nodes.size())
			throw std::out_of_range("Wrong start or finish node index!");

		using queueElemType = queueElem<graphVertex<vertexType>>;

		ExtPriority_Queue <graphVertex<vertexType>, std::vector<queueElemType>, std::greater<queueElemType> > epq;
		//  ����� ��������� ��������, � � ������� �, �������
		nodes[startIndex].parent = startIndex;
		nodes[startIndex].weight = 0;
		nodes[startIndex].state = vertexState::Opened;
		epq.push(nodes[startIndex]);

		//  ������� ������� - ���������� �������� � ������, ���� ������� �� ������ ����� ������ ������ ������� -
		//    � ����� ������ ������� �� ������� �� �������, � ����� �����������
		weight_type record = std::numeric_limits<weight_type>::max();

		while (!epq.empty()) {
			//  ����������� ��������� ������� �� ������� (� �� �� ������ ������ ����������� ������)
			
			//  � ������� �� ������!!!! top ���������� �� ������ �� ������ �������, � �������, �� �������
			//  ��������� ������ �������, ��� ��� ������ ������� �������� ����� �������, ������ ����� �����
			const graphVertex<vertexType> &current(epq.top());
			record = current.weight;  //  ��������� �������� �������� ������� ������ � �������
			epq.pop();

			//  ���� ������� ������� �������, ��� �� � �� ������ ������ ���� ����� ������ �������, �� �������
			if (current.vertex->name == finishIndex || nodes[finishIndex].weight < record)
			{
				//  ������ ���� �� ������ ���������
				std::vector<size_type> path;
				size_type nodeIndex(finishIndex);
				path.push_back(nodeIndex);
				while (nodeIndex != startIndex) {
					nodeIndex = nodes[nodeIndex].parent;
					path.push_back(nodeIndex);
				}
				std::reverse(path.begin(), path.end());
				return make_pair(path, current.weight);
			}
			
			//  ��������� ������� ������� ��� ��������
			nodes[current.vertex->name].state = vertexState::Finished;

			//  ��� ���� ��������� � �������
			for (auto adjIt = current.vertex->cbegin(); adjIt != current.vertex->cend(); ++adjIt)
				if (nodes[adjIt->dest].state == vertexState::None) {
					//  ��� ������� ��� �� ���������, � � ����� ������ � ������� ���������
					nodes[adjIt->dest].state = vertexState::Opened;
					nodes[adjIt->dest].parent = current.vertex->name;
					nodes[adjIt->dest].weight = nodes[current.vertex->name].weight + adjIt->weight;
					epq.push(nodes[adjIt->dest]);
				}
				else {
					//  ��� ���������, ������������� - ����� ����, ������ ����������
					if (nodes[adjIt->dest].state != vertexState::Finished && nodes[adjIt->dest].weight > nodes[current.vertex->name].weight + adjIt->weight) {
						nodes[adjIt->dest].weight = nodes[current.vertex->name].weight + adjIt->weight;
						nodes[adjIt->dest].parent = current.vertex->name;
						epq.decreaseKey(nodes[adjIt->dest].index);
					}
				}
		}
		//  ���� �� ���� ������, �� �� ����� - ���� �� ����������
		return make_pair(std::vector<size_type>(), 0);

	}
};