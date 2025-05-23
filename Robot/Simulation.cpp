// Simulation.cpp (Updated implementations)

#include "Simulation.h"
#include <string>
#include <iostream>
#include <limits>
#include <stdexcept> // For std::runtime_error
#include <fstream>   // For std::ofstream to save simulation
#include <sstream>   // For std::stringstream

// --- Helper functions (kept as before) ---
void clearScreen() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For Linux/macOS
#endif
}

void pressEnterToContinue() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

size_t getValidatedSizeTInput(const std::string& prompt) {
    size_t value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a positive whole number.\n";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

unsigned int getValidatedUnsignedIntInput(const std::string& prompt) {
    unsigned int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a positive whole number.\n";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

fs::path getFilePathInput(const std::string& prompt) {
    std::string pathStr;
    std::cout << prompt;
    std::getline(std::cin, pathStr);
    return fs::path(pathStr);
}

// --- Fully Implemented Simulation Option Functions ---

// Validates if the map is properly set up and ready.
bool Simulation::isSimulationValid() const {
    if (!map.isMapValid()) {
        std::cerr << "Error: The map is not valid for simulation.\n";
        return false;
    }
    // You might also add checks here for the robot's initial position validity if needed.
    std::cout << "Simulation validity check passed.\n";
    return true;
}

// Adds rubbish to a specific tile on the map.
void Simulation::addRubbish(size_t tileId, unsigned int dirtiness) {
    std::cout << "Action: Attempting to add " << dirtiness << " rubbish to Tile ID: " << tileId << ".\n";

    // Get the tile from the map using the assumed getTile method
    Tile* targetTile = map.getTile(tileId);

    if (targetTile) {
        // Try to cast it to a Floor tile
        Floor* floorTile = dynamic_cast<Floor*>(targetTile);
        if (floorTile) {
            floorTile->getDirty(dirtiness); // Call getDirty on the Floor tile
            std::cout << "Rubbish added to Tile " << tileId << ". New cleanliness: " << floorTile->getCleanliness() << ".\n";
        }
        else {
            std::cout << "Failed to add rubbish: Tile ID " << tileId << " is not a Floor tile (it's an Obstacle, Charger, or UnVisited).\n";
        }
    }
    else {
        std::cout << "Failed to add rubbish: Tile ID " << tileId << " does not exist on the map.\n";
    }
}

// Changes the robot's current position to a new tile ID.
void Simulation::changeRobotsPosition(size_t newPositionId) {
    if (robot.setPosition(newPositionId)) {
        std::cout << "Robot's position successfully changed to Tile ID: " << newPositionId << ".\n";
        // After changing position, the robot should update its memory about the new tile
        const Tile* currentTile = map.getTile(newPositionId); // Get the tile from the main map
        if (currentTile) {
            updateRobotMemory(newPositionId, currentTile); // Update robot's internal memory
        }
        else {
            // This case should ideally not happen if setPosition was successful and map.getTile is robust
            std::cerr << "Warning: Could not retrieve actual tile for ID " << newPositionId << " to update robot memory.\n";
        }
    }
    else {
        std::cout << "Failed to change robot's position to Tile ID: " << newPositionId << ". Invalid position or unable to set.\n";
    }
}

// Orders the robot to navigate back to its charger.
void Simulation::orderRobotToGoHome() {
    robot.orderToGoHome();
    std::cout << "Robot ordered to go home.\n";
}

// Orders the robot to move to a specific target tile ID.
void Simulation::orderRobotToMove(size_t targetTileId) {
    if (robot.orderToMove(targetTileId)) {
        std::cout << "Robot ordered to move to Tile ID: " << targetTileId << ".\n";
    }
    else {
        std::cout << "Failed to order robot to move to Tile ID: " << targetTileId << ". Path not found or invalid target.\n";
    }
}

// Orders the robot to clean a specific tile ID with a given radius.
void Simulation::orderRobotToClean(size_t tileId, unsigned int radius) {
    if (robot.orderToClean(tileId, radius)) {
        std::cout << "Robot ordered to clean Tile ID: " << tileId << " with radius: " << radius << ".\n";
    }
    else {
        std::cout << "Failed to order robot to clean Tile ID: " << tileId << ". Invalid target or already clean.\n";
    }
}

// Resets the robot's internal memory of the map.
void Simulation::resetRobotMemory() {
    if (robot.resetMemory()) {
        std::cout << "Robot's memory successfully reset.\n";
    }
    else {
        std::cout << "Failed to reset robot's memory.\n";
    }
}

// Saves the current state of the map and robot to a specified file.
void Simulation::saveSimulation(fs::path filePath) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for saving: " << filePath << std::endl;
        return;
    }

    try {
        // Map data first
        map.saveMap(outFile);
        outFile << "\n"; // Separator between map and robot data
        // Robot data next
        robot.saveRobot(outFile);
        std::cout << "Simulation state saved successfully to: " << filePath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error during saving: " << e.what() << std::endl;
    }
    outFile.close();
}

// Loads simulation data from a specified file (calls existing loadFromFile).
void Simulation::loadSimulation(fs::path filePath) {
    std::cout << "Action: Loading simulation from file: " << filePath << ".\n";
    loadFromFile(filePath);
}

// Runs the simulation for a specified number of steps, allowing the robot to perform actions.
// Runs the simulation for a specified number of steps, allowing the robot to perform actions.
void Simulation::runSimulation(unsigned int steps) {
    if (steps == 0) {
        std::cout << "No steps to run.\n";
        return;
    }
    std::cout << "Action: Running simulation for " << steps << " steps...\n";

    for (unsigned int i = 0; i < steps; ++i) {
        std::cout << "\n--- Step " << (i + 1) << " ---\n";

        // --- NEW: Robot explores current tile and its neighbors BEFORE making an action ---
        size_t currentRobotPos = robot.getPosition();

        // 1. Explore the current tile
        const Tile* currentTile = map.getTile(currentRobotPos);
        if (currentTile) {
            updateRobotMemory(currentRobotPos, currentTile);
        }
        else {
            std::cerr << "Error: Robot at invalid position " << currentRobotPos << " or tile not found on main map. Stopping simulation.\n";
            break; // Critical error, stop simulation
        }

        // 2. Explore immediate neighbors of the current tile
        // Use Map::getIndex to safely get neighbor IDs
        Direction directions[] = { Direction::up, Direction::down, Direction::left, Direction::right };

        for (Direction dir : directions) {
            std::optional<size_t> neighborIdOpt = map.getIndex(currentRobotPos, dir);
            if (neighborIdOpt.has_value()) {
                size_t neighborId = neighborIdOpt.value();
                const Tile* neighborTile = map.getTile(neighborId);
                if (neighborTile) {
                    updateRobotMemory(neighborId, neighborTile);
                }
            }
        }
        std::cout << "Robot completed sensing and updating memory for current tile and its neighbors.\n";
        // --- END NEW EXPLORATION LOGIC ---

        // 3. Robot performs an action (move, clean, explore) based on its updated internal map
        std::tuple<RobotAction, Direction> actionResult = robot.makeAction();
        RobotAction action = std::get<0>(actionResult);

        std::cout << "Robot action: ";
        switch (action) {
        case RobotAction::move: std::cout << "Move"; break;
        case RobotAction::clean: std::cout << "Clean"; break;
        case RobotAction::explore: std::cout << "Explore"; break;
        default: std::cout << "Unknown"; break; // Should ideally not be reached
        }
        std::cout << ".\n";

        // 4. If robot cleaned, update the actual main map
        // Note: The 'clean' action typically happens on the robot's *current* position.
        // If the robot moves, and then cleans, this logic might need refinement based on
        // when the robot *decides* to clean vs. when the cleaning *takes effect*.
        // For now, assuming RobotAction::clean implies cleaning at its post-action position.
        if (action == RobotAction::clean) {
            cleanTile(robot.getPosition(), robot.getCleaningEfficiency());
        }

        // Optionally, print simulation state after each step (or every N steps)
        // printSimulation();
        // pressEnterToContinue(); // Pause after each step if desired
    }
    std::cout << "Simulation finished after " << steps << " steps.\n";
}

// Handles exiting the simulation.
void Simulation::exitSimulation() {
    std::cout << "Exiting simulation. Goodbye!\n";
    // Add any final cleanup here if needed
}

// Prints the current state of the simulation (Map and Robot).
void Simulation::printSimulation() {
    std::cout << "\n--- Current Simulation State ---\n";

    // These rely on `operator<<(std::ostream& os, const Map& map)`
    // and `operator<<(std::ostream& os, const Robot& robot)` being implemented.
    std::cout << "Map State:\n";
    std::cout << map;

    std::cout << "\nRobot State:\n";
    std::cout << robot;

    std::cout << "--------------------------------\n";
}

// Updates the robot's internal memory (its own map) based on a sensed tile.
void Simulation::updateRobotMemory(size_t tileId, const Tile* tileObj) {
    // This function acts as the bridge between the actual map and robot's memory.
    robot.exploreTile(tileId, tileObj); // Robot adds/updates its internal map knowledge
    std::cout << "Internal: Robot's memory updated for Tile ID " << tileId << ".\n";
}

// Cleans a specific tile on the main map.
void Simulation::cleanTile(size_t tileId, unsigned int efficiency) {
    std::cout << "Internal: Attempting to clean Tile ID: " << tileId << " with efficiency: " << efficiency << ".\n";

    Tile* targetTile = map.getTile(tileId); // Get the tile from the main map

    if (targetTile) {
        // Try to cast it to a Floor tile
        Floor* floorTile = dynamic_cast<Floor*>(targetTile);
        if (floorTile) {
            floorTile->getCleaned(efficiency); // Call getCleaned on the Floor tile
            std::cout << "Tile " << tileId << " cleaned. New cleanliness: " << floorTile->getCleanliness() << ".\n";
        }
        else {
            std::cout << "Failed to clean: Tile ID " << tileId << " is not a Floor tile (it's an Obstacle, Charger, or UnVisited).\n";
        }
    }
    else {
        std::cout << "Failed to clean: Tile ID " << tileId << " does not exist on the map.\n";
    }
}

// --- The start() function (kept as before) ---
void Simulation::start(fs::path filePath) {
    clearScreen();
    std::cout << "---------------------------------------------------\n";
    std::cout << "        WELCOME TO THE ROBOT VACUUM CLEANER        \n";
    std::cout << "              SIMULATION GAME!                     \n";
    std::cout << "---------------------------------------------------\n";
    std::cout << "\n";

    if (!filePath.empty()) {
        std::cout << "Attempting to load initial simulation data from: " << filePath << std::endl;
        loadFromFile(filePath);
        if (isSimulationValid()) { // Check validity after loading
            std::cout << "Initial simulation state loaded and validated.\n";
        }
        else {
            std::cerr << "Initial simulation state is invalid. Proceeding with caution or resetting.\n";
            // You might add logic here to create a default valid map if loading fails
        }
        pressEnterToContinue();
    }
    else {
        std::cout << "Starting new simulation (no file loaded).\n";
        // If starting new, you might need to initialize map and robot with default valid states here
        // e.g., map = Map(width, height, chargerId); robot = Robot(width, height, chargerId);
        pressEnterToContinue();
    }

    bool running = true;
    while (running) {
        clearScreen();
        printSimulation();

        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Add Rubbish to a Tile\n";
        std::cout << "2. Change Robot's Position\n";
        std::cout << "3. Order Robot to Go Home\n";
        std::cout << "4. Order Robot to Move to a Tile\n";
        std::cout << "5. Order Robot to Clean a Tile\n";
        std::cout << "6. Reset Robot's Memory\n";
        std::cout << "7. Run Simulation Steps\n";
        std::cout << "8. Save Current Simulation State\n";
        std::cout << "9. Load Simulation from File\n";
        std::cout << "0. Exit Simulation\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            pressEnterToContinue();
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: { // Add Rubbish
            size_t tileId = getValidatedSizeTInput("Enter Tile ID to add rubbish to: ");
            unsigned int dirtiness = getValidatedUnsignedIntInput("Enter dirtiness level (e.g., 1-9): ");
            addRubbish(tileId, dirtiness);
            break;
        }
        case 2: { // Change Robot's Position
            size_t newPositionId = getValidatedSizeTInput("Enter new Tile ID for robot's position: ");
            changeRobotsPosition(newPositionId);
            break;
        }
        case 3: { // Order Robot to Go Home
            orderRobotToGoHome();
            break;
        }
        case 4: { // Order Robot to Move
            size_t targetTileId = getValidatedSizeTInput("Enter target Tile ID for robot to move: ");
            orderRobotToMove(targetTileId);
            break;
        }
        case 5: { // Order Robot to Clean
            size_t tileId = getValidatedSizeTInput("Enter Tile ID for robot to clean: ");
            unsigned int radius = getValidatedUnsignedIntInput("Enter cleaning radius: ");
            orderRobotToClean(tileId, radius);
            break;
        }
        case 6: { // Reset Robot's Memory
            resetRobotMemory();
            break;
        }
        case 7: { // Run Simulation Steps
            unsigned int steps = getValidatedUnsignedIntInput("Enter number of simulation steps to run: ");
            runSimulation(steps);
            break;
        }
        case 8: { // Save Simulation
            fs::path savePath = getFilePathInput("Enter filename to save simulation (e.g., my_sim.txt): ");
            saveSimulation(savePath);
            break;
        }
        case 9: { // Load Simulation
            fs::path loadPath = getFilePathInput("Enter filename to load simulation from (e.g., other_sim.txt): ");
            loadSimulation(loadPath);
            break;
        }
        case 0: { // Exit Simulation
            exitSimulation();
            running = false;
            break;
        }
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }

        if (running) {
            pressEnterToContinue();
        }
    }

    std::cout << "Thank you for playing the Robot Vacuum Cleaner Simulation!\n";
}

// --- loadFromFile implementation (kept as before) ---
// Note: This relies on Map::loadMap(istream) and Robot::loadRobot(istream) implementations
void Simulation::loadFromFile(fs::path filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::stringstream mapDataStream;
    std::stringstream robotDataStream;
    std::string line;
    bool readingMap = true;

    while (std::getline(inputFile, line)) {
        if (readingMap) {
            if (line.empty()) { // An empty line signals the end of map data and start of robot data
                readingMap = false;
            }
            else {
                mapDataStream << line << std::endl;
            }
        }
        else {
            robotDataStream << line << std::endl;
        }
    }
    inputFile.close();

    try {
        map.loadMap(mapDataStream);
        std::cout << "Map data loaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading map data: " << e.what() << std::endl;
        // If map loading fails, we cannot safely initialize the robot with map dimensions.
        // It's best to stop here or throw an exception.
        std::cerr << "Map loading failed. Cannot proceed with robot initialization. Returning.\n";
        return;
    }

    // --- NEW LOGIC FOR ROBOT INITIALIZATION ---
    // Check if the robotDataStream is empty (i.e., no robot data was found in the file)
    if (robotDataStream.str().empty()) {
        std::cout << "Robot data not found in file. Initializing robot with map dimensions and charger ID.\n";
        // Initialize/re-create the robot using the dimensions and charger ID from the loaded map.
        // This assumes map.getWidth(), map.getHeight(), map.getChargerId() are accessible after map.loadMap().
        robot = Robot(map.getWidth(), map.getHeight(), map.getChargerId());

        std::cout << "Robot initialized with: Width=" << map.getWidth()
            << ", Height=" << map.getHeight()
            << ", ChargerID=" << map.getChargerId() << ".\n";
    }
    else {
        // If robot data exists in the stream, proceed to load it
        try {
            robot.loadRobot(robotDataStream);
            std::cout << "Robot data loaded successfully." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading robot data: " << e.what() << std::endl;
            // Fallback: If robot loading fails, re-initialize it with map defaults.
            std::cerr << "Robot loading failed. Re-initializing robot with map dimensions and charger ID as a fallback.\n";
            robot = Robot(map.getWidth(), map.getHeight(), map.getChargerId());
        }
    }
}