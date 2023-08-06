#include "VillageMan.h"
#include "../Engine/Engine.h"

namespace {
	std::string path = "resource/objects/mans/villageman01/";
	std::string toPath(const char* str) {
		return path + str;
	}
}

void VillageMan::ActorStart() noexcept
{
	transform.setLocalScale(glm::vec3(0.01f, 0.01f, 0.01f));
	CreateModel("villageMan", toPath("village_man_01.dae"));
	SetName("mans");
	SetTag("tagEnemy");
	CreateStatus("resource/txt/Input.txt", "villageMan");

	if (this->status->GetStatuses().empty()) {
		STATUS::Status status;
		status.SetName("villageMan");
		status.SetStatusValues(8, "HP", 15.f, "ATK", 10.f, "DEF", 5.f, "LUK", 6.f);
		STATUS::StatusManager::GetInstance().SetOriginalStatus(status);
	}

	Engine& engine = Engine::Instance();

	auto animation =
		engine.CreateAnimation(toPath("animation/Walking.dae").c_str(),
			*(models.begin()));
	CreateAnimator(animation.get());

	animUseList = engine.GetAnimUseList("villageMan");

	if (animUseList->GetName() == "") {

		animUseList->SetAnimUseList("walk", animation->GetId());
		
		auto model = *(models.begin());

		unsigned int id = 0;
		id = engine.CreateAnimation(toPath("animation/WarriorIdle.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("warrior", id);
		
		id = engine.CreateAnimation(toPath("animation/SadIdle.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("sad", id);
		
		id = engine.CreateAnimation(toPath("animation/NeutralIdle.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("neutral", id);
		
		animUseList->SetName("villageMan");
		engine.SetAnimUseList(animUseList);
	}

}

#include "System/InputSystem.h"
void VillageMan::Update() noexcept
{
	if (InputSystem::GetKey("front")) {
		if (animUseList->GetAnimID("walk") !=
			animator->GetAnimID()) {
			Engine& engine = Engine::Instance();
			animator->PlayAnimation(
				engine.GetAnimation(animUseList->GetAnimID("walk")).get()
			);
		}
	}

	if (!InputSystem::AnyKeyDown()) {
		if (animUseList->GetAnimID("neutral") !=
			animator->GetAnimID()) {
			Engine& engine = Engine::Instance();
			animator->PlayAnimation(
				engine.GetAnimation(animUseList->GetAnimID("neutral")).get()
			);
		}
	}

	for (const auto& itr : componentList) {
		itr->Update();
	}
	auto name = this->GetName();
	if (HasAnimator() && this->isActive)
		animator->UpdateAnimation();

}
