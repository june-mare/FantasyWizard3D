#include "ObjectSprite.h"

ObjectSprite FindObjectSprite(std::string name,std::vector<ObjectSprite>& list)
{
	for (auto data : list){
		if (data.name == name){
			return data;
		}
	}
	return ObjectSprite();
}
