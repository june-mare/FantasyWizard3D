#pragma once
#include <memory>
#include <vector>
#include <string>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "Std3D/StdMesh.h"
#include "2DImage/Texture.h"
#include "2DImage/Sprite.h"
#include "2DImage/TileMap.h"
#include "2DImage/TileMapScroll.h"
#include "GameObject.h"
#include "Collision/Collision3D.h"
#include "Camera.h"
#include "skyBox.h"
#include "Lighting.h"
#include "Material.h"
#include "ObjectSprite.h"


/*
   @breaf スタティックシングルトンクラス想定
　 @note  uiList objectList を保持して関数実行を管理するクラス
*/
class ObjectManager
{
private:
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager* operator=(const ObjectManager&) = delete;
	ObjectManager();
	~ObjectManager();

	struct UILayer {
		TexturePtr tex = nullptr;/*レイヤーの画面に使用するテクスチャ*/
		GameObjectList gameObjectList = {};
		std::vector<Sprite> spriteList = {};
	};

	struct Collision
	{
		std::shared_ptr<BoundingVolume> frustumShape = nullptr;
		GameObjectPtr gameObject = nullptr;
	};

	GameObjectList objectList = {};
	GameObjectList map = {};
	std::vector<UILayer> uiList = {};
	std::vector<Collision> collisionList = {};
	std::vector<std::shared_ptr<Camera>> cameraList = {};
	std::shared_ptr<LightBuffer> lighting = {};	
	std::shared_ptr<MaterialBuffer> materialize = {};
	std::shared_ptr<SkyBox> skybox = {};	

public:
	/*インスタンス取得*/
	static ObjectManager& Instance()noexcept
	{
		static ObjectManager instance;
		return instance;
	}

	void Execute()noexcept;

	void SortToGameObject() {
		auto cmp = [](GameObjectPtr a, GameObjectPtr b)->bool {
			return a->GetName() < b->GetName();
		};
		std::stable_sort(objectList.begin(), objectList.end(), cmp);
	}

	template <typename T>
	GameObjectPtr CreateObj(const std::string& name = "", glm::vec3 position = glm::vec3(0))
	{
		GameObjectPtr gameObject = std::make_shared<T>();
		gameObject->SetName(name);
		gameObject->transform.setLocalPosition(position);
		gameObject->ActorStart();
		objectList.push_back(gameObject);
		return gameObject;
	}

	/*第一引数にAddUILayerがお勧め*/
	template <typename T>
	GameObjectPtr CreateUI(size_t layerNo, const std::string& name,
		glm::vec3 position = glm::vec3(0)) {
		if (layerNo >= uiList.size()) {
			return nullptr;
		}
		std::shared_ptr<GameObject> p = std::make_shared<T>();
		p->SetName(name);
		p->transform.setLocalPosition(position);
		uiList[layerNo].gameObjectList.push_back(p);
		return p;
	}

	std::shared_ptr<Camera> CreateCamera(std::string name, bool active, glm::vec3 position = glm::vec3(0),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = 0.f, float pitch = 0);

	GameObjectList CreateMap(const char* path);
	
	std::shared_ptr<SkyBox> CreateSkyBox(std::vector<std::string> filename);

	void CreateLighting();

	void CreateMaterial();

	std::shared_ptr<Camera> GetActiveCamera()const;

	void CameraActiveChange(std::string name)noexcept;

	size_t AddUILayer(const char* path, size_t reserve = 10, GLenum filterMode = GL_LINEAR)noexcept;

	GameObjectPtr FindWithNameObject(const char* name)const noexcept;

	GameObjectList FindWithNameObjects(const char* name)const noexcept;

	GameObjectPtr FindWithTagObject(const std::string& tag)const noexcept;

	GameObjectList FindWithTagObjects(const std::string& tag)const noexcept;

	GameObjectList GetGameObjectList()noexcept;
	
	GameObjectList GetMap()noexcept;
	
	std::vector<UILayer>& GetUILayer()noexcept;
	
	void BindLighting()noexcept;
	void BindMaterial()noexcept;

	void UnbindLighting()noexcept;
	void UnbindMaterial()noexcept;
	void AddLight(DirectionLight& light)noexcept;
	void AddLight(PointLight& light)noexcept;
	void AddLight(SpotLight& light)noexcept;

	unsigned int AddMaterial(Mesh::Material& material)noexcept;

	std::shared_ptr<SkyBox> GetSkyBox()noexcept;

	void ResetAllObjectList()noexcept;

	void Debug()noexcept;

private:
	void PushCollideList(std::vector<WorldCollider>* worldList, GameObjectList objectList);
	void CollideGameObject();
	void RemoveGameObject(GameObjectList& gameObjectList)noexcept;
	void RemoveCollision()noexcept;
	void MakeSpriteList(GameObjectList& gameObjectList, ObjectSpriteList& spriteLIst);
	void MakeSpriteList(GameObjectList& gameObjectList, SpriteList& spriteLIst);
};
