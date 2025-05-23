#include "Robot.h"

Direction Robot::move() {
	size_t nextTarget = path.front();
	path.pop();

	auto neighbourUp = map.getIndex(position_, Direction::up);
	auto neighbourDown= map.getIndex(position_, Direction::down);
	auto neighbourLeft = map.getIndex(position_, Direction::left);
	auto neighbourRight = map.getIndex(position_, Direction::right);

	if (neighbourUp.has_value() && neighbourUp == nextTarget) {
		position_ = nextTarget;
		return Direction::up;
	}
	else if (neighbourDown.has_value() && neighbourDown == nextTarget) {
		position_ = nextTarget;
		return Direction::down;
	}
	else if (neighbourLeft.has_value() && neighbourLeft == nextTarget) {
		position_ = nextTarget;
		return Direction::left;
	}
	else if (neighbourRight.has_value() && neighbourRight == nextTarget) {
		position_ = nextTarget;
		return Direction::right;
	}

	// Invalid move target
	throw std::runtime_error("Invalid move target");
}

void Robot::cleanTile() {
	Tile* tile = map.getTile(position_);

	if (auto floor = dynamic_cast<Floor*>(tile)) {
		setEfficiency(floor->getCleanliness());
	}
	else {
		setEfficiency(0u);
	}
	// TODO
	// Drains energy
}

bool Robot::createPath(size_t targetId) {
	std::vector<bool> visited(map.getSize(), false);
	std::vector<size_t> parent(map.getSize(), std::numeric_limits<size_t>::max());
	std::queue<size_t> q;

	visited[position_] = true;
	q.push(position_);
	while (!q.empty()) {
		size_t current = q.front();
		q.pop();
		if (current == targetId) {
			std::stack<size_t> tempStack;
			for (size_t v = targetId; v != -1; v = parent[v]) {
				tempStack.push(v);
			}
			while (!tempStack.empty()) {
				path.push(tempStack.top());
				tempStack.pop();
			}
			return true;
		}

		std::vector<std::optional<size_t>> neighbours;
		neighbours.push_back(map.getIndex(current, Direction::up));
		neighbours.push_back(map.getIndex(current, Direction::down));
		neighbours.push_back(map.getIndex(current, Direction::left));
		neighbours.push_back(map.getIndex(current, Direction::right));

		for (auto neighbour : neighbours) {
			if (neighbour.has_value()) {
				size_t idx = *neighbour;
				if (!visited[idx]) {
					visited[idx] = true;
					parent[idx] = current;
					q.push(idx);
				}
			}
		}
	}
	return false;
}

bool Robot::findUnvisited() {
	std::vector<bool> visited(map.getSize(), false);
	std::vector<size_t> parent(map.getSize(), std::numeric_limits<size_t>::max());
	std::queue<size_t> q;

	visited[position_] = true;
	q.push(position_);
	while (!q.empty()) {
		size_t current = q.front();
		Tile* tile = map.getTile(current);
		q.pop();

		if (auto unVisited = dynamic_cast<UnVisited*>(tile)) {
			std::stack<size_t> tempStack;
			for (size_t v = current; v != -1; v = parent[v]) {
				tempStack.push(v);
			}
			while (tempStack.size() != 1) {
				path.push(tempStack.top());
				tempStack.pop();
			}
			return true;
		}

		std::vector<std::optional<size_t>> neighbours;
		neighbours.push_back(map.getIndex(current, Direction::up));
		neighbours.push_back(map.getIndex(current, Direction::down));
		neighbours.push_back(map.getIndex(current, Direction::left));
		neighbours.push_back(map.getIndex(current, Direction::right));

		for (auto neighbour : neighbours) {
			if (neighbour.has_value()) {
				size_t idx = *neighbour;
				if (!visited[idx]) {
					visited[idx] = true;
					parent[idx] = current;
					q.push(idx);
				}
			}
		}
	}
	return false;
}

bool Robot::findTrash() {
	std::vector<bool> visited(map.getSize(), false);
	std::vector<size_t> parent(map.getSize(), std::numeric_limits<size_t>::max());
	std::queue<size_t> q;

	visited[position_] = true;
	q.push(position_);
	while (!q.empty()) {
		size_t current = q.front();
		Tile* tile = map.getTile(current);
		q.pop();
		
		auto floor = dynamic_cast<Floor*>(tile);
		if (floor && floor->isDirty()) {
			std::stack<size_t> tempStack;
			for (size_t v = current; v != -1; v = parent[v]) {
				tempStack.push(v);
			}
			while (tempStack.size() != 1) {
				path.push(tempStack.top());
				tempStack.pop();
			}
			return true;
		}

		std::vector<std::optional<size_t>> neighbours;
		neighbours.push_back(map.getIndex(current, Direction::up));
		neighbours.push_back(map.getIndex(current, Direction::down));
		neighbours.push_back(map.getIndex(current, Direction::left));
		neighbours.push_back(map.getIndex(current, Direction::right));

		for (auto neighbour : neighbours) {
			if (neighbour.has_value()) {
				size_t idx = *neighbour;
				if (!visited[idx]) {
					visited[idx] = true;
					parent[idx] = current;
					q.push(idx);
				}
			}
		}
	}
	return false;
}

Robot::Robot(std::istream& in) {
	loadRobot(in);
}

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

void Robot::setEfficiency(unsigned int efficiency) {
	efficiency = std::min(9u, efficiency);
	cleaningEfficiency = efficiency;
}

std::tuple<RobotAction, Direction> Robot::makeAction() {
	// If in invalid place or error, return error
	Tile* tile = map.getTile(position_);
	if (!tile->isMoveValid() || currTask == RobotAction::error) {
		currTask = RobotAction::error;
		return std::make_tuple(RobotAction::error, Direction::none);
	}

	// Clean if trash
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
		// Try to find next unvisited
		if (!findUnvisited()) {
			// Exploring complete, clean
			currTask = RobotAction::clean;
		}
	}
	// If no move target and in moving mode
	else if (currTask == RobotAction::move) {
		currTask = RobotAction::clean;
	}

	// If no move target, go to nearest trash or home
	if (path.empty()) {
		// Try to find trash
		if (!findTrash()) {
			// No trash found, go home
			// TODO EXTRA
			// If in home, clean more efficiently
			currTask = RobotAction::move;
			createPath(chargerId_);
		}
	}

	Direction direction;
	try {
		 direction = move();
	}
	catch (const std::runtime_error& e) {
		currTask = RobotAction::error;
		return std::make_tuple(RobotAction::error, Direction::none);
	}
	
	return std::make_tuple(RobotAction::move, direction);
}

void Robot::exploreTile(size_t tileId, const Tile* tileObj) {
	map.updateTile(tileId, tileObj);
}

void Robot::orderToGoHome() {
	while (!path.empty()) {
		path.pop();
	}
	currTask = RobotAction::move;
	createPath(chargerId_);
}

bool Robot::orderToMove(size_t id) {
	if (currTask == RobotAction::explore) { 
		return false; 
	}
	// TODO moving while not explored all
	while (!path.empty()) {
		path.pop();
	}
	createPath(id);
	currTask = RobotAction::move;
	return true;
}

bool Robot::orderToClean(size_t id, unsigned int radius) {
	if (currTask == RobotAction::explore) {
		return false;
	}
	// TODO
	return true;
}

bool Robot::resetMemory() {
	if (position_ != chargerId_) { 
		return false; 
	}

	map = Map(map.getWidth(), map.getHeight(), map.getChargerId());
	Charger* charger = new Charger();
	map.updateTile(map.getChargerId(), charger);
	delete charger;
	return true;
}

void Robot::loadRobot(std::istream& in) {
	// TODO
}
void Robot::saveRobot(std::ostream& out) const {
	// TODO
}

std::ostream& operator<<(std::ostream& os, const Robot& robot) {
	os << "Nothing yet XD\n";
	return os;
}