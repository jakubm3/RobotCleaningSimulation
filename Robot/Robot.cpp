#include "Robot.h"

Direction Robot::move() {
	if (path.empty()) {
		return Direction::none;
	}
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

	// Can't reach target, calculate new route
	size_t targetId = path.back();
	while (!path.empty()) {
		path.pop();
	}

	if (createPath(targetId)) {
		// Try new route
		return move();
	}

	// Invalid move target
	clearMoveTargets();
	throw std::runtime_error("Can't reach tile ID: " + std::to_string(nextTarget) + "\n");
}

void Robot::cleanTile() {
	Tile* tile = map.getTile(position_);

	if (auto floor = dynamic_cast<Floor*>(tile)) {
		setEfficiency(floor->getCleanliness());
	}
	else {
		setEfficiency(0u);
	}
}

void Robot::clearMoveTargets() {
	while (!path.empty()) {
		path.pop();
	}
	tilesToCheck.assign(map.getSize(), false);
}

bool Robot::createPath(size_t targetId) {
	while (!path.empty()) {
		path.pop();
	}
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
			tempStack.pop();
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
			if (neighbour.has_value() && map.canMoveOn(*neighbour)) {
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
	while (!path.empty()) {
		path.pop();
	}

	std::vector<bool> visited(map.getSize(), false);
	std::vector<size_t> parent(map.getSize(), std::numeric_limits<size_t>::max());
	std::queue<size_t> q;

	visited[position_] = true;
	q.push(position_);
	while (!q.empty()) {
		size_t current = q.front();
		const Tile* tile = map.getTile(current);
		q.pop();
		if (const auto unVisited = dynamic_cast<const UnVisited*>(tile)) {
			std::stack<size_t> tempStack;
			for (size_t v = current; v != -1; v = parent[v]) {
				tempStack.push(v);
			}
			tempStack.pop();
			while (tempStack.size() > 1) {
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
			if (neighbour.has_value() && map.canMoveOn(*neighbour)) {
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
	while (!path.empty()) {
		path.pop();
	}

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
			tempStack.pop();
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
			if (neighbour.has_value() && map.canMoveOn(*neighbour)) {
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
	map = Map(mapWidth, mapHeight, chargerId);
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
	clearMoveTargets();

	// TODO
	// If placed on unvisited or invalid tile, exception
	position_ = newPosition;
	// Explore curr tile
	return true;
}

bool Robot::isRobotValid() const {
	if (!map.isMapValid()) {
		return false;
	}
	return true;
}

void Robot::setEfficiency(unsigned int efficiency) {
	efficiency = std::min(9u, efficiency);
	cleaningEfficiency = efficiency;
}

std::tuple<RobotAction, Direction> Robot::makeAction() {
	// If in invalid place throw error
	Tile* tile = map.getTile(position_);
	if (!tile->isMoveValid()) {
		clearMoveTargets();
		throw std::runtime_error("Robot is on invalid tile\n");
	}

	// Clean if trash
	cleanTile();
	if (cleaningEfficiency > 0) {
		return std::make_tuple(RobotAction::clean, Direction::none);
	}

	if (currTask == RobotAction::explore) {		// Exploring mode
		if (path.empty() && findUnvisited()) {
			// Succesfully found new tile to explore
			return std::make_tuple(RobotAction::move, move());
		}
		else if (path.empty()){
			// All possible tiles explored
			currTask = RobotAction::clean;
			return makeAction();
		}
		else {
			// Move towards current target
			return std::make_tuple(RobotAction::move, move());
		}
	}
	else if (currTask == RobotAction::move) {	// Moving mode
		if (path.empty()) {	//TODO check if tiles to visit?
			currTask = RobotAction::clean;
			return makeAction();
		}
		else {
			return std::make_tuple(RobotAction::move, move());
		}
	}
	else if (currTask == RobotAction::clean) {	// Cleaning mode
		// Check if neighbours need cleaning
		std::vector<std::optional<size_t>> neighbours;
		neighbours.push_back(map.getIndex(position_, Direction::up));
		neighbours.push_back(map.getIndex(position_, Direction::down));
		neighbours.push_back(map.getIndex(position_, Direction::left));
		neighbours.push_back(map.getIndex(position_, Direction::right));

		for (auto neighbour : neighbours) {
			if (neighbour.has_value() && map.getTile(*neighbour)){
				if (auto floor = dynamic_cast<Floor*>(map.getTile(*neighbour))) {
					if (floor->isDirty()) {
						// Neighbour is dirty, go there
						createPath(*neighbour);
						return std::make_tuple(RobotAction::move, move());
					}
				}
			}
		}

		// Neighbours are clean
		if (!path.empty()) {
			// Go towards current target
			return std::make_tuple(RobotAction::move, move());
		}
		else if (findTrash()){
			// Found trash in memory, go clean it
			return std::make_tuple(RobotAction::move, move());
		}
		else if (false) {
			// TODO
			// Go to nearest tile to visit
		}
		else if (createPath(chargerId_)){
			// Return to charger
			if (position_ == chargerId_) {
				// Already in charger
				if (findUnvisited()) {
					// Explore
					currTask = RobotAction::explore;
					return std::make_tuple(RobotAction::move, move());
				}
				else {
					// Nothing to explore
					// TODO
					// ask to continue?
					// clean efficiently
					std::cout << "finished\n";
				}
			}
			return std::make_tuple(RobotAction::move, move());
		}
		else if (findUnvisited()) {
			// Can't reach charger, explore tiles
			currTask = RobotAction::explore;
			return std::make_tuple(RobotAction::move, move());
		}
		else {
			// Can't reach charger and nothing to explore, error
			throw std::runtime_error("Robot can't reach charger\n");
		}
	}

	throw std::runtime_error("Unknown mode\n");
}

void Robot::exploreTile(size_t tileId, const Tile* tileObj) {
	map.updateTile(tileId, tileObj);
}

bool Robot::orderToGoHome() {
	clearMoveTargets();
	if (!createPath(chargerId_)) {
		return false;
	}
	currTask = RobotAction::move;
	return true;
}

bool Robot::orderToMove(size_t id) {
	clearMoveTargets();
	if (!createPath(id)) {
		return false;
	}
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

void Robot::resetMemory() {
	clearMoveTargets();
	map = Map(map.getWidth(), map.getHeight(), map.getChargerId());
	Charger* charger = new Charger();
	map.updateTile(map.getChargerId(), charger);
	delete charger;
}

void Robot::loadRobot(std::istream& in) {
	std::stringstream mapStream;
	std::string line;

	while (std::getline(in, line)) {
		bool isEmptyRow = true;
		for (char c : line) {
			if (c != '0' && c != ' ' && c != '\t') {
				isEmptyRow = false;
				break;
			}
		}

		if (isEmptyRow) {
			break;
		}

		mapStream << line << "\n";
	}

	map.loadMap(mapStream);

	in >> position_ >> chargerId_;
	int currTaskInt;
	in >> currTaskInt;
	currTask = static_cast<RobotAction>(currTaskInt);
	in >> cleaningEfficiency;
	size_t tilesSize;
	in >> tilesSize;
	tilesToCheck.resize(tilesSize);
	for (size_t i = 0; i < tilesSize; ++i) {
		bool val;
		in >> val;
		tilesToCheck[i] = val;
	}
	size_t pathSize;
	in >> pathSize;
	std::queue<size_t> tempQueue;
	for (size_t i = 0; i < pathSize; ++i) {
		size_t elem;
		in >> elem;
		tempQueue.push(elem);
	}
	path = std::move(tempQueue);
}


void Robot::saveRobot(std::ostream& out) const {
	map.saveMap(out);
	out << "\n";
	out << position_ << ' ' << chargerId_ << ' ';
	out << static_cast<int>(currTask) << ' ';
	out << cleaningEfficiency << ' ';
	out << tilesToCheck.size() << ' ';
	for (bool b : tilesToCheck) {
		out << b << ' ';
	}
	out << path.size() << ' ';
	std::queue<size_t> tempQueue = path;
	while (!tempQueue.empty()) {
		out << tempQueue.front() << ' ';
		tempQueue.pop();
	}
}

void replaceCharAtIndex(std::string& mapStr, size_t index1D, char newChar, size_t width) {
	size_t x = index1D % width;
	size_t y = index1D / width;
	size_t indexInString = y * (width + 1) + x;

	if (indexInString < mapStr.size() && mapStr[indexInString] != '\n') {
		mapStr[indexInString] = newChar;
	}
}

std::ostream& operator<<(std::ostream& os, const Robot& robot) {
	os << "\n";
	std::ostringstream oss;
	oss << robot.map;
	std::string mapStr = oss.str();
	replaceCharAtIndex(mapStr, robot.position_, 'R', robot.map.getWidth());
	os << mapStr;
	os << "\n";
	os << "Current objective: ";
	switch (robot.currTask)
	{
	case RobotAction::move:
		os << "moving\n";
		break;
	case RobotAction::explore:
		os << "exploring\n";
		break;
	case RobotAction::clean:
		os << "cleaning\n";
		break;
	default:
		os << "unknown\n";
		break;
	}
	if (!robot.path.empty()) {
		os << "Current move target ID: " << robot.path.front() << "\n";
		os << "Final destination move target ID: " << robot.path.back() << "\n";
	}
	return os;
}