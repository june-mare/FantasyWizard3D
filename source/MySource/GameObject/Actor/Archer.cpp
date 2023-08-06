#include "Archer.h"
#include "../Engine/Engine.h"
#include "Component/PlayerController.h"


namespace {
	std::string path = "resource/objects/archer/";
	std::string toPath(const char* str) {
		return path + str;
	}
}


void Archer::ActorStart() noexcept
{
	transform.setLocalScale(glm::vec3(0.01f, 0.01f, 0.01f));
	CreateModel("archer", toPath("archer.dae"));
	SetName("archer");
	SetTag("tagPlayer");
	CreateStatus("resource/txt/Input.txt", "archer");

	if (this->status->GetStatuses().empty()) {
		STATUS::Status status;
		status.SetName("archer");
		status.SetStatusValues(8, "HP", 15.f, "ATK", 10.f, "DEF", 5.f, "LUK", 6.f);
		STATUS::StatusManager::GetInstance().SetOriginalStatus(status);
	}

	Engine& engine = Engine::Instance();
	auto animation =
		engine.CreateAnimation(toPath("animation/Idle01.dae").c_str(), *(models.begin()));
	CreateAnimator(animation.get());

	animUseList = engine.GetAnimUseList("archer");

	if (animUseList->GetName() == "") {

		animUseList->SetAnimUseList("idle1", animation->GetId());

		auto model = *(models.begin());

		unsigned int id = 0;
		id = engine.CreateAnimation(toPath("animation/Idle02.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("idle2", id);
		
		id = engine.CreateAnimation(toPath("animation/Death01.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("death1", id);

		id = engine.CreateAnimation(toPath("animation/Death02.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("death2", id);

		id = engine.CreateAnimation(toPath("animation/Damage.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("damage", id);

		id = engine.CreateAnimation(toPath("animation/WalkF.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("walkF", id);
		
		id = engine.CreateAnimation(toPath("animation/WalkB.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("walkB", id);

		id = engine.CreateAnimation(toPath("animation/WalkR.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("walkR", id);

		id = engine.CreateAnimation(toPath("animation/WalkL.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("walkL", id);

		id = engine.CreateAnimation(toPath("animation/RunF.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("runF", id);

		id = engine.CreateAnimation(toPath("animation/RunB.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("runB", id);

		id = engine.CreateAnimation(toPath("animation/RunR.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("runR", id);

		id = engine.CreateAnimation(toPath("animation/RunL.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("runL", id);

		id = engine.CreateAnimation(toPath("animation/Aim.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("aim", id);

		id = engine.CreateAnimation(toPath("animation/AimWalkF.dae").c_str(),
			model)->GetId();
		animUseList->SetAnimUseList("aimWalkF", id);

		id = engine.CreateAnimation(toPath("animation/Kick.dae").c_str(),
			model,false)->GetId();
		animUseList->SetAnimUseList("kick", id);

		id = engine.CreateAnimation(toPath("animation/Punch.dae").c_str(),
			model,false)->GetId();
		animUseList->SetAnimUseList("punch", id);

		id = engine.CreateAnimation(toPath("animation/Shot.dae").c_str(),
			model,false)->GetId();
		animUseList->SetAnimUseList("shot", id);

		animUseList->SetName("archer");
		engine.SetAnimUseList(animUseList);
	}

	/*コンポーネントの追加*/
	auto controller = AddComponent<PlayerController>();
	controller->move.speed = 2.f;
	controller->move.accIncrease = 0.2f;
	controller->move.accMax = 1.f;

	/*ステートの設定*/

}
