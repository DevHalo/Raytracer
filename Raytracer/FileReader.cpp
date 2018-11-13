#include "pch.h"
#include "FileReader.h"
#include <iostream>
#include <fstream>
#include "Sphere.h"
#include "Light.h"

// Takes in the name of the file and returns a pointer carrying all the parameters
std::unique_ptr<Parameters> FileReader::ReadFile(std::string filename)
{
	// Check if the file is actually a txt file
	if (filename.substr(filename.find_last_of(".") + 1) == "txt") 
	{
		// Open file and parse input if the file successfully opens
		std::ifstream file(filename);
		if (file.is_open())
		{
			std::unique_ptr<Parameters> param = std::make_unique<Parameters>();	

			std::string buffer, command, value; 
			char ch;							

			while (file >> buffer)
			{
				std::cout << "|" << buffer << "|";
			}

												/*
			while (file.get(ch))
			{
				// Convert character to ascii
				int ascii = int(ch);
				if (ascii != 20 && ascii != 10)
				{
					// If the character is a letter (part of a parameter)
					// add the character to the buffer
					buffer += ch;
				}
				else if (ascii = 20)
				{
					// If the current character is a space verify:
					// Is a command currently being processed? Invalid input
					if (!command.empty())
					{
						file.close();
						std::cout << "Malformed Parameter\n";
						return NULL;
					}
					else
					{
						// Finished reading a possible parameter
						command = buffer;
						buffer = "";
					}
				}
				else if (ascii == 10)
				{
					// If the character is a line feed, verify:
					// Are we currently processing a command
					if (!command.empty())
					{
						value = buffer;
						buffer = "";

						
						if (command == "SPHERE")
						{

						}
						else if (command == "LIGHT")
						{

						}
						else if (command == "OUTPUT")
						{

						}
						else {
							// Try casting the value to a float
							try
							{
								float val = std::stof(value);

								if (command == "NEAR")
								{
									param->near = std::make_unique<float>(val);
								}
								else if (command == "LEFT")
								{
									param->left = std::make_unique<float>(val);
								}
								else if (command == "RIGHT")
								{
									param->right = std::make_unique<float>(val);
								}
								else if (command == "BOTTOM")
								{
									param->bottom = std::make_unique<float>(val);
								}
								else if (command == "TOP")
								{
									param->top = std::make_unique<float>(val);
								}
								else if (command == "RES")
								{
									switch (numVariables)
									{
										case 0:
											param->resX = std::make_unique<int>(val);
											numVariables++;
											break;
										case 1:
											param->resY = std::make_unique<int>(val);
											numVariables = 0;
											break;
									}
								}
							}
							catch (const std::invalid_argument e)
							{
								std::cout << "Attempted to assign a parameter an invalid value\n";
							}
						}
					}
					buffer = "";
					command = "";
					value = "";
				}
			}
			*/

			file.close();

			if (param->near == nullptr ||
				param->left == nullptr ||
				param->right == nullptr ||
				param->bottom == nullptr ||
				param->top == nullptr ||
				param->resX == nullptr ||
				param->resY == nullptr ||
				param->back == nullptr ||
				param->ambient == nullptr)
			{
				std::cout << "Not all parameters were supplied\n";
			}
			return param;
		}
	}
	return nullptr;
}