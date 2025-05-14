#include <iostream>
#include <ostream>
#include <filesystem>
#include "FileManager.hpp"
#include "Simulation.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
	FileManager<Simulation> fm;
	Simulation simulation;
	fs::path filePath;
	bool correctFile = false;

	std::cout << "\nCLEANING ROBOT SIMULATION\n\n\n";

	if (argc == 2)
	{
		filePath = argv[1];
		
		if (fs::exists(filePath) && fm.readingMode(filePath)) {
			std::stringstream ss;
			fm.loadFromFile(ss);
			ss >> simulation;
			correctFile = simulation.isSimulationValid();
		}
	}

	while (!correctFile) {
		std::cout << "Failed to load file\n";
		std::cout << "Give file path: ";
		std::cin >> filePath;

		if (fs::exists(filePath) && fm.readingMode(filePath)) {
			std::stringstream ss;
			fm.loadFromFile(ss);
			ss >> simulation;
			correctFile = simulation.isSimulationValid();
		}
	}
	
	simulation.runSimulation(0);


	return 0;
}