#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"

class DamageSource
	:public Component
{
public:
	//ダメージを受けるタグ
	Collider::Tag tag = Collider::Tag::PlAttack;

	DamageSource(GameObjectPtr gameObject) :Component(gameObject) {
		name = "DamageSource";
	}

	void OnCollision(GameObject& other, std::vector<Collider::Tag> tagList) {
		for (auto itr : tagList) {
			if (itr == tag) {
				auto enemyStatus = other.GetStatus();
				float atk = enemyStatus->GetStatusValue("ATK");
				auto playerStatus = gameObject->GetStatus();
				float def = playerStatus->GetStatusValue("DEF");
				Damage damage;
				damage.amount = (atk - (def * 0.1f));
				gameObject->TakeDamage(other, damage);
			}
		}
	}

	void OnTakeDamage(GameObject& other, const Damage& damge)override {
		auto playerStatus = gameObject->GetStatus();
		float hp = playerStatus->GetStatusValue("HP");
		playerStatus->SetStatusValue("HP", hp - damge.amount);
	}
};