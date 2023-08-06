#include "VillageScene.h"
#include "Time/Time.h"
#include "GameObject/ObjectManager.h"
#include "GameObject/Actor/VillageMan.h"
#include "GameObject/Actor/Archer.h"
#include "GameObject/Actor/Paladin.h"
#include "GameObject/Actor/Beast.h"
#include "Component/ComponentInclude.h"
#include "System/InputSystem.h"

bool VillageScene::Initialize()
{
	Engine& engine = Engine::Instance();
	engine.StartFadeIn();
	ObjectManager& manager = ObjectManager::Instance();
	manager.ResetAllObjectList();
	
	//manager.CreateObj<Paladin>("");
	auto dirLight = manager.CreateObj<GameObject>("DirLight");
	auto light = dirLight->AddComponent<LightComponent>();
	light->type = 0;

	//const size_t bgLayer = manager.AddUILayer("resource/textures/image/title.png", 10, GL_LINEAR);
	//auto uiBackground = manager.CreateUI<GameObject>(
	//	bgLayer, "title_bg", glm::vec3(960.f, 640.f, 0.f));
	//uiBackground->AddSpriteTexture({ 0,0,1,1 }, 0, 0, 0.0f, 1.0);

	auto player = manager.CreateObj<Paladin>("",glm::vec3(0.f,2.5f,20.f));
	manager.CreateObj<Beast>("",glm::vec3(0.f,-0.74f,0.f));
	//manager.CreateObj<Archer>("", glm::vec3(5.f, 0.f, 0.0f));
	//manager.CreateObj<VillageMan>("", glm::vec3(5.f, 0.f, -3.0f));
	//manager.CreateObj<Archer>("", glm::vec3(5.f, 0.f, 3.0f));
	//manager.CreateObj<VillageMan>("", glm::vec3(3.f, 0.f, 0.0f));
	//manager.CreateObj<Archer>("", glm::vec3(-5.f, 0.f, 0.0f));
	//manager.CreateObj<VillageMan>("", glm::vec3(-5.f, 0.f, -3.0f));
	//manager.CreateObj<Archer>("", glm::vec3(-5.f, 0.f, 3.0f));
	//manager.CreateObj<VillageMan>("", glm::vec3(-3.f, 0.f, 0.0f));
	//manager.CreateObj<Archer>("", glm::vec3(-3.f, 0.f, 5.0f));
	//manager.CreateObj<VillageMan>("", glm::vec3(-3.f, 0.f, -5.0f));
	//manager.CreateObj<Archer>("", glm::vec3(3.f, 0.f, -5.0f));
	//manager.CreateObj<VillageMan>("", glm::vec3(3.f, 0.f, 5.0f));

	/*スカイボックス生成*/
	manager.CreateSkyBox(
		//スカイボックステキスチャ
		{
			"resource/textures/skybox/right.jpg",
			"resource/textures/skybox/left.jpg",
			"resource/textures/skybox/top.jpg",
			"resource/textures/skybox/bottom.jpg",
			"resource/textures/skybox/front.jpg",
			"resource/textures/skybox/back.jpg"
		}
	)->SetUp();

	/*カメラ生成*/
	auto camera = manager.CreateCamera("main", true, glm::vec3(0.f, 2.5f, 25.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f);
	manager.CreateCamera("sub", false, glm::vec3(0.f, 15.f, 20.f), glm::vec3(0.f, 1.f, 0.f), 15.f, 0.f);	
	camera->LookAtPosition(player->transform.getLocalPosition());
	manager.CreateMap("resource/objects/maps/map1.json");
	manager.CreateLighting();

	return false;
}

void DebugScene() {

	static int chara = 0;
	static glm::vec3 posision = {};
	ImGui::Begin("CharacterSpawn");

	ImGui::InputInt("CharaType", &chara);
	ImGui::InputFloat3("position", &posision[0]);


	ObjectManager& manager = ObjectManager::Instance();
	if (ImGui::Button("Spawn", ImVec2(100.f, 50.f))) {
		switch (chara)
		{
		case 1:
			manager.CreateObj<Archer>("", posision);
			break;
		case 2:
			manager.CreateObj<Paladin>("", posision);
			break;
		case 3:
			manager.CreateObj<Beast>("", posision);
			break;
		default:
			manager.CreateObj<VillageMan>("", posision);
			break;
		}
	}


}

void VillageScene::Debug() {
	DebugScene();
	ImGui::InputInt("LoadSize", &loadSize);
	ImGui::InputInt("Count ",&loadCount);
	ImGui::End();
}



void VillageScene::Execute()
{

	ObjectManager& manager = ObjectManager::Instance();
	
	/*カメラ切り替えテスト*/
	if (InputSystem::GetKey(GLFW_KEY_LEFT_CONTROL)) {
		if (!isSwich)manager.CameraActiveChange("sub");
		else manager.CameraActiveChange("main");
		if (timer > interval) {
			isSwich = isSwich ? false : true;
			timer = 0.f;
		}

		timer += Time::deltaTime;
	}

	auto camera = manager.GetActiveCamera();
	if (camera != nullptr) {

		camera->setSpeed(10.f);

		/*カメラ移動*/
		/*if (InputSystem::GetKey("front"))
			camera->ProcessKeyboard(Camera_Movement::FORWARD);
		if (InputSystem::GetKey("back"))
			camera->ProcessKeyboard(Camera_Movement::BACKWARD);
		if (InputSystem::GetKey("right"))
			camera->ProcessKeyboard(Camera_Movement::RIGHT);
		if (InputSystem::GetKey("left"))
			camera->ProcessKeyboard(Camera_Movement::LEFT);	*/
	}
}

/*ロードするもの*/
void VillageScene::SetUp()
{
	//isKeyStart = true;
	loadModels.push_back(LoadModelPath("cube","resource/objects/default/cube.obj"));
	loadModels.push_back(LoadModelPath("capsule","resource/objects/default/capsule.obj"));
	loadModels.push_back(LoadModelPath("cylinder","resource/objects/default/cylinder.obj"));
	loadModels.push_back(LoadModelPath("sphere","resource/objects/default/sphere.obj"));
	loadModels.push_back(LoadModelPath("archer","resource/objects/archer/archer.dae"));
	loadModels.push_back(LoadModelPath("paladin","resource/objects/paladin/Paladin.dae"));
	loadModels.push_back(LoadModelPath("beast","resource/objects/beast/beast.dae"));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Idle01.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Idle02.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Death01.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Death02.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Damage.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/WalkF.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/WalkB.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/WalkR.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/WalkL.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/RunL.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/RunR.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/RunF.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/RunB.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Aim.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/AimWalkF.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Kick.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Punch.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/archer/animation/Shot.dae", "archer",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Idle01.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Idle02.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Idle03.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Death01.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Death02.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Slash01.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Slash02.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Slash03.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Slash04.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/RunR.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/RunL.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/WalkR.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/WalkL.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Attack01.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Attack02.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Attack03.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Attack04.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Block.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Casting.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Damage01.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Damage02.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/Kick01.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/PowerUp.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/RunB.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/RunF.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/paladin/animation/WalkF.dae", "paladin",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Idle01.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Idle02.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Attack01.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Attack02.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Attack03.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Attack04.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Bark01.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Bark02.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/Death.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/RunF.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/TurnL.dae", "beast",true));
	loadAnims.push_back(LoadAnimationPath("resource/objects/beast/animation/TurnR.dae", "beast",true));
	loadMaps.push_back("resource/objects/maps/map1.json");
}