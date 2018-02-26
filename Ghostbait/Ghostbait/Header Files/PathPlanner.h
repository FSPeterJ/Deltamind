#pragma once
#include "TraversalResult.h" //hate

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

namespace DirectX { struct XMFLOAT2; }

class HexPath;
class HexGrid;

typedef float(*HeuristicFunction)(HexTile*, HexTile*);


enum class TileType : unsigned char {
	Static  = 1 << 0,
	Dynamic = 1 << 1,
};

enum class PathingAlgorithm {
	BreadthFirst,
	Dijkstra,
	AStar,
	DStarLite,
	MTDStarLite,
	HPAStar,
	HAAStar,
};

class PathPlanner {
	static HeuristicFunction heuristicFunction;
	static HexGrid* grid;

	/// <summary>
	/// Chooses the algorithm to run based on the states of the start and goal tiles.
	/// </summary>
	/// <param name="start">The start tile's state.</param>
	/// <param name="goal">The goal tile's state.</param>
	/// <returns>The algorithm to use.</returns>
	static PathingAlgorithm ChooseAlgorithm(TileType startType, TileType goalType);

	/// <summary>
	/// Creates a path from specified tile to every/any other tile on the grid within steps steps and is limited by maxMovement.
	/// </summary>
	/// <param name="tile">The tile.</param>
	/// <param name="steps">The steps.</param>
	/// <param name="maxMovement">The maximum movement.</param>
	/// <returns>TraversalResult.</returns>
	static TraversalResult BreadthFirstTraverse(HexTile *const tile, size_t steps, size_t maxMovement);

	/// <summary>
	/// Creates a path from specified tile to every/any other tile on the grid within cost cost and is limited by maxMovement.
	/// </summary>
	/// <param name="tile">The tile.</param>
	/// <param name="cost">The cost.</param>
	/// <param name="maxMovement">The maximum movement.</param>
	/// <returns>TraversalResult.</returns>
	static TraversalResult DijkstraTraverse(HexTile *const tile, size_t cost, size_t maxMovement);

	/// <summary>
	/// Constructs a path from the start tile to the goal tile by searching every tile in the grid. Optimal in steps.
	/// </summary>
	/// <param name="start">The start tile.</param>
	/// <param name="goal">The goal tile.</param>
	/// <returns>A path from the start to goal or an empty path if one is not found.</returns>
	static HexPath BreadthFirstSearch(HexTile *const start, HexTile *const goal);

	/// <summary>
	/// Constructs a path from the start tile to the goal tile by searching tiles in the grid. Optimal in cost.
	/// </summary>
	/// <param name="start">The start tile.</param>
	/// <param name="goal">The goal tile.</param>
	/// <returns>A path from the start to goal or an empty path if one is not found.</returns>
	static HexPath DijkstraSearch(HexTile *const start, HexTile *const goal);

	/// <summary>
	/// Constructs a path from the start tile to the goal tile by searching tiles in the grid. Optimal in steps and cost given an admissible heuristic.
	/// </summary>
	/// <param name="start">The start tile.</param>
	/// <param name="goal">The goal tile.</param>
	/// <param name="Heuristic">The admissible heuristic.</param>
	/// <returns>A path from the start to goal or an empty path if one is not found.</returns>
	static HexPath AStarSearch(HexTile *const start, HexTile *const goal, HeuristicFunction Heuristic);

	//HexPath DStarSearch(HexTile *const start, HexTile *const goal){}

public:

	static void SetHeuristic(HeuristicFunction heuristic);
	static void SetGrid(HexGrid* _grid);

	static HexPath FindPath(HexTile*const start, HexTile*const goal, TileType startType, TileType goalType);

	static HexPath FindPath(const DirectX::XMFLOAT2 start, const DirectX::XMFLOAT2 goal, TileType startType, TileType goalType);

	template <PathingAlgorithm a>
	typename std::enable_if<a == PathingAlgorithm::BreadthFirst || a == PathingAlgorithm::Dijkstra, TraversalResult>::type
	static Traverse(HexTile*const start, std::size_t constraint, std::size_t maxSteps = 0);


	/// <summary>
	/// Calculates the path from start to goal within x steps. Returns empty path if path is invalid or too far.
	/// </summary>
	/// <param name="start">The start.</param>
	/// <param name="goal">The goal.</param>
	/// <param name="steps">The steps.</param>
	/// <returns>HexPath.</returns>
	HexPath CalculatePathWithinXSteps(HexTile *const start, HexTile *const goal, size_t steps);

	/// <summary>
	/// Calculates the path from start to goal within x cost. Returns empty path if path is invalid or too far.
	/// </summary>
	/// <param name="start">The start.</param>
	/// <param name="goal">The goal.</param>
	/// <param name="cost">The cost.</param>
	/// <returns>HexPath.</returns>
	HexPath CalculatePathWithinXCost(HexTile *const start, HexTile *const goal, size_t cost);

};
