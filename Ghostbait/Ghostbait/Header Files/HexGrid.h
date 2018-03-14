#pragma once
#include <unordered_set>
#include "Delegate.h"
#include "Controlable.h"
#include "HexagonalGridLayout.h"
#include "HexagonTileSpecializer.h"
#include "HexTileEqualityCheck.h"

namespace DirectX { struct XMFLOAT2; }

struct HexagonalGridLayout;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

class HexRegion;
class HexPath;

class HexGrid {
	using GridContainer = std::unordered_set<HexTile*, std::hash<HexTile*>, EqualComparator>;

	float map_radius = 0;

	GridContainer map;
	GridContainer::iterator iter;

	Delegate<> DrawLine;
	Delegate<> DrawRange;
	Delegate<> DrawIntersect;
	Delegate<> DrawXStepsPath;
	Delegate<> DrawAIPath;
	
	HexagonalGridLayout layout;
	static HexRegion blocked;

	std::size_t curPos = 0;
	void SetUpDrawingPaths();

	void step();

	/// <summary>
	/// Gets the region cooresponding to the specified bounds. Can return tiles that are not part of the grid.
	/// If you only want tiles on the grid, use Filter().
	/// </summary>
	/// <param name="xmin">The xmin.</param>
	/// <param name="xmax">The xmax.</param>
	/// <param name="ymin">The ymin.</param>
	/// <param name="ymax">The ymax.</param>
	/// <param name="zmin">The zmin.</param>
	/// <param name="zmax">The zmax.</param>
	/// <returns>HexRegion.</returns>
	HexRegion GetRegion(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);
	
	static const float Blocked;
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="HexGrid"/> class with the desired radius layout.
	/// </summary>
	/// <param name="_radius">The radius.</param>
	/// <param name="_layout">The layout.</param>
	HexGrid(float _radius, const HexagonalGridLayout& _layout);
	
	~HexGrid();

	/// <summary>
	/// Converts a world coordinate into a grid coordinate.
	/// </summary>
	/// <param name="p">The world point to convert.</param>
	/// <returns>The hex tile where the world coordinate maps to. Returns null if no tile maps.</returns>
	HexTile* PointToTile(const DirectX::XMFLOAT2& p);

	/// <summary>
	/// Converts a HexTile into a world coordinate.
	/// </summary>
	/// <param name="tile">The tile.</param>
	/// <returns>The world coordinate center of the tile.</returns>
	DirectX::XMFLOAT2 TileToPoint(HexTile * tile);

	/// <summary>
	/// Gets a random unbiased tile on the grid.
	/// </summary>
	/// <returns>HexTile *.</returns>
	HexTile* GetRandomTile();

	/// <summary>
	/// Snaps the specified world coordinate to a grid coordinate.
	/// </summary>
	/// <param name="p">The world coordinate.</param>
	/// <param name="snapPoint">The out snap point.</param>
	/// <returns>true if the passed in point was able to map to a tile on the grid, otherwise false.</returns>
	bool Snap(const DirectX::XMFLOAT2& p, OUT DirectX::XMFLOAT2& snapPoint);

	/// <summary>
	/// Gets a region of tiles n steps away from the specified tile.
	/// </summary>
	/// <param name="tile">The tile.</param>
	/// <param name="n">The radius of steps.</param>
	/// <returns>HexRegion.</returns>
	HexRegion GetTilesNStepsAway(HexTile *const tile, int n);

	/// <summary>
	/// Sets the weight of the tile at tilePosition. If the tile does not map to a tile on the grid, nothing happens.
	/// </summary>
	/// <param name="tilePosition">The tile position.</param>
	/// <param name="weight">The weight.</param>
	/// <returns>true if the weight of the tile was set, otherwise false.</returns>
	bool SetWeight(const DirectX::XMFLOAT2& tilePosition, float weight);
	
	/// <summary>
	/// Sets the weight of tile. If the tile is not a tile on the grid, nothing happens.
	/// </summary>
	/// <param name="tile">The tile.</param>
	/// <param name="weight">The weight.</param>
	/// <returns>true if the weight of the tile was set, otherwise false.</returns>
	bool SetWeight(HexTile *const tile, float weight);

	/// <summary>
	/// Sets the tile at tilePosition as an obstacle. If the tile does not map to a tile on the grid, nothing happens.
	/// </summary>
	/// <param name="obstaclePosition">The obstacle position.</param>
	/// <returns>true if the weight of the tile was set, otherwise false.</returns>
	bool AddObstacle(const DirectX::XMFLOAT2& obstaclePosition);

	/// <summary>
	/// Removes the tile at tilePosition as an obstacle. If the tile does not map to a tile on the grid, nothing happens.
	/// </summary>
	/// <param name="obstaclePosition">The obstacle position.</param>
	/// <returns>true if the weight of the tile was set, otherwise false.</returns>
	bool RemoveObstacle(const DirectX::XMFLOAT2& obstaclePosition);

	/// <summary>
	/// Given two tiles and their radii, finds the tiles which intersect.
	/// </summary>
	/// <param name="a">The first tile.</param>
	/// <param name="aRadius">The first tile's radius.</param>
	/// <param name="b">The second tile.</param>
	/// <param name="bRadius">The second tile's radius.</param>
	/// <returns>a HexRegion containing the intersecting tiles.</returns>
	HexRegion GetIntersectingTilesRanges(HexTile *const a, int aRadius, HexTile *const b, int bRadius);

	/// <summary>
	/// Determines whether the specified tile is blocked. If tile is invalid, it returns true.
	/// </summary>
	/// <param name="tile">The tile.</param>
	/// <returns>true if blocked or invalid, otherwise false.</returns>
	static bool IsBlocked(HexTile* tile);

	/// <summary>
	/// Determines whether the specified position is blocked. If tile is invalid, it returns true.
	/// </summary>
	/// <param name="pos">The position.</param>
	/// <returns>true if blocked or invalid, otherwise false.</returns>
	bool IsBlocked(const DirectX::XMFLOAT2& pos);

	/// <summary>
	/// Determines whether the specified position is blocked. If tile is invalid, it returns true.
	/// </summary>
	/// <param name="x">The x.</param>
	/// <param name="z">The z.</param>
	/// <returns>true if blocked or invalid, otherwise false.</returns>
	bool IsBlocked(const float x, const float z);

	/// <summary>
	/// Only use this when you know the exact grid coordinate of the tile. Do not pass in world position coordinates.
	/// </summary>
	/// <param name="x">The x.</param>
	/// <param name="y">The y.</param>
	/// <returns>HexTile * if valid, otherwise null.</returns>
	HexTile* GetTileExact(const int x, const int y);

	/// <summary>
	/// Only use this when you know the exact grid coordinate of the tile. Do not pass in world position coordinates.
	/// </summary>
	/// <param name="t">The tile.</param>
	/// <returns>HexTile * if valid, otherwise null.</returns>
	HexTile* GetTileExact(HexTile& t) const;

	/// <summary>
	/// Populates this instance.
	/// </summary>
	void Fill(bool withRandomObstacles);

	/// <summary>
	/// Displays this instance in debug. It will only draw tiles within a radius of 3 from the specified position.
	/// </summary>
	/// <param name="player">The player.</param>
	void Display(DirectX::XMFLOAT2& player);

	HexGrid(const char* _filename, float _radius = 1.0f, HexagonalGridLayout _layout = HexagonalGridLayout::FlatLayout);

};
