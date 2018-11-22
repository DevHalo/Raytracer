#include "pch.h"
#include "FileReader.h"
#include <fstream>
#include <iostream>

// Takes in the name of the file and returns a pointer carrying all the parameters
std::unique_ptr<Parameters> FileReader::ReadFile(std::string filename)
{
	// Check if the file is actually a txt file
	if (filename.substr(filename.find_last_of(".") + 1) != "txt") { return nullptr; }
	
	// Open file and return null pointer if it doesn't open
	std::ifstream file(filename);

	if (!file.is_open()) { 
		return nullptr; 
	}

	// Stores the parameters returned to the caller
	std::unique_ptr<Parameters> param = std::make_unique<Parameters>();

	std::string buffer, command;		// Temp variables for storing the buffer and current command 
	int numVariable = 0;				// Stores the number of variables for a given command
	std::shared_ptr<Sphere> tempSphere;	// Temp storage for a sphere object
	std::shared_ptr<Light> tempLight;	// Temp storage for a light object

	// While we still have 
	while (file >> buffer)
	{
		// Attempt to cast the buffer to a float
		std::shared_ptr<float> val = ConvertToFloat(buffer);

		// If the current word is alphabetic
		if (val == nullptr)
		{
			// If we were already processing a command before, reset everything
			if (!command.empty())
			{
				if (command == "AMBIENT" && param->ambient != nullptr) { delete &(param->ambient); }
				if (command == "BACK" && param->back != nullptr) { delete &(param->back); }
			}

			// Check if the buffer matches any of the parameter names
			if (buffer == "RES") { command = buffer; numVariable = 2; }
			else if (buffer == "BACK")
			{
				command = buffer;
				numVariable = 3;

				// Allocate space for background colour
				param->back = std::make_shared<glm::vec3>();
			}
			else if (buffer == "AMBIENT")
			{
				command = buffer;
				numVariable = 3;
	
				// Allocate space for background colour
				param->ambient = std::make_shared<glm::vec3>();
			}
			else if (buffer == "SPHERE")
			{
				// If there are less than 15 lights
				if (param->spheres.size() < 14)
				{
					command = buffer;
					numVariable = 15;

					// Allocate space for possible new sphere
					tempSphere = std::make_shared<Sphere>();
				}
				else { std::cout << "Failed to add sphere. There are too many spheres.\n"; }
			}
			else if (buffer == "LIGHT")
			{
				if (param->lights.size() < 10)
				{
					command = buffer;
					numVariable = 7;

					// Allocate space for possible new light
					tempLight = std::make_shared<Light>();
				}
				else { std::cout << "Failed to add light. There are too many lights.\n"; }
			}
			else if (buffer == "OUTPUT") { command = buffer; }
			else
			{
				// If the buffer is a string, check if we are
				// currently processing a command and assign accordingly
				if (command == "SPHERE" && numVariable == 15)
				{ tempSphere->name = buffer; numVariable--; }
				else if (command == "LIGHT" && numVariable == 7)
				{ tempLight->name = buffer; numVariable--; }
				else if (command == "OUTPUT")
				{ param->output = buffer; command = ""; }
				else { command = buffer; }
			}
		}
		else
		{
			// If the value is numeric, process the buffer to whatever is
			// currently the command being processed
			if (command == "NEAR") { param->near = std::move(val); command = ""; }
			else if (command == "LEFT") { param->left = std::move(val); command = ""; }
			else if (command == "RIGHT") { param->right = std::move(val); command = ""; }
			else if (command == "BOTTOM") { param->bottom = std::move(val); command = ""; }
			else if (command == "TOP") { param->top = std::move(val); command = ""; }
			else if (command == "RES") 
			{
				switch (numVariable)
				{
					case 1: param->resY = std::make_shared<int>(static_cast<int>(*val)); command = ""; break;
					case 2: param->resX = std::make_shared<int>(static_cast<int>(*val)); break;
				}

				numVariable--;
			}
			else if (command == "BACK")
			{
				switch (numVariable)
				{
					case 1: param->back->b = *val; command = ""; break;
					case 2: param->back->g = *val; break;
					case 3: param->back->r = *val; break;
				}

				numVariable--;
			}
			else if (command == "AMBIENT")
			{
				switch (numVariable)
				{
					case 1: param->ambient->b = *val; command = ""; break;
					case 2: param->ambient->g = *val; break;
					case 3: param->ambient->r = *val; break;
				}

				numVariable--;
			}
			else if (command == "SPHERE")
			{
				// Processes each parameter for a sphere object
				switch (numVariable)
				{
					case 1:
						tempSphere->n = static_cast<int>(*val);
						tempSphere->pos.w = 1.0f;
						param->spheres.push_back(tempSphere);
						command = "";
						break;
					case 2: tempSphere->k_refl = *val; break;
					case 3: tempSphere->k_spec = *val; break;
					case 4: tempSphere->k_diff = *val; break;
					case 5: tempSphere->k_a = *val; break;
					case 6: tempSphere->colour.b = *val; break;
					case 7: tempSphere->colour.g = *val; break;
					case 8: tempSphere->colour.r = *val; break;
					case 9: tempSphere->scl.z = *val; break;
					case 10: tempSphere->scl.y = *val; break;
					case 11: tempSphere->scl.x = *val; break;
					case 12: tempSphere->pos.z = *val; break;
					case 13: tempSphere->pos.y = *val; break;
					case 14: tempSphere->pos.x = *val; break;
				}

				numVariable--;
			}
			else if (command == "LIGHT")
			{
				// Processes each parameter for a light object
				switch (numVariable)
				{
					case 1:
						tempLight->intensity.z = *val;
						tempLight->pos.x = 1.0f;
						param->lights.push_back(tempLight);
						command = "";
						break;
					case 2: tempLight->intensity.y = *val; break;
					case 3: tempLight->intensity.x = *val; break;
					case 4: tempLight->pos.z = *val; break;
					case 5: tempLight->pos.y = *val; break;
					case 6: tempLight->pos.x = *val; break;
				}

				numVariable--;
			}
			else { command = ""; }
		}
	}

	// Close the file after reading it
	file.close();

	// If any of the following parameters were not specified during the file IO process
	// The raytracer cannot render the scene thus return a null pointer
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
		return nullptr;
	}
	return param;
}

// Method to convert a float into a shared pointer
std::shared_ptr<float> FileReader::ConvertToFloat(std::string strVal) const
{
	// If the supplied string is not a float, return a null pointer instead
	try { return std::make_shared<float>(std::stof(strVal)); }
	catch (std::invalid_argument e) { return nullptr; }
}
