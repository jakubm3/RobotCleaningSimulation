#include "Robot.h"

Robot::Robot(size_t mapWidth, size_t mapHeight, size_t chargerId) {
	tilesToCheck.assign(mapWidth * mapHeight, false);
	// TODO
	// Load width x height map of unvisited tiles
	// Explore chargerId
	position_ = chargerId;
	chargerId_ = chargerId;
	currTask = RobotAction::explore;
}

bool Robot::setPosition(size_t newPosition) {
	// Only if not in exploring mode
	if (currTask == RobotAction::explore) {
		return false;
	}
	// Clears path
	while (!path.empty()) {
		path.pop();
	}

	// TODO
	// If placed on unvisited or invalid tile, exception
	position_ = newPosition;
	// Explore curr tile
	return true;
}

std::tuple<RobotAction, Direction> Robot::makeAction() {
	// Check current tile, clean if trash
	cleanTile();
	if (cleaningEfficiency > 0) {
		return std::make_tuple(RobotAction::clean, Direction::none);
	}

	// If has move target and in cleaning mode
	if (!path.empty() && currTask == RobotAction::clean) {
		// TODO
		// If next target has trash, ignore
		// Check tiles around, if has trash, go there
	}
	// If no move target and in explore mode
	else if (currTask == RobotAction::explore) {
		size_t nextTarget;
		if (findUnvisited(nextTarget)) {
			// Go towards next unvisited
			createPath(nextTarget);
		}
		else {
			// No unvisited, finished exploring
			currTask = RobotAction::clean;
		}
	}
	// If no move target and in moving mode
	else if (currTask == RobotAction::move) {
		currTask = RobotAction::clean;
	}

	// If no move target, go to nearest trash or home
	if (path.empty()) {
		size_t nextTarget;
		if (findTrash(nextTarget)) {
			// Found trash, go to trash
			createPath(nextTarget);
		}
		else {
			// No trash found, go home
			// TODO EXTRA
			// If in home, clean more efficiently
			currTask = RobotAction::move;
			createPath(chargerId_);
		}
	}

	return std::make_tuple(RobotAction::move, move());
}

Direction Robot::move() {
	// TODO
	return Direction::none;
}

void Robot::cleanTile() {
	// TODO
	// Check how dirty is tile, set efficiency
	// Drains energy = cleaningEfficiency
}

void Robot::createPath(size_t targetId) {
	// TODO
}

bool Robot::findUnvisited(size_t& targetId) const {
	// TODO
	return true;
}

bool Robot::findTrash(size_t& targetId) const {
	// TODO
	return true;
}

void Robot::orderToGoHome() {

}

bool Robot::orderToMove(size_t id) {
	// TODO
	return true;
}

bool Robot::orderToClean(size_t id, unsigned int radius) {
	// TODO
	return true;
}

bool Robot::resetMemory() {
	// TODO
	return true;
}

void Robot::loadRobot(std::istream& in) {
	// TODO
}
void Robot::saveRobot(std::ostream& out) const {
	// TODO
}