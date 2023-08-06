#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Model.h"
class Scene;
using ScenePtr = std::shared_ptr<Scene>;

/*シーンの基底クラス*/
class Scene
{	
	bool isFirst = false;
	float fadeScreanInterval = 1.f;
	float fadeScreanTimer = 0.f;
protected:
	bool isKeyStart = false;
	std::vector<std::string> loadMaps{};
	struct LoadModelPath {
		LoadModelPath(std::string name,std::string path):
			name(name), path(path){}
		std::string name{};
		std::string path{};
	};
	std::vector<LoadModelPath> loadModels;
	struct LoadAnimationPath {	
		LoadAnimationPath(std::string path, std::string modelName, bool loop) :
			path(path), modelName(modelName), loop(loop) {}
		std::string path{};
		std::string modelName{};
		bool loop = false;
	};
	std::vector<LoadAnimationPath> loadAnims;
	int loadCount = 0;
	int loadSize = 0;
public:
	int GetLoadCount();
	int GetLoadSize();
	float GetLoadPercentage();
	bool isLoad = false;
	Scene() = default;
	virtual ~Scene() = default;
	virtual void SetUp(){};
	void Loading();
	virtual bool Initialize() { return true; }
	virtual void Execute(){};
	virtual void Finalize() {};
	virtual void Debug() {};
};
