#include "ClearTimeManager.h"
#include <fstream>

void ClearTimeManager::ClearTime(int num, float time)
{
	if (clearTimes.find(num) != clearTimes.end()){
		if (clearTimes[num] < time) {
			return;
		}
	}
	clearTimes[num] = time;
}

float ClearTimeManager::ClearTime(int num)
{
	return clearTimes.at(num);
}

void ClearTimeManager::SaveTimes() noexcept
{
	std::ofstream file;
	file.open("resource/dat/xxxx.dat", std::ios::out);
	
	for (auto itr : clearTimes)	{
		std::string writeText = " map " + std::to_string(itr.first);
		file << writeText << std::endl;
		writeText.clear();
		writeText = " time " + std::to_string(itr.second);
		file << writeText << std::endl;
	}
	file.close();
}

void ClearTimeManager::LoadTimes() noexcept
{
	std::ifstream file;
	file.open("resource/dat/xxxx.dat", std::ios::in);
	int count;
	while (!file.eof()){
		
		std::string line;
		std::getline(file, line);

		if (sscanf_s(line.data(), " map %x", &count) == 1){
			continue;
		}
		float time = 0;
		if (sscanf_s(line.data(), " time %f", &time) == 1)
		{
			clearTimes[count] = time;
		}		
	}
}
