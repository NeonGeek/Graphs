#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <tuple>
#include <string>
#include <iterator>
#include <stack>
#include <queue>
#include <map>
#include <algorithm>
#include <functional>
using namespace std;

vector<string> Split(string str, char delimiter) {
	vector<string> vec;
	string currentString = "";
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != delimiter)
			currentString.push_back(str[i]);
		else {
			if (currentString != "")
				vec.push_back(currentString);
			currentString = "";
		}
	}
	if (currentString != "")
		vec.push_back(currentString);
	return vec;
}

class DSU {
private:
	vector<int> parent;
	vector <int> rank;
	int size = 0;
public:
	void make_set(int v) {
		if (parent.size() < v) {
			for (int i = size; i < v; i++) {
				parent.push_back(0);
				rank.push_back(0);
			}
			size = v;
		}
		parent[v - 1] = v;
		rank[v - 1] = 0;
	}

	int find_set(int v) {
		if (v == parent[v - 1])
			return v;
		return parent[v - 1] = find_set(parent[v - 1]);
	}

	void union_sets(int a, int b) {
		a = find_set(a);
		b = find_set(b);
		if (a != b) {
			if (rank[a - 1] < rank[b - 1])
				swap(a, b);
			parent[b - 1] = a;
			if (rank[a - 1] == rank[b - 1])
				++rank[a - 1];
		}
	}
};

void QSort(vector<tuple<int, int, int>> &mas, int first, int last)
{
	int mid;
	tuple<int, int, int> count;
	int f = first, l = last;
	mid = get<2>(mas[(f + l) / 2]);
	do
	{
		while (get<2>(mas[f])<mid) f++;
		while (get<2>(mas[l])>mid) l--;
		if (f <= l)
		{
			count = mas[f];
			mas[f] = mas[l];
			mas[l] = count;
			f++;
			l--;
		}
	} while (f<l);
	if (first<l) QSort(mas, first, l);
	if (f<last) QSort(mas, f, last);
}

class GraphRepresentationType {
protected:
	int vertexNumber = 0;
	bool isWeighed;
	bool isOriented;
public:
	virtual void readGraph(istream & ist) = 0;
	virtual void addEdge(int from, int to, int weight = 1) = 0;
	virtual void removeEdge(int from, int to) = 0;
	virtual int changeEdge(int from, int to, int newWeight) = 0;
	virtual vector<vector<int>> transformToAdjMatrix() = 0;
	virtual vector<set<pair<int, int>>> transformToAdjList() = 0;
	virtual vector <tuple<int, int, int>> transformToListOfEdges() = 0;
	virtual void writeGraph(string fileName) = 0;
	virtual tuple<bool, bool, int> GetInfo() = 0;
};

class AdjMatrixGraph :public GraphRepresentationType {
private:
	vector<vector<int>> adjacencyMatrix;
public:

	AdjMatrixGraph() {

	}

	AdjMatrixGraph(int n) {
		vertexNumber = n;
		for (int i = 0; i < n; i++) {
			vector<int> vertex;
			for (int j = 0; j < n; j++)
				vertex.push_back(0);
			adjacencyMatrix.push_back(vertex);
		}
	}

	AdjMatrixGraph(vector<vector<int>> matrix, tuple<bool, bool, int> info) {
		adjacencyMatrix = matrix;
		isWeighed = get<0>(info);
		isOriented = get<1>(info);
		vertexNumber = get<2>(info);
	}

	void readGraph(istream & ist) override {
		ist >> vertexNumber;
		ist >> isOriented;
		ist >> isWeighed;
		for (int i = 0; i < vertexNumber; i++) {
			vector<int> row;
			for (int j = 0; j < vertexNumber; j++) {
				int cell;
				ist >> cell;
				row.push_back(cell);
			}
			adjacencyMatrix.push_back(row);
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		adjacencyMatrix[from - 1][to - 1] = weight;
		if (!isOriented)
			adjacencyMatrix[to - 1][from - 1] = weight;
	}

	void removeEdge(int from, int to) override {
		adjacencyMatrix[to - 1][from - 1] = 0;
		adjacencyMatrix[from - 1][to - 1] = 0;
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight = adjacencyMatrix[from - 1][to - 1];
		adjacencyMatrix[to - 1][from - 1] = newWeight;
		if (!isOriented)
			adjacencyMatrix[from - 1][to - 1] = newWeight;
		return weight;
	}

	vector<vector<int>> transformToAdjMatrix() override {
		return adjacencyMatrix;
	}

	vector<set<pair<int, int>>> transformToAdjList() override {
		vector<set<pair<int, int>>> adjList;
		for (int i = 0; i < vertexNumber; i++) {
			set<pair<int, int>> vertex;
			for (int j = 0; j < vertexNumber; j++) {
				if (adjacencyMatrix[i][j] > 0) {
					vertex.insert(pair<int, int>(j + 1, adjacencyMatrix[i][j]));
				}
			}
			adjList.push_back(vertex);
		}
		return adjList;
	}

	vector <tuple<int, int, int>> transformToListOfEdges() override {
		vector<tuple<int, int, int>> edgesList;
		for (int i = 0; i < vertexNumber; i++) {
			for (int j = 0; j < vertexNumber; j++) {
				if (adjacencyMatrix[i][j] > 0) {
					edgesList.push_back(tuple<int, int, int>(i + 1, j + 1, adjacencyMatrix[i][j]));
					if (!isOriented)
						adjacencyMatrix[j][i] = 0;
				}
			}
		}
		return edgesList;
	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		for (int i = 0; i < vertexNumber; i++) {
			for (int j = 0; j < vertexNumber; j++)
				file << adjacencyMatrix[i][j] << " ";
			file << endl;
		}
	}

	tuple<bool, bool, int> GetInfo() override {
		return tuple<bool, bool, int>(isWeighed, isOriented, vertexNumber);
	}
};

class AdjListGraph :public GraphRepresentationType {
private:
	vector<set<pair<int, int>>> adjList;
public:
	AdjListGraph() {

	}

	AdjListGraph(int n) {
		vertexNumber = n;
		for (int i = 0; i < n; i++) {
			set<pair<int, int>> vertex;
			adjList.push_back(vertex);
		}
	}

	AdjListGraph(vector<set<pair<int, int>>> list, tuple<bool, bool, int> info) {
		adjList = list;
		isWeighed = get<0>(info);
		isOriented = get<1>(info);
		vertexNumber = get<2>(info);
	}

	void readGraph(istream & ist) override {
		ist >> vertexNumber;
		ist >> isOriented;
		ist >> isWeighed;
		string currentVertexList;
		getline(ist, currentVertexList);

		if (isWeighed) {
			for (int i = 0; i < vertexNumber; i++) {
				getline(ist, currentVertexList);
				vector<string> neighbours = Split(currentVertexList, ' ');
				set<pair<int, int>> adjanceciesOfVertex;
				for (int j = 0; j < neighbours.size(); j += 2) {
					pair<int, int> adjancecy(stoi(neighbours[j]), stoi(neighbours[j + 1]));
					adjanceciesOfVertex.insert(adjancecy);
				}
				adjList.push_back(adjanceciesOfVertex);
			}
		}
		else {
			for (int i = 0; i < vertexNumber; i++) {
				getline(ist, currentVertexList);
				vector<string> neighbours = Split(currentVertexList, ' ');
				set<pair<int, int>> adjanceciesOfVertex;
				for (int j = 0; j < neighbours.size(); j++) {
					pair<int, int> adjancecy(stoi(neighbours[j]), 1);
					adjanceciesOfVertex.insert(adjancecy);
				}
				adjList.push_back(adjanceciesOfVertex);
			}
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		if (isWeighed) {
			adjList[from - 1].insert(pair<int, int>(to, weight));
			if (!isOriented)
				adjList[to - 1].insert(pair<int, int>(from, weight));
		}
		else {
			adjList[from - 1].insert(pair<int, int>(to, 1));
			if (!isOriented)
				adjList[to - 1].insert(pair<int, int>(from, 1));
		}
	}

	void removeEdge(int from, int to) override {
		for (auto iter = adjList[from - 1].begin(); iter != adjList[from - 1].end(); iter++)
			if ((iter->first) == to) {
				adjList[from - 1].erase(iter);
				break;
			}
		if (!isOriented) {
			for (auto iter = adjList[to - 1].begin(); iter != adjList[to - 1].end(); iter++)
				if ((iter->first) == from) {
					adjList[to - 1].erase(iter);
					break;
				}
		}
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight;
		for (auto iter = adjList[from - 1].begin(); iter != adjList[from - 1].end(); iter++)
			if ((iter->first) == to) {
				weight = iter->second;
				break;
			}
		this->removeEdge(from, to);
		this->addEdge(from, to, newWeight);
		if (!isOriented) {
			this->removeEdge(to, from);
			this->addEdge(to, from, newWeight);
		}
		return weight;
	}

	vector<vector<int>> transformToAdjMatrix() override {
		vector<vector<int>> adjMatrix;
		for (int i = 0; i < vertexNumber; i++) {
			vector<int> row;
			for (int j = 0; j < vertexNumber; j++)
				row.push_back(0);
			adjMatrix.push_back(row);
		}

		if (isWeighed) {
			for (int i = 0; i < vertexNumber; i++) {
				for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
					adjMatrix[i][iter->first - 1] = iter->second;
					if (!isOriented)
						adjMatrix[iter->first - 1][i] = iter->second;
					else
						adjMatrix[iter->first - 1][i] = -(iter->second);
				}
			}
		}
		else {
			for (int i = 0; i < vertexNumber; i++) {
				for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
					adjMatrix[i][iter->first - 1] = 1;
					if (!isOriented)
						adjMatrix[iter->first - 1][i] = 1;
					else
						adjMatrix[iter->first - 1][i] = -1;
				}
			}
		}
		return adjMatrix;
	}

	vector<set<pair<int, int>>> transformToAdjList() override {
		return adjList;
	}

	vector <tuple<int, int, int>> transformToListOfEdges() override {
		vector<tuple<int, int, int>> edgesList;
		vector<pair<int, int>> vec;
		for (int i = 0; i < vertexNumber; i++) {
			for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
				bool flag = true;
				for (int j = 0; j < vec.size(); j++)
					if ((vec[j].first == iter->first) && (vec[j].second == i + 1))
						flag = false;
				if (flag) {
					edgesList.push_back(tuple<int, int, int>(i + 1, iter->first, iter->second));
					vec.push_back(pair<int, int>(i + 1, iter->first));
				}
			}
		}
		return edgesList;
	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		file << "L " << vertexNumber << endl;
		file << isOriented << " " << isWeighed;
		for (int i = 0; i < vertexNumber; i++) {
			file << endl;
			for (auto iter = adjList[i].begin(); iter != adjList[i].end(); iter++) {
				//file << i + 1 << " ";
				if (iter != adjList[i].begin()) file << " ";
				file << iter->first;
				if (isWeighed)
					file << " " << iter->second;
			}
		}
		file.close();
	}

	tuple<bool, bool, int> GetInfo() override {
		return tuple<bool, bool, int>(isWeighed, isOriented, vertexNumber);
	}

	vector<tuple<int, int, int>> getSpaingTreePrima() {
		vector<tuple<int, int, int>> spaingTree;
		priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
		vector<int> distances(vertexNumber, INT32_MAX);
		vector<int> parent(vertexNumber, -1);
		vector<bool> isMarked(vertexNumber, false);

		pq.push(make_pair(0, 1));
		distances[0] = 0;

		while (!pq.empty()) {
			int u = pq.top().second - 1;
			pq.pop();
			isMarked[u] = true;
			for (auto it = adjList[u].begin(); it != adjList[u].end(); ++it) {
				int v = it->first - 1;
				int weight = it->second;
				if (isMarked[v] == false && distances[v] > weight) {
					distances[v] = weight;
					pq.push(make_pair(distances[v], v + 1));
					parent[v] = u;
				}
			}
		}

		for (int i = 1; i < parent.size(); ++i)
			spaingTree.push_back(tuple<int, int, int>(parent[i] + 1, i + 1, distances[i]));
		return spaingTree;
	}

	bool checkEulerCircle() {
		for (int i = 0; i < vertexNumber; i++) {
			if (adjList[i].size() % 2 == 1)
				return false;
		}
		return true;
	}

	int checkEuler(bool circleExist) {
		int firstVertex = 1;
		if (!circleExist) {
			int numberOfOddVertexes = 0;
			for (int i = 0; i < vertexNumber; i++) {
				if (adjList[i].size() % 2 == 1) {
					firstVertex = i + 1;
					numberOfOddVertexes++;
				}
			}
			if (numberOfOddVertexes > 2)
				return 0;
		}
		return firstVertex;
	}

	vector<int> getEuleranTourEffective() {
		vector<set<pair<int, int>>> list = adjList;
		vector<int> tour;
		stack<int> s;
		int firstVertex = checkEuler(checkEulerCircle());
		if (firstVertex == 0) return tour;
		s.push(firstVertex);

		while (!s.empty()) {
			int w = s.top();
			int first = -1; int second = -1;
			if (list[w - 1].size() > 0) {
				for (auto iter = list[w - 1].begin(); iter != list[w - 1].end(); iter++) {
					if (list[iter->first - 1].size() > 0) {
						s.push(iter->first);
						first = w;
						second = iter->first;
						list[w - 1].erase(iter);
						break;
					}
				}
			}

			if (second != -1) {
				for (auto iter = list[second - 1].begin(); iter != list[second - 1].end(); iter++) {
					if (iter->first == first) {
						list[second - 1].erase(iter);
						break;
					}
				}
			}

			if (w == s.top()) {
				tour.push_back(w);
				s.pop();
			}
		}
		return tour;
	}

	int getEdgesNumber() {
		int num = 0;
		for (int i = 0; i < adjList.size(); i++) {
			num += adjList[i].size();
		}
		return num;
	}

	bool isBridge(int from, int to, int weigth) {
		removeEdge(from + 1, to + 1);
		vector<bool> marks;
		for (int i = 0; i < vertexNumber; i++)
			marks.push_back(false);
		queue<int> q;
		marks[from] = true;
		q.push(from);
		while (!q.empty()) {
			int u = q.front();
			q.pop();
			for (auto iter = adjList[u].begin(); iter != adjList[u].end(); iter++) {
				if (iter->first - 1 == to) {
					return false;
					addEdge(from + 1, to + 1, weigth);
				}
				if (!marks[iter->first - 1]) {
					marks[iter->first - 1] = true;
					q.push(iter->first - 1);
				}
			}
		}
		addEdge(from + 1, to + 1, weigth);
		return true;
	}

	vector<int> getEuleranTourFleri() {
		vector<int> tour;
		int v = checkEuler(checkEulerCircle());
		if (v == 0) return tour;
		tour.push_back(v);
		int edgesNumber = getEdgesNumber() / 2;

		while (edgesNumber > 0) {
			int count = 0;
			set<pair<int, int>>::iterator iter = adjList[v - 1].begin();
			while (iter != adjList[v - 1].end()) {
				int u = iter->first - 1;
				int weigth = iter->second;
				count++;
				if ((!isBridge(v - 1, u, weigth)) || (adjList[v - 1].size() == 1)) {
					edgesNumber--;
					tour.push_back(u + 1);
					removeEdge(v, u + 1);
					v = u + 1;
					break;
				}
				else {
					iter = adjList[v - 1].begin();
					for (int i = 0; i < count; i++)
						iter++;
				}
			}
		}
		return tour;
	}

	int checkBipart() {
		vector<int> marks;
		for (int i = 0; i < adjList.size(); i++)
			marks.push_back(0);
		queue<int> q;
		q.push(0);
		marks[0] = 1;
		int length = 1;
		int current_mark = 1;
		while (!q.empty()) {
			int node = q.front();
			q.pop();
			current_mark *= -1;
			for (auto iter = adjList[node].begin(); iter != adjList[node].end(); iter++) {
				if (marks[iter->first - 1] == 0) {
					marks[iter->first - 1] = current_mark;
					q.push(iter->first - 1);
					length++;
				}
				else {
					if (marks[iter->first - 1] == current_mark) {
						continue;
					}
					else
						return 0;
				}
				if (length == vertexNumber)
					return length;
			}
		}
		if (length != vertexNumber)
			return 0;
		return length;
	}

	bool TryKun(int v, vector<bool> &used, vector<int> &vec) {
		if (used[v])  return false;
		used[v] = true;
		for (auto iter = adjList[v].begin(); iter != adjList[v].end(); iter++) {
			int to = iter->first - 1;
			if (vec[to] == -1 || TryKun(vec[to], used, vec)) {
				vec[to] = v;
				return true;
			}
		}
		return false;
	}

	vector<pair<int, int> > getMaximumMatchingBipart() {
		vector<pair<int, int>> bipart;
		vector<int> vec;
		vector<bool> used;
		for (int i = 0; i < vertexNumber; i++) {
			used.push_back(false);
			vec.push_back(-1);
		}
		for (int i = 0; i < vertexNumber; i++)
			TryKun(i, used, vec);

		for (int i = 0; i < vertexNumber; i++) {
			if (used[i]) {
				bipart.push_back(pair<int, int>(i + 1, vec[i] + 1));
				used[i] = false;
				used[vec[i]] = false;
			}
		}
		return bipart;
	}
};

class ListOfEdgesGraph :public GraphRepresentationType {
private:
	int edgesNumber;
	vector <tuple<int, int, int>> edgesList;
public:
	ListOfEdgesGraph() {

	}

	ListOfEdgesGraph(int n) {
		vertexNumber = n;
	}

	ListOfEdgesGraph(vector <tuple<int, int, int>> list, tuple<bool, bool, int> info) {
		edgesList = list;
		isWeighed = get<0>(info);
		isOriented = get<1>(info);
		vertexNumber = get<2>(info);
		edgesNumber = list.size();
	}

	void readGraph(istream & ist) override {
		ist >> vertexNumber;
		ist >> edgesNumber;
		ist >> isOriented;
		ist >> isWeighed;
		string currentEdge;
		getline(ist, currentEdge);
		if (isWeighed) {
			for (int i = 0; i < edgesNumber; i++) {
				getline(ist, currentEdge);
				vector<string> vertexes = Split(currentEdge, ' ');
				edgesList.push_back(tuple<int, int, int>(stoi(vertexes[0]), stoi(vertexes[1]), stoi(vertexes[2])));
			}
		}
		else {
			for (int i = 0; i < edgesNumber; i++) {
				getline(ist, currentEdge);
				vector<string> vertexes = Split(currentEdge, ' ');
				edgesList.push_back(tuple<int, int, int>(stoi(vertexes[0]), stoi(vertexes[1]), 1));
			}
		}
	}

	void addEdge(int from, int to, int weight = 1) override {
		if (isWeighed) {
			edgesList.push_back(tuple<int, int, int>(from, to, weight));
		}
		else {
			edgesList.push_back(tuple<int, int, int>(from, to, 1));
		}
		edgesNumber++;
	}

	void removeEdge(int from, int to) override {
		for (int i = 0; i < edgesNumber; i++) {
			if ((get<0>(edgesList[i]) == from) && (get<1>(edgesList[i]) == to)) {
				edgesList.erase(edgesList.begin() + i);
				break;
			}
		}
		edgesNumber--;
	}

	int changeEdge(int from, int to, int newWeight) override {
		int weight;
		for (int i = 0; i < edgesNumber; i++) {
			if ((get<0>(edgesList[i]) == from) && (get<1>(edgesList[i]) == to)) {
				weight = get<2>(edgesList[i]);
				break;
			}
		}
		this->removeEdge(from, to);
		this->addEdge(from, to, newWeight);
		return weight;
	}

	vector<vector<int>> transformToAdjMatrix() override {
		vector<vector<int>> adjMatrix;
		for (int i = 0; i < vertexNumber; i++) {
			vector<int> row;
			for (int j = 0; j < vertexNumber; j++)
				row.push_back(0);
			adjMatrix.push_back(row);
		}

		for (int i = 0; i < edgesNumber; i++) {
			adjMatrix[get<0>(edgesList[i]) - 1][get<1>(edgesList[i]) - 1] = get<2>(edgesList[i]);
			if (!isOriented)
				adjMatrix[get<1>(edgesList[i]) - 1][get<0>(edgesList[i]) - 1] = get<2>(edgesList[i]);
			else
				adjMatrix[get<1>(edgesList[i]) - 1][get<0>(edgesList[i]) - 1] = -get<2>(edgesList[i]);
		}
		return adjMatrix;
	}

	vector<set<pair<int, int>>> transformToAdjList() override {
		vector<set<pair<int, int>>> adjList;
		for (int i = 0; i < vertexNumber; i++) {
			set<pair<int, int>> vertex;
			adjList.push_back(vertex);
		}

		for (int i = 0; i < edgesNumber; i++) {
			adjList[get<0>(edgesList[i]) - 1].insert(pair<int, int>(get<1>(edgesList[i]), get<2>(edgesList[i])));
			if (!isOriented)
				adjList[get<1>(edgesList[i]) - 1].insert(pair<int, int>(get<0>(edgesList[i]), get<2>(edgesList[i])));
		}
		return adjList;
	}

	vector <tuple<int, int, int>> transformToListOfEdges() override {
		return edgesList;
	}

	void writeGraph(string fileName) override {
		ofstream file(fileName);
		file << "E " << vertexNumber << " " << edgesList.size() << endl;
		file << isOriented << " " << isWeighed << endl;
		for (int i = 0; i < edgesNumber; i++) {
			file << get<0>(edgesList[i]) << " " << get<1>(edgesList[i]);
			if (isWeighed)
				file << " " << get<2>(edgesList[i]);
			file << endl;
		}
		file.close();
	}

	tuple<bool, bool, int> GetInfo() override {
		return tuple<bool, bool, int>(isWeighed, isOriented, vertexNumber);
	}

	vector <tuple<int, int, int>> getSpaingTreeKruscal() {
		vector <tuple<int, int, int>> spaingTree;
		QSort(edgesList, 0, edgesList.size() - 1);

		DSU dsu;
		for (int i = 0; i < vertexNumber; i++)
			dsu.make_set(i + 1);
		for (int queue = 0; queue < edgesNumber; queue++) {
			tuple<int, int, int> edge = edgesList[queue];
			if (dsu.find_set(get<0>(edge)) != dsu.find_set(get<1>(edge))) {
				dsu.union_sets(dsu.find_set(get<0>(edge)), dsu.find_set(get<1>(edge)));
				spaingTree.push_back(edge);
			}
		}
		return spaingTree;
	}

	vector <tuple<int, int, int>> getSpaingTreeBoruvka() {
		vector <tuple<int, int, int>> spaingTree;
		DSU dsu;
		for (int i = 0; i < vertexNumber; i++)
			dsu.make_set(i + 1);

		while (spaingTree.size() < vertexNumber - 1) {
			auto minEdges = map<int, int>();
			for (int i = 0; i < vertexNumber; ++i)
				minEdges[i] = -1;
			for (int i = 0; i < edgesList.size(); ++i)
			{
				auto edge = edgesList[i];
				int from = get<0>(edge);
				int to = get<1>(edge);
				int weight = get<2>(edge);
				int fromComponent = dsu.find_set(from);
				int toComponent = dsu.find_set(to);
				if (fromComponent != toComponent) {
					if (minEdges[fromComponent] == -1 || get<2>(edgesList[minEdges[fromComponent]]) > weight)
						minEdges[fromComponent] = i;
					if (minEdges[toComponent] == -1 || get<2>(edgesList[minEdges[toComponent]]) > weight)
						minEdges[toComponent] = i;
				}
			}
			for (int i = 0; i < minEdges.size(); i++) {
				if (minEdges[i] != -1) {
					auto edge = edgesList[minEdges[i]];
					dsu.union_sets(get<0>(edge), get<1>(edge));
					spaingTree.push_back(tuple<int, int, int>(get<0>(edge), get<1>(edge), get<2>(edge)));
				}
			}
		}
		return spaingTree;
	}
};

class Graph {
private:
	GraphRepresentationType* representation = nullptr;
public:
	Graph() {

	}

	Graph(int n) {
		representation = new AdjListGraph(n);
	}

	void readGraph(string fileName) {
		ifstream file(fileName);
		char repr;
		file >> repr;
		switch (repr)
		{
		case 'C':	representation = new AdjMatrixGraph(); break;
		case 'L':	representation = new AdjListGraph(); break;
		case 'E':	representation = new ListOfEdgesGraph(); break;
		}
		representation->readGraph(file);
		file.close();
	}

	void addEdge(int from, int to, int weight = 1) {
		representation->addEdge(from, to, weight);
	}

	void removeEdge(int from, int to) {
		representation->removeEdge(from, to);
	}

	int changeEdge(int from, int to, int newWeight) {
		return representation->changeEdge(from, to, newWeight);
	}

	void transformToAdjMatrix() {
		representation = new AdjMatrixGraph(representation->transformToAdjMatrix(), representation->GetInfo());
	}

	void transformToAdjList() {
		representation = new AdjListGraph(representation->transformToAdjList(), representation->GetInfo());
	}

	void transformToListOfEdges() {
		representation = new ListOfEdgesGraph(representation->transformToListOfEdges(), representation->GetInfo());
	}

	void writeGraph(string fileName) {
		representation->writeGraph(fileName);
	}

	Graph getSpaingTreePrima() {
		this->transformToAdjList();
		vector<tuple<int, int, int>> minimalSpanningTree = reinterpret_cast<AdjListGraph*>(representation)->getSpaingTreePrima();
		Graph* spanningTree = new Graph();
		spanningTree->representation = new ListOfEdgesGraph(minimalSpanningTree, representation->GetInfo());
		return *spanningTree;
	}

	Graph getSpaingTreeKruscal() {
		this->transformToListOfEdges();
		vector <tuple<int, int, int>> minimalSpanningTree = reinterpret_cast<ListOfEdgesGraph*>(representation)->getSpaingTreeKruscal();
		Graph* spaingTree = new Graph();
		spaingTree->representation = new ListOfEdgesGraph(minimalSpanningTree, representation->GetInfo());
		return *spaingTree;
	}

	Graph getSpaingTreeBoruvka() {
		this->transformToListOfEdges();
		vector <tuple<int, int, int>> minimalSpanningTree = reinterpret_cast<ListOfEdgesGraph*>(representation)->getSpaingTreeKruscal();
		Graph* spaingTree = new Graph();
		spaingTree->representation = new ListOfEdgesGraph(minimalSpanningTree, representation->GetInfo());
		return *spaingTree;
	}

	bool checkEulerCircle() {
		this->transformToAdjList();
		return reinterpret_cast<AdjListGraph*>(representation)->checkEulerCircle();
	}

	int checkEuler(bool circleExist) {
		this->transformToAdjList();
		circleExist = reinterpret_cast<AdjListGraph*>(representation)->checkEulerCircle();
		return reinterpret_cast<AdjListGraph*>(representation)->checkEuler(circleExist);
	}

	vector<int> getEuleranTourFleri() {
		Graph *graph = new Graph();
		graph->representation = new AdjListGraph(representation->transformToAdjList(), representation->GetInfo());
		return reinterpret_cast<AdjListGraph*>(graph->representation)->getEuleranTourFleri();
	}

	vector<int> getEuleranTourEffective() {
		this->transformToAdjList();
		return reinterpret_cast<AdjListGraph*>(representation)->getEuleranTourEffective();
	}

	int checkBipart(vector<char> &marks) {
		this->transformToAdjList();
		return reinterpret_cast<AdjListGraph*>(representation)->checkBipart();
	}

	vector<pair<int, int> > getMaximumMatchingBipart() {
		this->transformToAdjList();
		return reinterpret_cast<AdjListGraph*>(representation)->getMaximumMatchingBipart();
	}

};

int main()
{
	return 0;
}
