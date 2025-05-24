#include <iostream>
#include <ostream>
#include <filesystem>
#include "FileManager.hpp"
#include "Simulation.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
	
	Simulation simulation;
	fs::path filePath = "";

	if (argc == 2)
	{
		filePath = argv[1];
	}

	simulation.start(filePath);

	return 0;
}