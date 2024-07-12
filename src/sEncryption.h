#pragma once
#include <cstdlib>
#include <iostream>


void __cdecl zeroBuffer(char*, int);

constexpr unsigned int LOWER_lowerBound = 65;
constexpr unsigned int LOWER_upperBound = 90;
constexpr unsigned int CAPS_lowerBound = 97;
constexpr unsigned int CAPS_upperBound = 122;
constexpr unsigned int NUMS_lowerBound = 48;
constexpr unsigned int NUMS_upperBound = 57;
constexpr unsigned int SPACE = 32;

bool isValidChar(unsigned char byte)
{
	if ((byte >= LOWER_lowerBound && byte <= LOWER_upperBound) || (byte >= CAPS_lowerBound && byte <= CAPS_upperBound) || (byte >= NUMS_lowerBound && byte <= NUMS_upperBound) || byte==SPACE)
	{
		return true;
	}
	return false;
}
std::vector<std::string> ParseStrings(char* bytes, unsigned int size, unsigned int criteria)
{
	std::vector<std::string> ret;
	unsigned int i, j, criteriaMatched;
	
	bool* mask = new bool[size];
	memset(mask, 0, size);
	for (i = 0; i < size; i++)
	{
		mask[i] = isValidChar(bytes[i]);
	}
	for (i = 0; i < size; i++)
	{
		if (mask[i])
		{
			for (j = 0; i+j<=size; j++)
			{
				if (!mask[i + j])
				{
					break;
				}
			}
			if (j >= criteria)
			{
				std::stringstream strStream;
				strStream << std::hex << i;

				ret.push_back(strStream.str() + std::string(": ") + std::string(bytes + i, j));
				i += j;
			}
		}
	}
	delete[] mask;
	return ret;
}



int main(int argc, char**argv)
{
	if (argc == 1)
	{
		std::cout << "No input\n";
		return 1;
	}

	int sensitivityCriteria;
	std::string blahblah = std::string(argv[2]);
	std::stringstream parser(blahblah);
	parser >> sensitivityCriteria;
	
	char* bytes;
	unsigned long long increment = 0, sizeOnDisk = 0;
	//"C:\\Users\\Falcon\\Downloads\\X670EAORUSMASTER.F32a"
	std::ifstream binary(argv[1], std::ifstream::binary);

	binary.seekg(0, binary.end);
	sizeOnDisk = binary.tellg();
	binary.seekg(0, binary.beg);

	if (!binary.good())
	{
		std::cout << "File has failed to open. Possible bad file path or lack of privilege.\n";
	}

	bytes = new char[sizeOnDisk];
	memset(bytes, 0, sizeOnDisk);

	while (binary.good())
	{
		binary.read(bytes + increment, 4096);
		increment += 4096;
	};

	std::vector<std::string> strs = ParseStrings(bytes, sizeOnDisk, sensitivityCriteria);
	for(int i = 0; i < strs.size(); i++)
	{
		std::cout << strs[i] << "\n";
	}
	
	return 0;
}
