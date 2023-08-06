#pragma once
#include <Windows.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>

namespace FileSystem
{
	template <typename First, typename Second>
	bool SaveFile(const char* path, const char* subject,
		std::unordered_map<First, Second> list)noexcept
	{
		std::ifstream ifs(path);
		std::vector<std::string> writeList;
		if (!ifs)return false;
		std::string line;
		bool findSubject = false;
		while (std::getline(ifs, line))
		{
			if (std::strcmp(line.c_str(), subject) == 0)
				findSubject = true;
			writeList.push_back(line);
		}
		ifs.close();

		std::ofstream ofs;
		if (findSubject)
		{
			ofs.open(path, std::ios::out);
			if (!ofs)return false;
			bool Overwrite = false;

			for (int itr = 0; itr < writeList.size(); itr++)
			{
				if (!Overwrite)
					ofs << writeList[itr] << "\n";
				if (writeList[itr] == subject)
				{
					Overwrite = true;
					for (auto itr : list)
					{
						ofs << itr.first << " "
							<< itr.second << "\n";
					}
					ofs << "end" << "\n";
				}
				if (writeList[itr] == "end")
				{
					Overwrite = false;
				}
			}
		}
		else
		{
			ofs.open(path, std::ios::app);
			ofs << subject << std::endl;
			for (auto i : list)
			{
				ofs << i.first << " "
					<< i.second << std::endl;
			}
			ofs << "end" << std::endl;
			ofs.close();
		}
		ofs.close();
		return true;
	}
	template <typename T>
	
	bool SaveFile(const char* path, const char* subject,
		std::vector<T> list)noexcept
	{
		std::ifstream ifs(path);
		std::vector<std::string> writeList;
		if (!ifs)return false;
		std::string line;
		bool findSubject = false;
		while (std::getline(ifs, line))
		{
			if (std::strcmp(line.c_str(), subject) == 0)
				findSubject = true;
			writeList.push_back(line);
		}
		ifs.close();

		std::ofstream ofs;
		if (findSubject)
		{
			ofs.open(path, std::ios::out);
			if (!ofs)return false;
			bool Overwrite = false;

			for (int itr = 0; itr < writeList.size(); itr++)
			{
				if (!Overwrite)
					ofs << writeList[itr] << "\n";
				if (writeList[itr] == subject)
				{
					Overwrite = true;
					for (auto itr : list)
					{
						ofs << itr << "\n";
					}
					ofs << "end" << "\n";
				}
				if (writeList[itr] == "end")
				{
					Overwrite = false;
				}
			}
		}
		else
		{
			ofs.open(path, std::ios::app);
			ofs << subject << std::endl;
			for (auto i : list)
			{
				ofs << i << std::endl;
			}
			ofs << "end" << std::endl;
			ofs.close();
		}
		ofs.close();
		return true;
	}

	template <typename First,typename Second>
	bool ReadFile(const char* path, const char* subject,
		std::unordered_map<First, Second>& list)noexcept
	{
		std::ifstream ifs(path);
		if (!ifs)return false;
		std::string line;
		bool findSubject = false;
		while (std::getline(ifs, line))
		{
			if (std::strcmp(line.c_str(),subject) == 0)
			{
				findSubject = true;
				continue;
			}
			if (!findSubject)
				continue;
			if (std::strcmp(line.c_str(), "end") == 0)
				break;

			Second value = {};
			First key = {};
			std::stringstream ss;

			ss << line;
			ss >> key; 
			ss >> value;
			list[key] = value;
		}
		ifs.close();
		return findSubject;
	}

	template <typename T>
	bool ReadFile(const char* path, const char* subject,
		std::vector<T>& list)noexcept
	{
		std::ifstream ifs(path);
		if (!ifs)return false;
		std::string line;
		bool findSubject = false;
		while (std::getline(ifs, line))
		{
			if (std::strcmp(line.c_str(), subject) == 0)
			{
				findSubject = true;
				continue;
			}
			if (!findSubject)
				continue;
			if (std::strcmp(line.c_str(), "end") == 0)
				break;
			T value;
			std::stringstream ss;

			ss << line;
			ss >> value;
			list.push_back(value);
		}
		ifs.close();
		return findSubject;
	}


}