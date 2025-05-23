#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "Tile.h"
#include "Obstacle.h"
#include "Charger.h"
#include "Floor.h"
#include "UnVisited.h"

enum class Direction {
	up,
	down,
	left,
	right,
	none
};

class Map {
private:
	size_t width;
	size_t height;
	size_t chargerId;
	std::vector<std::unique_ptr<Tile>> tiles;

public:
	Map() = default;
	Map(std::istream& in);
	Map(size_t mapWidth, size_t mapHeight, size_t chargerTileId);

	size_t getWidth() const noexcept { return width; }
	size_t getHeight() const noexcept { return height; }
	size_t getChargerId() const noexcept { return chargerId; }

	//TODO rule of five

	bool isMapValid() const;
	bool canMoveOn(size_t tileId) const;
	void loadMap(std::istream& in);
	void updateTile(size_t tileId, const Tile& tileObj);
	std::optional<size_t> getIndex(size_t position, Direction direction);

	friend std::ostream& operator<<(std::ostream& os, const Map& map);
};