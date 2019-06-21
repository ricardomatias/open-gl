#include "File.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>


std::string Read(std::string filePath)
{
	std::string line;
	std::stringstream contents;

	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(filePath);

		contents << file.rdbuf();

		file.close();
	}
	catch (const std::exception&)
	{
		std::cout << "ERROR: File '" << filePath << "' could not be read." << std::endl;
	}

	return contents.str();
}
