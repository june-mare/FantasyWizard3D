#include "Paladin.h"
#include "../Engine/Engine.h"
#include "Component/ComponentInclude.h"
#include "GameObject/ObjectManager.h"

namespace{
	std::string path = "resource/objects/paladin/";
	std::string toPath(const char* str) {
		return path + str;
	}
}

void Paladin::ActorStart()noexcept {
	transform.setLocalScale(glm::vec3(0.01f, 0.01f, 0.01f));
	CreateModel("paladin",toPath("Paladin.dae"));
	SetName("paladin");
	SetTag("tagEnemy");
	CreateStatus("resource/txt/Input.txt", "paladin");

	if (this->status->GetStatuses().empty()) {
		STATUS::Status status;
		status.SetName("paladin");
		status.SetStatusValues(8, "HP", 15.f, "ATK", 10.f, "DEF", 5.f, "LUK", 6.f);
		STATUS::StatusManager::GetInstance().SetOriginalStatus(status);
	}

	Engine& engine = Engine::Instance();
	auto animation =
		engine.CreateAnimation(toPath("animation/Idle01.dae").c_str(), *(models.begin()));
	CreateAnimator(animation.get());

	animUseList = engine.GetAnimUseList("paladin");
	animator->SetBaseBoneName("mixamorig_Hips");
	if (animUseList->GetName() == "") {
		auto model = *(models.begin());
		unsigned int id = 0;

		id = animation->GetId();
		animUseList->SetAnimUseList("idle1", id);
		
		id = engine.CreateAnimation(toPath("animation/Idle02.dae").c_str(),model)->GetId();
		animUseList->SetAnimUseList("idle2", id);
		
		id = engine.CreateAnimation(toPath("animation/Idle03.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("idle3", id);

		id = engine.CreateAnimation(toPath("animation/Death01.dae").c_str(), model,false)->GetId();
		animUseList->SetAnimUseList("death1", id);

		id = engine.CreateAnimation(toPath("animation/Death02.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("death2", id);

		id = engine.CreateAnimation(toPath("animation/Slash01.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("slash1", id);
		
		id = engine.CreateAnimation(toPath("animation/Slash02.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("slash2", id);

		id = engine.CreateAnimation(toPath("animation/Slash03.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("slash3", id);

		id = engine.CreateAnimation(toPath("animation/Slash04.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("slash4", id);

		id = engine.CreateAnimation(toPath("animation/RunR.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("runr", id);

		id = engine.CreateAnimation(toPath("animation/RunL.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("runl", id);

		id = engine.CreateAnimation(toPath("animation/WalkR.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("walkr", id);

		id = engine.CreateAnimation(toPath("animation/WalkL.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("warkl", id);

		id = engine.CreateAnimation(toPath("animation/Attack01.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("attack1", id);
		
		id = engine.CreateAnimation(toPath("animation/Attack02.dae").c_str(), model,false)->GetId();
		animUseList->SetAnimUseList("attack2", id);

		id = engine.CreateAnimation(toPath("animation/Attack03.dae").c_str(), model,false)->GetId();
		animUseList->SetAnimUseList("attack3", id);

		id = engine.CreateAnimation(toPath("animation/Attack04.dae").c_str(), model,false)->GetId();
		animUseList->SetAnimUseList("attack4", id);

		id = engine.CreateAnimation(toPath("animation/Block.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("block", id);

		id = engine.CreateAnimation(toPath("animation/Casting.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("casting", id);

		id = engine.CreateAnimation(toPath("animation/Damage01.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("damage1", id);

		id = engine.CreateAnimation(toPath("animation/Damage02.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("damage2", id);

		id = engine.CreateAnimation(toPath("animation/Kick01.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("kick1", id);

		id = engine.CreateAnimation(toPath("animation/PowerUp.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("powerup", id);

		id = engine.CreateAnimation(toPath("animation/RunB.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("runb", id);

		id = engine.CreateAnimation(toPath("animation/RunF.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("runf", id);

		id = engine.CreateAnimation(toPath("animation/WalkF.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("walkf", id);

		animUseList->SetName("paladin");
		engine.SetAnimUseList(animUseList);
	}

	/*コンポーネントの追加*/
	auto collision = AddCollision<BoxCollider>();	
	collision->box.extent = glm::vec3(25.f,50.f,25.f);
	collision->box.center = glm::vec3(0.f, 50.f, 0.f);
	collision->SetTag(Collider::Tag::Player);
	AddComponent<MaterialComponent>();
	auto controller = AddComponent<PlayerController>();
	controller->animUseList = animUseList;
	controller->move.speed = 5.f;
	controller->move.accIncrease = 0.2f;
	controller->move.accMax = 2.f;

	AddComponent<DamageSource>()->tag = Collider::Tag::EnAttack;
	AddComponent<Falling>();
}

void Paladin::ActorUpdate() noexcept
{
	ObjectManager& manager = ObjectManager::Instance();
	manager.GetActiveCamera()->LookAtPosition(animPosition + transform.getLocalPosition());

}