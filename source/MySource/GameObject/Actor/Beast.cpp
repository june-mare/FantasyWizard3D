#include "Beast.h"
#include "../Engine/Engine.h"
#include "Component/ComponentInclude.h"
#include "System/InputSystem.h"

namespace
{
	std::string path = "resource/objects/beast/";
	std::string toPath(const char* str) {
		return path + str;
	}
}

void Beast::ActorStart() noexcept
{
	transform.setLocalScale(glm::vec3(0.03f, 0.03f, 0.03f));
	CreateModel("beast", toPath("beast.dae"));
	SetName("beast");
	SetTag("tagEnemy");
	CreateStatus("resource/txt/Input.txt", "beast");

	if (this->status->GetStatuses().empty()) {
		STATUS::Status status;
		status.SetName("beast");
		status.SetStatusValues(8, "HP", 80.f, "ATK", 15.f, "DEF", 5.f, "LUK", 2.f);
		STATUS::StatusManager::GetInstance().SetOriginalStatus(status);
	}

	Engine& engine = Engine::Instance();
	auto animation =
		engine.CreateAnimation(toPath("animation/Idle01.dae").c_str(), *(models.begin()));
	CreateAnimator(animation.get());

	animUseList = engine.GetAnimUseList("beast");
	animator->SetBaseBoneName("mixamorig_Hips");

	if (animUseList->GetName() == "") {
		auto model = *(models.begin());
		unsigned int id = 0;

		id = animation->GetId();
		animUseList->SetAnimUseList("idle1", id);

		id = engine.CreateAnimation(toPath("animation/Idle02.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("idle2", id);

		id = engine.CreateAnimation(toPath("animation/Attack01.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("attack1", id);

		id = engine.CreateAnimation(toPath("animation/Attack02.dae").c_str(), model, false)->GetId();
		animUseList->SetAnimUseList("attack2", id);

		id = engine.CreateAnimation(toPath("animation/Attack03.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("attack3", id);

		id = engine.CreateAnimation(toPath("animation/Attack04.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("attack4", id);

		id = engine.CreateAnimation(toPath("animation/Bark01.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("bark1", id);

		id = engine.CreateAnimation(toPath("animation/Bark02.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("bark2", id);

		id = engine.CreateAnimation(toPath("animation/Death.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("death", id);

		id = engine.CreateAnimation(toPath("animation/RunF.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("runf", id);

		id = engine.CreateAnimation(toPath("animation/TurnL.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("turnl", id);

		id = engine.CreateAnimation(toPath("animation/TurnR.dae").c_str(), model)->GetId();
		animUseList->SetAnimUseList("turnr", id);

		animUseList->SetName("beast");
		engine.SetAnimUseList(animUseList);
	}

	auto collision = AddCollision<BoxCollider>();
	auto aabb = generateAABB(*models.begin()->get());
	collision->box.extent = aabb.extent;
	collision->box.center = aabb.center;
	collision->tag = Collider::Tag::Enemy;
	auto material = AddComponent<MaterialComponent>();
	material->material.baseColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
	AddComponent<DamageSource>()->tag = Collider::Tag::PlAttack;
	AddComponent<Falling>();
}