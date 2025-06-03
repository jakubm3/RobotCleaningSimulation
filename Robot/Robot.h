#pragma once
#include <iostream>
#include <queue>
#include <utility>
#include <stack>
#include <tuple>
#include <string>
#include <sstream>
#include <limits>
#include "Map.h"

enum class RobotAction {
	move,
	clean,
	explore,
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

	Direction move();
	void cleanTile();
	bool createPath(size_t targetId);
	bool createPathUnvisited();
	bool createPathTrash();
	bool createPathToVisit();
	void clearMoveTargets();
public:
	Robot() = delete;
	Robot(std::istream& in);
	Robot(size_t mapWidth, size_t mapHeight, size_t chargerId);
	void setPosition(size_t newPosition);
	bool isRobotValid() const;
	bool isRobotStateValid() const;
	void setEfficiency(unsigned int efficiency);
	size_t getPosition() const noexcept { return position_; }
	size_t getChargerId() const noexcept { return chargerId_; }
	unsigned int getCleaningEfficiency() const noexcept { return cleaningEfficiency; }
	const Map& getMemoryMap() const noexcept { return map; }
	RobotAction getCurrTask() const noexcept { return currTask; }

	std::tuple<RobotAction, Direction> makeAction();
	void exploreTile(size_t tileId, const Tile* tileObj);

	void orderToCleanEfficiently();
	bool orderToGoHome();
	bool orderToMove(size_t id);
	bool orderToClean(size_t id, unsigned int radius);
	void resetMemory();

	void loadRobot(std::istream& in);
	void saveRobot(std::ostream& out) const;

	friend std::ostream& operator<<(std::ostream& os, const Robot& robot);
};