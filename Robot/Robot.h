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
public:
	Robot() = delete;
	Robot(size_t mapWidth, size_t mapHeight, size_t chargerId) noexcept;
	Direction move();
	void createPath(size_t targetId);
	void cleanTile() noexcept;
	std::tuple<RobotAction, Direction> makeAction();
	bool findUnvisited(size_t& targetId) const;
	bool findTrash(size_t& targetId) const;
	void loadRobot(std::istream& in);
	void saveRobot(std::ostream& out) const;
};