#pragma once
#include "TraversalResult.h" //hate
#include <functional>
#include <queue>
#include <unordered_set>



enum SearchType {
	PredessorsOf,
	SuccessorsOf
};
enum UnionType {
	None,
	UnionSelf
};

namespace DirectX { struct XMFLOAT2; struct XMFLOAT4X4; }

class HexGrid;

//typedef HexagonTile<int> HexTile;
template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

typedef std::pair<float, float> FloatPair;
typedef std::function<void(HexTile* const)> NeighborFunction;

template<typename T, typename priority_t>
struct PriorityQueue {
	//we should profile and check if this is a bottle neck.
	//it uses a binary heap behind the scenes but may be worth it to switch to a deque if it is slow with large/complex paths
	typedef std::pair<priority_t, T> PQElement;
	std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

	inline bool empty() const { return elements.empty(); }

	inline void push(T item, priority_t priority) { elements.emplace(priority, item); }

	T top() {
		T top = elements.top().second;
		return top;
	}

	PQElement top_key() {
		PQElement top = elements.top();
		return top;
	}

	void pop() {
		elements.pop();
	}

	T pop_back() {
		T pop = elements.top().second;
		elements.pop();
		return pop;
	}
};

template<typename K, typename V>
struct PriorityQueueMap {
	typedef std::pair<K, V> pair;
	std::deque<pair> q;

	//this would ideally be a passed in template arg
	static bool Predicate(pair const& p1, pair const& p2) { return p1.second > p2.second; }

	bool empty() const { return q.empty(); }

	void clear() { q.clear(); }

	std::size_t size() const { return q.size(); }

	void insert(K const& key, V const& value) {
		pair node = std::make_pair(key, value);
		q.insert(std::upper_bound(q.begin(), q.end(), node, Predicate), node);
	}

	pair front() const { return q.back(); }

	void pop() { q.pop_back(); }

	typename std::deque<pair>::iterator find(K const& key) { return std::find_if(q.begin(), q.end(), [=](const pair& element) { return element.first == key; }); }

	typename std::deque<pair>::iterator remove(K const& key) { return q.erase(find(key)); }

	bool contains(K const& key) { return find(key) != q.end(); }

	void update(K const& key, V const& value) { remove(key); insert(key, value); }
};

struct TileInfo {
	float g, rhs, cost;
};

struct DStarCommon {
	friend class PathPlanner;

	HexGrid *const grid = nullptr;
	HexTile *start = nullptr, *goal = nullptr;
	PriorityQueueMap<HexTile*, FloatPair> open;

	//CostMap cumulativeCost; //g-value
	//CostMap rhs; //min of next cost
	//CostMap costChange; //pending cost changes
	std::unordered_map<HexTile*, TileInfo> knownTiles;
	std::unordered_set<HexTile*> changedTiles;
	float km;

	DStarCommon & operator=(const DStarCommon& other) { return *this; } //is this ever called? If so need to fix

	DStarCommon(HexTile*const _start, HexTile*const _goal, HexGrid*const _grid);

	float GetMinimumFrom(const SearchType neighbors, HexTile*const tile, std::function<float(HexTile*const)>, UnionType includeSelf = None);

	HexTile* GetMinimumTileFrom(const SearchType neighbors, HexTile*const tile, std::function<float(HexTile*const)>, UnionType includeSelf = None);

	void ForEachPredessor(HexTile*const tile, std::function<void(HexTile*const)> exec, UnionType includeSelf = None);

	void ForEachSuccessor(HexTile*const tile, std::function<void(HexTile*const)> exec, UnionType includeSelf = None);

	FloatPair CalculateKey(HexTile*const tile);

	float GetGVal(HexTile* tile);

	float GetRHS(HexTile* tile);

	float GetCost(HexTile* tile);

	void SetGVal(HexTile* tile, float value);

	void SetRHS(HexTile* tile, float value);

	void UpdateOpenList(HexTile*const tile);

	virtual void Update() = 0;
	//Update
	//Compute
};

class DStarLite : DStarCommon {
	friend class PathPlanner;

	std::size_t perceptionRange = 3;
	HexTile *last = nullptr, *curNode = nullptr, **nextTileInPath = nullptr;

	//void Replan() {
	//	path->clear();
	//
	//	curNode = start;
	//
	//	while(curNode != goal) {
	//		path->push_back(curNode);
	//
	//		mostMinimum = grid->BlockWeight();
	//		ForEachSuccessor(curNode, [=](HexTile*const sPrime) {
	//			float minposib = sPrime->weight + GetGVal(sPrime];
	//			if(minposib < mostMinimum) {
	//				mostMinimum = minposib;
	//				curNode = sPrime;
	//			}
	//		});
	//
	//		if(grid->IsBlocked(curNode)) {
	//			path->clear();
	//			return; //no path
	//		}
	//
	//	}
	//
	//	path->push_back(goal);
	//
	//}

	void ComputeShortestPath();

public:
	DStarLite(HexGrid *const _grid, HexTile *const _start, HexTile *const _goal, HexTile** _nextTileInPath);
	DStarLite & operator=(const DStarLite& other) { return *this; } //is this ever called? If so need to fix
	void Update();
};

class MTDStarLite : DStarCommon {
	friend class PathPlanner;

	HexPath* path = nullptr;
	std::vector<HexTile*> deleted;
	VisitedMap parent;
	DirectX::XMFLOAT4X4* goalReference = nullptr, *startReference = nullptr;

	void ForEachInSearchTreeButNotSubtreeRootedAt(HexTile*const tile, NeighborFunction exec);
	void BasicDeletion(HexTile*const oldStart);
	void ComputeCostMinimalPath();
	void OptimizedDelete();

public:
	MTDStarLite(HexGrid *const _grid, DirectX::XMFLOAT4X4* _startRef, DirectX::XMFLOAT4X4* _goalRef, HexPath* _path);
	MTDStarLite & operator=(const MTDStarLite& other) { return *this; } //is this ever called? If so, MUST FIX
	void Update();
};