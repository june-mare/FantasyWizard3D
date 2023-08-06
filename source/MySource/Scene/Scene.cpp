#include "Scene.h"
#include "../Engine/Engine.h"
#include "GameObject/ObjectManager.h"
#include "System/InputSystem.h"


int Scene::GetLoadCount()
{
	return loadCount;
}

int Scene::GetLoadSize()
{
	return loadSize;
}

float Scene::GetLoadPercentage()
{
	if (loadSize != 0)
		return static_cast<float>(GetLoadCount()) / static_cast<float>(GetLoadSize());
	return 0.f;
}

void Scene::Loading()
{
	if (!isFirst) {
		SetUp();
		isFirst = true;
		if (!loadModels.empty())
			loadSize += static_cast<int>(loadModels.size());
		if (!loadAnims.empty())
			loadSize += static_cast<int>(loadAnims.size());
		if (!loadMaps.empty())
			loadSize += static_cast<int>(loadMaps.size());
	}

	if (!loadMaps.empty()) {
		ObjectManager& manager = ObjectManager::Instance();
		manager.CreateMap(loadMaps.back().c_str());
		loadMaps.pop_back();
		loadCount++;
	}
	else if (!loadModels.empty()) {
		Engine& engine = Engine::Instance();
		engine.LoadModel(loadModels.back().name, loadModels.back().path);
		loadModels.pop_back();
		loadCount++;
	}
	else if (!loadAnims.empty()) {
		Engine& engine = Engine::Instance();
		engine.CreateAnimation(loadAnims.back().path.c_str(), loadAnims.back().modelName,
			loadAnims.back().loop);
		loadAnims.pop_back();
		loadCount++;
	}

	if (loadSize == loadCount) {
		fadeScreanTimer += Time::deltaTime;
		if (isKeyStart) {
			if (InputSystem::GetKey(GLFW_KEY_ENTER)) {
				isLoad = true;
				Initialize();
			}
		}
		else if(fadeScreanInterval < fadeScreanTimer){
			isLoad = true;
			fadeScreanTimer = 0.f;
			Initialize();
		}
	}
}