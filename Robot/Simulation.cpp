#include "Simulation.h"
#include <string>
#include <limits> // Required for std::numeric_limits

bool Simulation::isSimulationValid() const {
    // TODO: Implement validation check
    return false;
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

void Simulation::addRubbish() {
    // unsigned int howDirty = 1, size_t id = static_cast<size_t>(-1)
    // TODO: Implement adding rubbish
}


void Simulation::changeRobotsPosition() {
    // TODO: set new robot position
}
void Simulation::orderRobotToGoHome() {
    // TODO: just call function XD
}
void Simulation::orderRobotToMove() {
    // TODO: order to move at
}
void Simulation::orderRobotToClean() {
    // TODO: order to clean at
}
void Simulation::resetRobotMemory() {
    // TODO: order to reset memory
}
void Simulation::saveSimulation() {
    // TODO: save to file
}
void Simulation::loadSimulation() {
    // TODO: load from file
}
void Simulation::runSimulation() {
    // TODO: run X steps, every time before step update tiles around in memory
}
void Simulation::exitSimulation() {
    // TODO: exit
}

void Simulation::printSimulation() {
    // TODO: print progress of simulation
}

void Simulation::updateRobotMemory(size_t tileId, const Tile& tileObj) {
    // TODO: update memory
}
void Simulation::cleanTile(size_t tileId, unsigned int efficiency) {
    // TODO: clean tile
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
        case 1:
            addRubbish(); // Will need to prompt user for tile ID and dirtiness
            break;
        case 2:
            changeRobotsPosition(); // Will need to prompt user for new position
            break;
        case 3:
            orderRobotToGoHome();
            break;
        case 4:
            orderRobotToMove(); // Will need to prompt user for target tile ID
            break;
        case 5:
            orderRobotToClean(); // Will need to prompt user for target tile ID and efficiency
            break;
        case 6:
            resetRobotMemory();
            break;
        case 7:
            runSimulation(); // Will need to prompt user for number of steps
            break;
        case 8:
            saveSimulation(); // Will need to prompt user for file path
            break;
        case 9:
            // When loading, you might want to prompt for a new file path
            // For now, let's just re-call loadSimulation, which should prompt for path
            loadSimulation();
            break;
        case 0:
            exitSimulation(); // This function should handle any cleanup and set running to false
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
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
