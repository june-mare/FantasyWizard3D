#define _CRT_SECURE_NO_WARNINGS
#include "ObjectManager.h"
#include <fstream>
#include <algorithm>
#include "Shader/Shader.h"
#include "BufferObject/VertexArray.h"
#include "Std3D/StdMesh.h"
#include "2DImage/Texture.h"
#include "2DImage/Sprite.h"
#include "Collision/Collision3D.h"
#include "Debug/Debug.h"
#include "Window/Viewport.h"
#include "../Engine/Engine.h"
#include "Component/LightComponent.h"
#include "Component/MaterialComponent.h"

inline constexpr float radians(float d) { return d / 180.0f * 3.14159265f; }

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
	ResetAllObjectList();
};

void ObjectManager::Execute() noexcept
{
	/*処理中に配列が変化しないようにコピーを生成*/
	std::vector<GameObject*> list(objectList.size());
	std::transform(objectList.begin(), objectList.end(),
		list.begin(), [](const GameObjectPtr& e) {return e.get(); });

	/*マップゲームオブジェクトの追加*/
	const size_t size = list.size();
	list.resize(size + map.size());
	std::transform(map.begin(), map.end(),
		list.begin() + size,
		[](const GameObjectPtr& e) { return e.get(); });

	/*UIレイヤーのゲームオブジェクトのコピーを作成*/
	for (const auto& layer : uiList) {
		/*previous(プレビアス、[直前の・ひとつ前の])*/
		const size_t prevListSize = list.size();
		list.resize(prevListSize + layer.gameObjectList.size());
		std::transform(layer.gameObjectList.begin(), layer.gameObjectList.end(),
			list.begin() + prevListSize,
			[](const GameObjectPtr& e) { return e.get(); });
	}

	for (const auto& itr : list) {
		itr->Start();
	}

	lighting->BeginUpdate();
	materialize->BeginUpdate();

	for (const auto& itr : list) {
		if (!itr->IsDead()) {
			itr->Update();
			itr->ActorUpdate();
		}
	}

	lighting->EndUpdate();
	materialize->EndUpdate();

	/*コライダー計算*/
	CollideGameObject();

	RemoveGameObject(objectList);
	/*コライダーの中のやつも消す*/
	RemoveCollision();
	for (auto& layer : uiList) {
		RemoveGameObject(layer.gameObjectList);
	}

	for (auto& layer : uiList) {
		MakeSpriteList(layer.gameObjectList, layer.spriteList);
	}

}

size_t ObjectManager::AddUILayer(const char* path, size_t reserve, GLenum filterMode)noexcept
{
	Engine& engine = Engine::Instance();
	auto texList = engine.GetTextures();
	TexturePtr tmptex = nullptr;
	if (!texList.empty())
		for (const auto& itr : texList) {
			if (path == itr->GetName().c_str()) {
				tmptex = itr;
			}
		}

	if (tmptex == nullptr) {
		tmptex = engine.CreateTexture(path);
	}
	uiList.push_back({ tmptex });
	uiList.back().gameObjectList.reserve(reserve);
	uiList.back().spriteList.reserve(reserve);
	return uiList.size() - 1;
}

void ObjectManager::ResetAllObjectList() noexcept
{
	objectList.clear();
	uiList.clear();
	cameraList.clear();
	collisionList.clear();
	map.clear();
	skybox.reset();
}


void ObjectManager::Debug() noexcept
{
	auto DebugSpace = []()->void {
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	};

	if (ImGui::CollapsingHeader("Hierarchie"))
	{
		if (ImGui::CollapsingHeader("ActiveCamera"))
		{
			if (!cameraList.empty()) {

				auto camera = GetActiveCamera();

				ImGui::Text(camera->getName().c_str());

				glm::vec3 pos = camera->Position;
				ImGui::DragFloat3("pos", &pos.x);
				camera->Position = pos;

				float yaw = camera->Yaw;
				ImGui::DragFloat("yaw", &yaw);
				camera->Yaw = yaw;

				float pitch = camera->Pitch;
				ImGui::DragFloat("pitch", &pitch);
				camera->Pitch = pitch;

				float zoom = camera->Zoom;
				ImGui::DragFloat("zoom", &zoom);
				camera->Zoom = zoom;

				float speed = camera->getSpeed();
				ImGui::DragFloat("speed", &speed);
				camera->setSpeed(speed);

				camera->UpdateCameraVectors();
			}
		}

		if (ImGui::CollapsingHeader("GameObject")) {

			if (!objectList.empty()) {
				int count = 0;
				std::string privName = "";
				for (auto& itr : objectList) {
					if (privName != itr->GetName()) {
						privName = itr->GetName();
						count = 0;
					}
					else {
						count++;
					}
					auto headerName = itr->GetName() + std::to_string(count);
					if (ImGui::CollapsingHeader(headerName.c_str())) {
						itr->Debug();
						DebugSpace();
					}
				}
			}
		}

		if (ImGui::CollapsingHeader("Map3D")) {
			if (!map.empty())
				for (auto& itr : map) {
					if (ImGui::CollapsingHeader(itr->GetName().c_str())) {
						itr->Debug();
						DebugSpace();
					}
				}
		}

		if (ImGui::CollapsingHeader("UI")) {
			if (!uiList.empty()) {
				for (auto& itr : uiList) {

					ImGui::Text("TextureName", itr.tex->GetName().c_str());
					ImGui::Spacing();

					ImGui::Text("RegistObject");
					for (auto& obj : itr.gameObjectList) {
						ImGui::Text(obj->GetName().c_str());
						obj->Debug();
						DebugSpace();
					}
				}
			}
		}



		if (ImGui::CollapsingHeader("Light")) {
			if (lighting != nullptr) {
				ImGui::InputInt("LightType", &lighting->type);
				if (ImGui::Button("Create", ImVec2(100.f, 50.f))) {
					auto light = CreateObj<GameObject>("light");
					auto data = light->AddComponent<LightComponent>();
					data->type = lighting->type;
				}
			}
		}

		if (ImGui::CollapsingHeader("SkyBox")) {
			if (skybox != nullptr)
				skybox->Debug();
		}
	}
}

std::shared_ptr<Camera> ObjectManager::CreateCamera(std::string name, bool active, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
	auto camera = std::make_shared<Camera>(position, up, yaw, pitch);
	camera->setName(name);
	camera->isActive = active;
	cameraList.push_back(camera);
	return camera;
}

GameObjectList ObjectManager::CreateMap(const char* path)
{
	map.clear();
	map.reserve(500);

	std::ifstream jsonFile(path, std::ios::in);
	if (!jsonFile) {
		LOG_WARNING("%sを開けません", path);
		return {};
	}

	Engine& engine = Engine::Instance();
	std::string line = {};
	while (std::getline(jsonFile, line)) {
		const char* p = line.c_str();
		int readByte = 0;

		// データ行の開始判定
		char name[256];
		if (sscanf(p, R"( { "name" : "%255[^"]" %n)", name, &readByte) != 1) {
			continue; // データ行ではない
		}
		name[255] = '\n';
		p += readByte;

		// 名前以外の要素を読み込む
		char baseMeshName[256];
		if (sscanf(p, R"(, "mesh" : "%255[^"]" %n)", baseMeshName, &readByte) != 1) {
			continue;
		}
		baseMeshName[255] = '\0';
		p += readByte;

		glm::vec3 t(0);
		if (sscanf(p, R"(, "translate" : [ %f, %f, %f ] %n)",
			&t.x, &t.y, &t.z, &readByte) != 3) {
			continue;

		}
		p += readByte;

		glm::vec3 r(0);
		if (sscanf(p, R"(, "rotate" : [ %f, %f, %f ] %n)",
			&r.x, &r.y, &r.z, &readByte) != 3) {
			continue;

		}
		p += readByte;

		glm::vec3 s(1);
		if (sscanf(p, R"(, "scale" : [ %f, %f, %f ] %n)",
			&s.x, &s.y, &s.z, &readByte) != 3) {
			continue;

		}
		p += readByte;

		const std::string meshName = std::string("resource/objects/maps/") + baseMeshName + ".obj";

		/*gameobject作成*/
		auto gameObject = std::make_shared<GameObject>();
		gameObject->SetName(name);
		gameObject->transform.setLocalPosition(t);
		gameObject->transform.setLocalRotation(r);
		gameObject->transform.setLocalScale(s);
		auto model = gameObject->CreateModel(baseMeshName, meshName);
		auto collision = gameObject->AddCollision<BoxCollider>();
		auto aabb = generateAABB(*model.get());
		collision->box.extent = aabb.extent;
		collision->box.center = aabb.center;
		collision->tag = Collider::Tag::Object;
		gameObject->rigid = GameObject::Rigid::Static;
		gameObject->AddComponent<MaterialComponent>();
		map.push_back(gameObject);
	}

	auto cmp = [](GameObjectPtr a, GameObjectPtr b)->bool {
		return a->GetName() < b->GetName();
	};
	std::stable_sort(map.begin(), map.end(), cmp);

	// 作成したゲームオブジェクト配列を返す
	//LOG("3Dモデル配置ファイル%sを読み込みました", path);

	return map;
}

void ObjectManager::CameraActiveChange(std::string name) noexcept
{
	if (cameraList.empty())return;
	bool e = false;
	for (const auto& itr : cameraList)
	{
		if (itr->getName() == name)
		{
			itr->isActive = true;
			e = true;
		}
		else
			itr->isActive = false;
	}

	if (!e)
		cameraList.begin()->get()->isActive = true;
}

std::shared_ptr<SkyBox> ObjectManager::CreateSkyBox(std::vector<std::string> filename)
{
	auto box = std::make_shared<SkyBox>("resource/shader/skybox.vert", "resource/shader/skybox.frag");
	for (auto& itr : filename)
		box->textureNames.push_back(itr);
	skybox = box;
	box.reset();
	return skybox;
}

void ObjectManager::CreateLighting()
{
	if (lighting == nullptr) {
		lighting = LightBuffer::Create(50);
	}
}

void ObjectManager::CreateMaterial()
{
	if (materialize == nullptr) {
		materialize = MaterialBuffer::Create(500);
	}
}

std::shared_ptr<Camera> ObjectManager::GetActiveCamera() const
{
	if (cameraList.empty())return nullptr;

	for (const auto& itr : cameraList) {
		if (itr->isActive) return itr;
	}
	auto firstCamera = cameraList.begin()->get();
	firstCamera->isActive = true;
	return firstCamera->shared_from_this();
}

GameObjectPtr ObjectManager::FindWithNameObject(const char* name) const noexcept
{
	for (auto itr : objectList) {
		if (itr->GetName() == name) {
			return itr->shared_from_this();
		}
	}
	return nullptr;
}

GameObjectList ObjectManager::FindWithNameObjects(const char* name) const noexcept
{
	GameObjectList returnValue;
	returnValue.reserve(objectList.size());

	for (const auto& itr : objectList) {
		if (itr->GetName() == name) {
			returnValue.push_back(itr->shared_from_this());
		}
	}
	return returnValue;
}

GameObjectPtr ObjectManager::FindWithTagObject(const std::string& tag) const noexcept
{
	for (const auto& itr : objectList)
	{
		if (itr->GetTag() == tag)
		{
			return itr->shared_from_this();
		}
	}
	return nullptr;
}

GameObjectList ObjectManager::FindWithTagObjects(const std::string& tag) const noexcept
{
	GameObjectList returnValue;
	returnValue.reserve(objectList.size());

	for (const auto& itr : objectList) {
		if (itr->GetTag() == tag) {
			returnValue.push_back(itr->shared_from_this());
		}
	}
	return returnValue;
}

GameObjectList ObjectManager::GetGameObjectList() noexcept
{
	return objectList;
}

GameObjectList ObjectManager::GetMap() noexcept
{
	return map;
}

std::vector<ObjectManager::UILayer>& ObjectManager::GetUILayer() noexcept
{
	return uiList;
}

void ObjectManager::BindLighting() noexcept
{
	lighting->Bind();
}

void ObjectManager::BindMaterial() noexcept
{
	materialize->Bind();
}

void ObjectManager::UnbindLighting() noexcept
{
	lighting->Unbind();
	//lighting->SwapBuffers();
}

void ObjectManager::UnbindMaterial() noexcept
{
	materialize->Unbind();
	materialize->SwapBuffers();
}

void ObjectManager::AddLight(DirectionLight& light) noexcept
{
	lighting->AddDirectionLight(light);
}
void ObjectManager::AddLight(PointLight& light) noexcept
{
	lighting->AddPointLight(light);
}
void ObjectManager::AddLight(SpotLight& light) noexcept
{
	lighting->AddSpotLight(light);
}

unsigned int ObjectManager::AddMaterial(Mesh::Material& material) noexcept
{
	return materialize->AddMaterial(material);
}

//private

void ObjectManager::MakeSpriteList(GameObjectList& gameObjectList, ObjectSpriteList& spriteList)
{
	/*既存の動く物体用のスプライト配列を空にする*/
	spriteList.clear();

	/*優先順位が大きい順番に並び替える*/
	std::stable_sort(
		gameObjectList.begin(), gameObjectList.end(),
		[](const GameObjectPtr& a, const GameObjectPtr& b) {
			return a->layer > b->layer;
		}
	);

	/*同時にワールド座標へ変換する*/
	for (const auto& e : gameObjectList) {
		for (const auto& sprite : e->spriteList) {
			spriteList.push_back(sprite);
			Sprite& s = spriteList.back().spriteList.back();

			auto tmp = e->transform.getLocalPosition();
			s.x += tmp.x;
			s.y += tmp.y;

			e->transform.setLocalPosition(tmp);
		}
	}
}

std::shared_ptr<SkyBox> ObjectManager::GetSkyBox() noexcept
{
	return skybox;
}

void ObjectManager::MakeSpriteList(GameObjectList& gameObjectList, SpriteList& spriteList)
{
	/*既存の動く物体用のスプライト配列を空にする*/
	spriteList.clear();

	/*優先順位が大きい順番に並び替える*/
	std::stable_sort(
		gameObjectList.begin(), gameObjectList.end(),
		[](const GameObjectPtr& a, const GameObjectPtr& b) {
			return a->layer > b->layer;
		}
	);

	/*同時にワールド座標へ変換する*/
	for (const auto& e : gameObjectList) {
		for (const auto& sprite : e->spriteList) {
			for (const auto& sd : sprite.spriteList) {
				spriteList.push_back(sd);
				Sprite& s = spriteList.back();
				auto tmp = e->transform.getLocalPosition();
				s.x += tmp.x;
				s.y += tmp.y;

				e->transform.setLocalPosition(tmp);
			}
		}
	}
}

void ObjectManager::PushCollideList(std::vector<WorldCollider>* worldList, GameObjectList objectList)
{
	for (auto obj : objectList) {
		if (obj->IsDead() || !obj->IsStarted())
			continue;
		const auto& local = obj->colliderList;

		if (local.empty())
			continue;

		WorldCollider world;
		world.gameObject = obj;
		world.colliderList.resize(local.size());
		for (size_t i = 0; i < local.size(); ++i) {
			if (obj->HasAnimator()) {
				Transform tramsform = obj->transform;
				tramsform.setLocalPosition(
					tramsform.getLocalPosition() + obj->animPosition);
				world.colliderList[i] =
					local[i]->GetTransformedCollider(tramsform);
			}
			else {
				world.colliderList[i] =
					local[i]->GetTransformedCollider(obj->transform);
			}
			world.colliderList[i]->SetTag(local[i]->GetTag());
			world.colliderList[i]->IsTrriger(local[i]->IsTrriger());
			world.colliderList[i]->SetOnceCheck(local[i]->GetOnceCheck());
		}
		worldList->push_back(world);
	}
}

void ObjectManager::CollideGameObject()
{
	std::vector<WorldCollider> worldList;
	worldList.reserve(objectList.size() + map.size());

	PushCollideList(&worldList, objectList);
	PushCollideList(&worldList, map);

	for (auto pa = worldList.begin(); pa != worldList.end(); ++pa)
	{
		for (auto pb = pa + 1; pb != worldList.end(); ++pb)
		{
			if (pb->gameObject->IsDead()) {
				continue;	/*衝突しない*/
			}

			auto tagList = pa->DetectCollision(*pb);
			if (!tagList.empty()) {

				for (auto& itr : pa->gameObject->colliderList){
					if(std::find(tagList.begin(), tagList.end(), itr->GetTag())
						!= tagList.end()) {
						if (itr->GetOnceCheck()) {
							itr->OnceCollide(true);
						}
					}	
				}
				for (auto& itr : pb->gameObject->colliderList) {
					if (std::find(tagList.begin(), tagList.end(), itr->GetTag())
						!= tagList.end()) {
						if (itr->GetOnceCheck()) {
							itr->OnceCollide(true);
						}
					}
				}

				/*衝突しているので、しょとつイベント関数を実行*/
				pa->gameObject->OnCollision(*pb->gameObject, tagList);
				pb->gameObject->OnCollision(*pa->gameObject, tagList);

				if (pa->gameObject->IsDead()) {
					break;		//paが死んでいるならループを打ち切る
				}
			}
			else {
				pa->gameObject->OutCollision();
				pb->gameObject->OutCollision();
			}
		}
	}
}

void ObjectManager::RemoveGameObject(GameObjectList& gameObjectList) noexcept
{
	/*生きているオブジェクトと死んでいるオブジェクトに分ける*/
	const auto i = std::stable_partition(
		gameObjectList.begin(), gameObjectList.end(),
		[](const GameObjectPtr& e) {return !e->IsDead(); }
	);

	if (i == gameObjectList.end())
		return;

	/*死んでいるオブジェクトを別の配列に移動*/
	GameObjectList deadList(
		std::make_move_iterator(i),
		std::make_move_iterator(gameObjectList.end()));

	/*配列から移動済みのオブジェクトを削除*/
	gameObjectList.erase(i, gameObjectList.end());

	/*死んでいるオブジェクトに対してDestroyイベントを実行*/
	for (auto& e : deadList) {
		e->OnDestroy();
	}
	/*ここで死んでいるオブジェクトに対しが削除される*/
}

void ObjectManager::RemoveCollision()noexcept
{
	collisionList.erase(
		std::remove_if(collisionList.begin(), collisionList.end(),
			[](const Collision& e) {return e.gameObject->IsDead(); })
		, collisionList.end());
}