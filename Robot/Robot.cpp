#include "Robot.h"

Direction Robot::move() {
	if (path.empty()) {
		return Direction::none;
	}
	size_t nextTarget = path.front();
	path.pop();

	for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right}) {
		auto neighbour = map.getIndex(position_, dir);
		if (neighbour.has_value() && neighbour == nextTarget && map.canMoveOn(nextTarget)) {
			position_ = nextTarget;
			return dir;
		}
		else if (neighbour == nextTarget) {
			// Calculate new route
			if (createPath(path.back())) {
				return move();
			}
			else {
				break;
			}
		}
	}

	// Can't reach move target
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

bool Robot::createPathUnvisited() {
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

bool Robot::createPathTrash() {
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

bool Robot::createPathToVisit() {
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

		if (tilesToCheck[current]) {
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
	map = Map(mapWidth, mapHeight, chargerId);
	tilesToCheck.assign(map.getSize(), false);
	position_ = chargerId;
	chargerId_ = chargerId;
	currTask = RobotAction::explore;
}

void Robot::setPosition(size_t newPosition) {
	clearMoveTargets();
	position_ = newPosition;
	currTask = RobotAction::explore;
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
	if (position_ >= map.getSize() || !tile->isMoveValid()) {
		clearMoveTargets();
		throw std::runtime_error("Robot is on invalid tile\n");
	}

	tilesToCheck[position_] = false;

	// Clean if trash
	cleanTile();
	if (cleaningEfficiency > 0) {
		return std::make_tuple(RobotAction::clean, Direction::none);
	}

	if (currTask == RobotAction::explore) {		// Exploring mode
		if (path.empty() && createPathUnvisited()) {
			// Succesfully found new tile to explore
			return std::make_tuple(RobotAction::move, move());
		}
		else if (path.empty()) {
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
		if (path.empty()) {
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
			if (neighbour.has_value() && map.getTile(*neighbour)) {
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
		else if (createPathTrash()) {
			// Found trash in memory, go clean it
			return std::make_tuple(RobotAction::move, move());
		}
		else if (createPathToVisit()) {
			// Go to nearest tile to visit
			return std::make_tuple(RobotAction::move, move());
		}
		else if (createPath(chargerId_)) {
			// Return to charger
			if (position_ == chargerId_) {
				// Already in charger
				if (createPathUnvisited()) {
					// Explore
					currTask = RobotAction::explore;
					return std::make_tuple(RobotAction::move, move());
				}
				else {
					// Nothing to do - all complete
					currTask = RobotAction::none;
					return makeAction();
				}
			}
			return std::make_tuple(RobotAction::move, move());
		}
		else if (createPathUnvisited()) {
			// Can't reach charger, explore tiles
			currTask = RobotAction::explore;
			return std::make_tuple(RobotAction::move, move());
		}
		else {
			// Can't reach charger and nothing to explore, error
			throw std::runtime_error("Robot can't reach charger\n");
		}
	}
	else if (currTask == RobotAction::none) {
		return std::make_tuple(RobotAction::none, Direction::none);
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

void Robot::orderToCleanEfficiently() {
	orderToClean(position_, -1);
}

bool Robot::orderToClean(size_t id, unsigned int radius) {
	clearMoveTargets();
	if (!(createPath(id))) {
		return false;
	}
	currTask = RobotAction::move;

	std::vector<bool> visited(tilesToCheck.size(), false);
	std::queue<std::pair<size_t, size_t>> q; // (index, distance)
	std::vector<size_t> parent(map.getSize(), std::numeric_limits<size_t>::max());

	q.push({ id, 0 });
	parent[id] = id;
	visited[id] = true;
	tilesToCheck[id] = true;

	while (!q.empty()) {
		auto [index, dist] = q.front();
		q.pop();
		if (dist > radius) continue;

		const Tile* tile = map.getTile(index);
		if (!tile || !tile->isMoveValid() || dynamic_cast<const UnVisited*>(tile)) continue;

		if (!tilesToCheck[parent[index]]) {
			tilesToCheck[index] = true;
		}

		for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right}) {
			std::optional<size_t> nIndex = map.getIndex(index, dir);
			if (nIndex && !visited[*nIndex]) {
				visited[*nIndex] = true;
				parent[*nIndex] = index;
				q.push({ *nIndex, dist + 1 });
			}
		}
	}
	return true;
}

void Robot::resetMemory() {
	clearMoveTargets();
	map = Map(map.getWidth(), map.getHeight(), map.getChargerId());
	currTask = RobotAction::explore;
}

void Robot::loadRobot(std::istream& in) {
	// 1. Odczytaj map ze strumienia a do pustej linii
	std::stringstream mapStream;
	std::string line;
	while (std::getline(in, line)) {
		if (line.empty()) break; // pusta linia jako separator
		mapStream << line << "\n";
	}
	map.loadMap(mapStream);

	// 2. Wczytaj pozycje i ustawienia
	int currTaskInt;
	size_t tilesSize = 0;
	size_t pathSize = 0;

	in >> position_ >> chargerId_;
	in >> currTaskInt;
	currTask = static_cast<RobotAction>(currTaskInt);
	in >> cleaningEfficiency;

	// 3. Wczytaj tilesToCheck
	in >> tilesSize;
	tilesToCheck.resize(tilesSize);
	for (size_t i = 0; i < tilesSize; ++i) {
		bool val;
		in >> val;
		tilesToCheck[i] = val;
	}

	// 4. Wczytaj path
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
	// 1. Zapisz map
	map.saveMap(out);
	out << "\n";

	// 2. Zapisz pozycje i ustawienia
	out << position_ << ' ' << chargerId_ << ' ';
	out << static_cast<int>(currTask) << ' ';
	out << cleaningEfficiency << ' ';

	// 3. Zapisz tilesToCheck
	out << tilesToCheck.size() << ' ';
	for (bool b : tilesToCheck) {
		out << b << ' ';
	}

	// 4. Zapisz path
	out << path.size() << ' ';
	std::queue<size_t> tempQueue = path;
	while (!tempQueue.empty()) {
		out << tempQueue.front() << ' ';
		tempQueue.pop();
	}

	out << "\n"; // kocowy newline dla czytelnoci
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
	if (robot.map.getSize() == 0) {
		os << "No robot memory.\n";
		return os;
	}

	os << "\n";

	std::ostringstream oss;
	oss << robot.map; // This calls operator<< for MemoryMap
	std::string mapStr = oss.str();

	// Place 'R' for robot's current position on its remembered map
	replaceCharAtIndex(mapStr, robot.position_, 'R', robot.map.getWidth());

	// If robot has a path, place 'X' for the destination
	if (!robot.path.empty()) {
		replaceCharAtIndex(mapStr, robot.path.back(), 'X', robot.map.getWidth());
	}
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
	return os;
}