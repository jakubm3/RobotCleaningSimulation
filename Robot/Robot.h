#pragma once
#include <iostream>
#include <queue>
#include <tuple>
#include "Map.h"

enum class RobotAction {
	move,
	clean,
	explore
};

enum class Direction {
	up,
	down,
	left,
	right,
	none
};

class Robot {
private:
	size_t position_;
	size_t chargerId_;
	std::queue<size_t> path;
	Map map;
	RobotAction currTask;
	unsigned int cleaningEfficiency = 0;
	std::vector<bool> tilesToCheck;
public:
	Robot() = delete;
	Robot(size_t mapWidth, size_t mapHeight, size_t chargerId);
	bool setPosition(size_t newPosition);
	size_t getPosition() const noexcept { return position_; }
	size_t getChargerId() const noexcept { return chargerId_; }
	unsigned int getCleaningEfficiency() const noexcept { return cleaningEfficiency; }
	const Map& getMemoryMap() const noexcept { return map; }
	RobotAction getCurrTask() const noexcept { return currTask; }

	std::tuple<RobotAction, Direction> makeAction();
	Direction move();
	void cleanTile();
	void createPath(size_t targetId);
	bool findUnvisited(size_t& targetId) const;
	bool findTrash(size_t& targetId) const;

	void orderToGoHome();
	bool orderToMove(size_t id);
	bool orderToClean(size_t id, unsigned int radius);
	bool resetMemory();
	
	void loadRobot(std::istream& in);
	void saveRobot(std::ostream& out) const;
};