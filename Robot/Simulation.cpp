#include "Simulation.h"
#include <string>
#include <limits> // Required for std::numeric_limits

bool Simulation::isSimulationValid() const {
    std::cout << "Validation check (not fully implemented).\n";
    return true; // Assume valid for now to proceed
}

void clearScreen() {
#ifdef _WIN32
    system("cls"); // For Windows
#endif
}

void pressEnterToContinue() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
    std::cin.get(); // Wait for Enter key
}

// Helper to get size_t input with validation
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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear leftover newline
            return value;
        }
    }
}

// Helper to get unsigned int input with validation
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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear leftover newline
            return value;
        }
    }
}

// Helper to get file path input (can contain spaces)
fs::path getFilePathInput(const std::string& prompt) {
    std::string pathStr;
    std::cout << prompt;
    std::getline(std::cin, pathStr); // Use getline to read whole line
    return fs::path(pathStr);
}

bool Simulation::isSimulationValid() const {
    std::cout << "Validation check (not fully implemented).\n";
    return true; // Assume valid for now to proceed
}

void Simulation::addRubbish(size_t tileId, unsigned int dirtiness) {
    std::cout << "Action: Adding " << dirtiness << " rubbish to Tile ID " << tileId << ".\n";
    // TODO: Implement logic here to modify map tile's dirtiness
    // Example: map.getTile(tileId).addDirt(dirtiness);
}

void Simulation::changeRobotsPosition(size_t newPositionId) {
    std::cout << "Action: Changing robot's position to Tile ID " << newPositionId << ".\n";
    // TODO: Implement logic here to change robot's position
    // Example: robot.setPosition(newPositionId);
}

void Simulation::orderRobotToGoHome() {
    std::cout << "Action: Ordering robot to go home.\n";
    // TODO: Implement logic here to make robot go home
    // Example: robot.orderToGoHome();
}

void Simulation::orderRobotToMove(size_t targetTileId) {
    std::cout << "Action: Ordering robot to move to Tile ID " << targetTileId << ".\n";
    // TODO: Implement logic here to make robot move
    // Example: robot.orderToMove(targetTileId);
}

void Simulation::orderRobotToClean(size_t tileId, unsigned int radius) {
    std::cout << "Action: Ordering robot to clean Tile ID " << tileId << " with radius " << radius << ".\n";
    // TODO: Implement logic here to make robot clean
    // Example: robot.orderToClean(tileId, radius);
}

void Simulation::resetRobotMemory() {
    std::cout << "Action: Resetting robot's memory.\n";
    // TODO: Implement logic here to reset robot's memory
    // Example: robot.resetMemory();
}

void Simulation::saveSimulation(fs::path filePath) {
    std::cout << "Action: Saving simulation to file: " << filePath << ".\n";
    // TODO: Implement logic here to save map and robot state to file
    // Example: map.saveMap(fileStream); robot.saveRobot(fileStream);
}

void Simulation::loadSimulation(fs::path filePath) {
    std::cout << "Action: Loading simulation from file: " << filePath << ".\n";
    // This will call the already implemented loadFromFile function
    loadFromFile(filePath);
}

void Simulation::runSimulation(unsigned int steps) {
    std::cout << "Action: Running simulation for " << steps << " steps.\n";
    // TODO: Implement simulation loop logic
    // Example: for (unsigned int i = 0; i < steps; ++i) { robot.makeAction(); /* update map based on action */ }
}

void Simulation::exitSimulation() {
    std::cout << "Exiting simulation...\n";
    // TODO: Add any final cleanup here if needed
}

void Simulation::printSimulation() {
    std::cout << "\n--- Current Simulation State ---\n";
    // TODO: Implement actual printing of Map and Robot state
    // For example:
    // std::cout << map; // Assuming operator<< is overloaded for Map
    // std::cout << robot; // Assuming operator<< is overloaded for Robot
    std::cout << "(Map and Robot display not fully implemented).\n";
    std::cout << "Robot is currently at position: " << robot.getPosition() << "\n"; // Example direct access
    std::cout << "--------------------------------\n";
}

void Simulation::updateRobotMemory(size_t tileId, const Tile& tileObj) {
    std::cout << "Internal: Updating robot memory for Tile ID " << tileId << ".\n";
    // TODO: Implement updating robot's internal map memory
    // Example: robot.exploreTile(tileId, tileObj);
}

void Simulation::cleanTile(size_t tileId, unsigned int efficiency) {
    std::cout << "Internal: Cleaning Tile ID " << tileId << " with efficiency " << efficiency << ".\n";
    // TODO: Implement actual tile cleaning logic
    // Example: map.getTile(tileId).clean(efficiency);
}


// start
void Simulation::start(fs::path filePath) {
    clearScreen();
    std::cout << "---------------------------------------------------\n";
    std::cout << "        WELCOME TO THE ROBOT VACUUM CLEANER        \n";
    std::cout << "              SIMULATION GAME!                     \n";
    std::cout << "---------------------------------------------------\n";
    std::cout << "\n";

    // Attempt to load simulation data if a file path is provided
    if (!filePath.empty()) {
        std::cout << "Attempting to load initial simulation data from: " << filePath << std::endl;
        loadFromFile(filePath);
        pressEnterToContinue();
    }
    else {
        std::cout << "Starting new simulation (no file loaded).\n";
        pressEnterToContinue();
    }

    bool running = true;
    while (running) {
        clearScreen();

        // 1. Print the current state of the simulation
        printSimulation();

        // 2. Display the main menu options
        std::cout << "\n--- Main Menu ---" << std::endl;
        std::cout << "1. Add Rubbish to a Tile" << std::endl;
        std::cout << "2. Change Robot's Position" << std::endl;
        std::cout << "3. Order Robot to Go Home" << std::endl;
        std::cout << "4. Order Robot to Move to a Tile" << std::endl;
        std::cout << "5. Order Robot to Clean a Tile" << std::endl;
        std::cout << "6. Reset Robot's Memory" << std::endl;
        std::cout << "7. Run Simulation Steps" << std::endl;
        std::cout << "8. Save Current Simulation State" << std::endl;
        std::cout << "9. Load Simulation from File" << std::endl;
        std::cout << "0. Exit Simulation" << std::endl;
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        // Input validation
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            pressEnterToContinue();
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // 3. Execute the chosen action
        switch (choice) {
        case 1: { // Add Rubbish
            size_t tileId = getValidatedSizeTInput("Enter Tile ID to add rubbish to: ");
            unsigned int dirtiness = getValidatedUnsignedIntInput("Enter dirtiness level (e.g., 1-10): ");
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
            loadSimulation(loadPath); // Calls loadFromFile with the user-provided path
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

        // Pause before looping again, unless exiting
        if (running) {
            pressEnterToContinue();
        }
    }

    std::cout << "Thank you for playing the Robot Vacuum Cleaner Simulation!" << std::endl;
}

//Tile* Simulation::findTileWithId(size_t id) const {
//    // TODO: Implement finding tile by ID
//    return nullptr;
//}

// loadFromFile
void Simulation::loadFromFile(fs::path filePath) {
    std::ifstream inputFile(filePath); // Open the file for reading.

    // Check if the file was opened successfully.
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return; // Exit if file cannot be opened.
    }

    std::stringstream mapDataStream;
    std::stringstream robotDataStream;
    std::string line;
    bool readingMap = true;

    while (std::getline(inputFile, line)) {
        if (readingMap) {
            if (line.empty()) {
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

    // Load data into the map object using its loadMap method.
    try {
        map.loadMap(mapDataStream);
        std::cout << "Map data loaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading map data: " << e.what() << std::endl;
    }


    // Load data into the robot object using its loadRobot method.
    try {
        robot.loadRobot(robotDataStream);
        std::cout << "Robot data loaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading robot data: " << e.what() << std::endl;
    }
}
