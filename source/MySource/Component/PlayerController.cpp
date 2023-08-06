#include "PlayerController.h"
#include "../GameObject/ObjectManager.h"

void PlayerController::Start() noexcept
{
	walkTimer.SetTimer(0.5f);
	attackTimer.SetTimer(0.8f);
}

void PlayerController::Update() noexcept
{
	Engine& engine = Engine::Instance();
	glm::vec3 velocity(0);

	attackTimer.Execute();
	walkTimer.Execute();

	/*移動キー入力*/
	if (!isAttack) {
		{
			ObjectManager& manager = ObjectManager::Instance();
			auto camera = manager.GetActiveCamera();

			if (walkTimer.TimeUp()) {
				if (InputSystem::GetKey(GLFW_KEY_I)) {
					warking = warking ? false : true;
					walkTimer.Reset();
				}
			}

			if (InputSystem::GetKey("look")) {
				if (InputSystem::GetKey("front")) {
					velocity += camera->Front * move.speed;
				}
				if (InputSystem::GetKey("back")) {
					velocity -= camera->Front * move.speed;
				}
				if (InputSystem::GetKey("right")) {
					velocity += camera->Right * move.speed;
				}
				if (InputSystem::GetKey("left")) {
					velocity -= camera->Right * move.speed;
				}
			}
			else {
				bool isMove = false;
				if (InputSystem::GetKey("front")) {
					velocity += camera->Front * move.speed;
					isMove = true;
				}
				if (InputSystem::GetKey("back")) {
					velocity -= camera->Front * move.speed;
					isMove = true;
				}
				if (InputSystem::GetKey("right")) {
					velocity += camera->Right * move.speed;
					isMove = true;
				}
				if (InputSystem::GetKey("left")) {
					velocity -= camera->Right * move.speed;
					isMove = true;
				}
				if (isMove) {
					if (warking) {
						auto anim = engine.GetAnimation(animUseList->GetAnimID("walkf"));
						gameObject->animator->PlayAnimation(anim.get());
					}
					else {
						auto anim = engine.GetAnimation(animUseList->GetAnimID("runf"));
						gameObject->animator->PlayAnimation(anim.get());
					}
				}
				else {
					auto anim = engine.GetAnimation(animUseList->GetAnimID("idle2"));
					gameObject->animator->PlayAnimation(anim.get());
				}
			}
		}
		/*アクセラレートの計算*/
		{
			if (move.moveVector != velocity) {
				move.Reset();
			}
			else
				move.Acceleration();
			velocity.y = 0.f;
			move.moveVector = velocity;
		}
		if (velocity != glm::vec3(0)) {
			gameObject->transform.lookAt(
				gameObject->transform.getLocalPosition() + velocity,
				gameObject->transform.up);

			auto addPos = move.IncreasePos();
			if (warking)addPos *= 0.25f;
			gameObject->AddPosition(addPos);
		}
	}

	if (attackTimer.TimeUp()) {
		if (InputSystem::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			if (animSetList.empty()) {
				animSetList.push_back(engine.GetAnimation(animUseList->GetAnimID("attack1")));
				gameObject->animator->PlayAnimation(animSetList[0].get());
				CollideCreate(glm::vec3(0.f, 50.f, 100.f));
			}
			else if (animSetList.size() == 1) {
				animSetList.push_back(engine.GetAnimation(animUseList->GetAnimID("slash2")));
			}
			else if (animSetList.size() == 2) {
				animSetList.push_back(engine.GetAnimation(animUseList->GetAnimID("slash3")));
			}
			else if (animSetList.size() == 3) {
				animSetList.push_back(engine.GetAnimation(animUseList->GetAnimID("slash1")));
			}
			attackTimer.Reset();
		}
	}
	if (animSetList.size() != 0) {
		isAttack = true;

		if (gameObject->animator->CheckEndAnim()) {
			comboCount++;			
			gameObject->AddPosition(
				gameObject->animPosition);
			gameObject->animPosition = glm::vec3(0.f);
			if (comboCount >= animSetList.size()) {
				comboCount = 0;
				isAttack = false;
				animSetList.clear();
				animSetList.shrink_to_fit();
				auto anim = engine.GetAnimation(animUseList->GetAnimID("idle1"));
				gameObject->animator->PlayAnimation(anim.get());
			}
			else {
				gameObject->animator->PlayAnimation(animSetList[comboCount].get());
				CollideCreate(glm::vec3(0.f, 50.f, 100.f));
			}
		}
	}

	/*ほんで攻撃どうする?????*/
}

void PlayerController::CollideCreate(glm::vec3 point)
{
	auto collision = gameObject->AddCollision<BoxCollider>();
	collision->box.center = point;
	collision->box.extent = glm::vec3(80.f, 30.f, 80.f);
	collision->SetOnceCheck(true);
	collision->SetTag(Collider::Tag::PlAttack);
	collision->SetTimer(0.2f);
	collision->isTrriger = true;
}
