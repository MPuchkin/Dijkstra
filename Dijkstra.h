//---------------------------------------------------------------------------------------------------------
//  Реализация алгоритма Дейкстры с использованием очереди с приоритетами,
//  реализованной на основе бинарной кучи на массиве (классическая пирамида). 
//
//  Основная сложность - реализация метода DecreaseKey для очереди, так как в
//  процессе уменьшения ключа элемент перемещается к началу очереди с приоритетами,
//  и при этом перемещает другие элементы очереди (в отличие от фибоначчиевых куч,
//  в которых итераторы на элементы не инвалидируются). Соответственно, нужно учитывать
//  перемещение этих элементов.
//
//  Требования к графу для корректной работы:
//    - граф должен предоставлять константные итераторы cbegin и cend для списка вершин
//    - вершина графа должна определять типы size_type (для индексации) и weight_type (веса рёбер);
//    - вершина графа должна предоставлять константные итераторы cbegin и cend для обхода смежных вершин
//    - вершины графа нумеруются от 0 до V-1 (где V - количество вершин графа), без "дырок"
//    - вершина графа имеет поле size_type name, определяющее номер вершины
//---------------------------------------------------------------------------------------------------------

#pragma once
#include <deque>
#include <vector>
#include <functional>
#include <algorithm>

//---------------------------------------------------------------------------------------------------------
//  Элемент очереди - легковесная структура, по сути просто указатель на обёртку над вершиной

template<typename T>
struct queueElem {
	T* data;
	queueElem(T & elem) { data = &elem; }
	inline void update_index(size_t index) noexcept { data->update_index(index); }
};

//  Операции над элементами кучи (тут вообще-то только "больше" нужно, т.к. только она используется в очереди)
template<typename T>
inline bool operator<(const queueElem<T>& lhs, const queueElem<T>& rhs) { return *(lhs.data) < *(rhs.data); }
template<typename T>
inline bool operator==(const queueElem<T>& lhs, const queueElem<T>& rhs) {	return *(lhs.data) == *(rhs.data); }
template<typename T>
inline bool operator>(const queueElem<T>& lhs, const queueElem<T>& rhs) { return (rhs < lhs); }
template<typename T>
inline bool operator!=(const queueElem<T>& lhs, const queueElem<T>& rhs) { return !(lhs == rhs); }

//---------------------------------------------------------------------------------------------------------
//  Классическая очередь с приоритетами на основе кучи, но с операцией Decrease_Key,
//  что-то типа Extended Priority Queue
template<class T, class Container = std::vector<queueElem<T> >, class Compare = std::less<typename Container::value_type>>
class ExtPriority_Queue {

	Compare cmp;  //  Компаратор - здесь особо не нужен, можно просто было операцию > использовать

	Container cont;  //  Контейнер - вектор.

	//  Очень плохая идея мешать в одном заголовке size_type вектора и size_type из вершины - каша получится
	using size_type = typename Container::size_type;
	
	//  Классическое представление дерева двоичной кучи в виде массива - так определяем родителя и левого/правого потомков
	inline size_type parent(size_type i) const { return (i - 1) / 2; }
	inline size_type   left(size_type i) const { return 2 * i + 1; }
	inline size_type  right(size_type i) const { return 2 * i + 2; }

	//  "Топим" элемент в куче до его места. Считаем, что у элемента index в дочерних поддеревьях всё хорошо,
	//  и свойство кучи не выполняется только в самом элементе - его надо "утопить" до его правильного места,
	//  меняя местами с максимальным из дочерних узлов.
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
			//  Родительская структура должна знать, что элемент перемещён на новое место
			cont[index].update_index(index);

			index = max_index;
		}
		//  Ну и теперь собственно у нашего "утопленника" тоже информируем родителя о новом месте в очереди
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
		//  Добавляем элемент в конец очереди, и вызываем DecreaseKey
		cont.push_back(queueElem<T>(elem));
		//  Здесь надо осторожно! Размещённый элемент имеет неверный index, и это должна поправить функция
		//  decreaseKey - там обязательно надо сделать обновление индекса
		decreaseKey(cont.size() - 1);
	}
	
	//  Извлечение элемента из начала очереди
	void pop() noexcept {
		//  Заменяем начальный элемент последним
		cont[0] = std::move(cont[cont.size() - 1]);
		//  Последний тихо-мирно оставляем за бортом (его уже в начало перенесли)
		cont.pop_back();
		//  Внутри обязателен вызов update_index
		heapify(0);
	}
	
	//  Это в пояснениях не нуждается
	inline const T& top() const {
		return *(cont[0].data);
	}

	//  Считаем, что обновился индекс у элемента, и его надо переместить на нужное место
	void decreaseKey(size_type index) noexcept {
		while (index > 0 && cmp(cont[parent(index)], cont[index])) {
			std::swap(cont[index], cont[parent(index)]);
			//  И обновляем индексы подвинутых вершин
			cont[index].update_index(index);
			index = parent(index);
		}
		cont[index].update_index(index);
	}

	inline bool empty() const { return cont.empty(); }

	//  Отладочная печать - можно и выкинуть
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

//  Состояние вершины в алгоритме Дейкстры - неизвестно, открыта, закрыта
enum class vertexState {None, Opened, Finished};

//  Обёртка для вершин графа, использующаяся в алгоритме Дейкстры
//  Хранит все необходимые поля, а также привязана к узлам двоичной кучи
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

//  Адаптер для алгоритма Дейкстры
template<typename vertexType>
class Dijkstra {
private:
	
	//  Типы для индексов и весов
	using size_type = typename graphVertex<vertexType>::size_type;
	using weight_type = typename graphVertex<vertexType>::weight_type;
	
	//  Информация об адаптерах вершин хранится в 
	std::vector<graphVertex<vertexType>> nodes;
public:
	//  Конструктор - просто цепляется к существующему графу
	//  считаем, что в исходном графе индексация с 0, индексы соответствуют
	template<typename vertexIter>
	Dijkstra(vertexIter & begin, vertexIter & end) {
		for (auto it = begin; it != end; ++it)
			nodes.push_back(graphVertex<vertexType>(*it));
	}

	//  Конструктор - просто цепляется к существующему графу
	template<typename vertexCont>
	explicit Dijkstra(const vertexCont & cont) {
		for (auto it = cont.cbegin(); it != cont.cend(); ++it)
			nodes.push_back(graphVertex<vertexType>(*it));
	}

	//  Собственно, сам алгоритм Дейкстры - ищем последовательность индексов вершин, и стоимость пути
	std::pair<std::vector<size_type>, weight_type> calcPath(size_type startIndex, size_type finishIndex) {
		//  Сначала создаём очередь с приоритетом и пихаем туда стартовую вершину
		if (startIndex >= nodes.size() || finishIndex >= nodes.size())
			throw std::out_of_range("Wrong start or finish node index!");

		using queueElemType = queueElem<graphVertex<vertexType>>;

		ExtPriority_Queue <graphVertex<vertexType>, std::vector<queueElemType>, std::greater<queueElemType> > epq;
		//  Задаём стартовую вершинку, и в очередь её, родимую
		nodes[startIndex].parent = startIndex;
		nodes[startIndex].weight = 0;
		nodes[startIndex].state = vertexState::Opened;
		epq.push(nodes[startIndex]);

		//  Спорное решение - остановить алгоритм в случае, если вершины на выходе имеют оценку больше целевой -
		//    в таком случае целевую мы никогда не улучшим, и можно заканчивать
		weight_type record = std::numeric_limits<weight_type>::max();

		while (!epq.empty()) {
			//  Вытаскиваем очередную вершину из очереди (у неё на данный момент минимальная оценка)
			
			//  В обморок не падать!!!! top возвращает не ссылку на голову очереди, а элемент, на который
			//  ссылается голова очереди, так что начало очереди спокойно можно удалить, ссылка будет живой
			const graphVertex<vertexType> &current(epq.top());
			record = current.weight;  //  обновляем значение минимума отметок вершин в очереди
			epq.pop();

			//  Если текущая вершина целевая, или же у неё оценка больше либо равна оценки целевой, то выходим
			if (current.vertex->name == finishIndex || nodes[finishIndex].weight < record)
			{
				//  Строим путь по меткам родителей
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
			
			//  Маркируем текущую вершину как закрытую
			nodes[current.vertex->name].state = vertexState::Finished;

			//  Для всех связанных с текущей
			for (auto adjIt = current.vertex->cbegin(); adjIt != current.vertex->cend(); ++adjIt)
				if (nodes[adjIt->dest].state == vertexState::None) {
					//  Эту вершину ещё не открывали, её в любом случае в очередь добавляем
					nodes[adjIt->dest].state = vertexState::Opened;
					nodes[adjIt->dest].parent = current.vertex->name;
					nodes[adjIt->dest].weight = nodes[current.vertex->name].weight + adjIt->weight;
					epq.push(nodes[adjIt->dest]);
				}
				else {
					//  Уже открывали, пересчитываем - может быть, оценка уменьшится
					if (nodes[adjIt->dest].state != vertexState::Finished && nodes[adjIt->dest].weight > nodes[current.vertex->name].weight + adjIt->weight) {
						nodes[adjIt->dest].weight = nodes[current.vertex->name].weight + adjIt->weight;
						nodes[adjIt->dest].parent = current.vertex->name;
						epq.decreaseKey(nodes[adjIt->dest].index);
					}
				}
		}
		//  Если мы сюда попали, то всё плохо - пути не существует
		return make_pair(std::vector<size_type>(), 0);

	}
};
