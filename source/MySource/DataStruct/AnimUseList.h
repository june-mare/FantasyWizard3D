#pragma once
#include <unordered_map>
#include <string>

class AnimUseList
{
private:
	std::unordered_map<std::string, unsigned int>
		animUseList = {};
	std::string name = "";
public:
	inline std::unordered_map<std::string, unsigned int>& GetList(){
		return animUseList;
	}

	inline void SetName(std::string name) {
		this->name = name;
	}
	
	inline std::string GetName() {
		return name;
	}

	void SetAnimUseList(std::string key, unsigned int value) {
		animUseList[key] = value;
	}

	unsigned int GetAnimID(std::string key){
		if (animUseList.find(key) != animUseList.end())
			return animUseList[key];
		return 0;
	}	
};