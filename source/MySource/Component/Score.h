#pragma once
#include "Component.h"
#include "ScoreManager.h"

class Score :public Component
{
public:
	int score = 100;
	ScoreManagerPtr scoreManager;
public:
	Score(GameObjectPtr gameObject) :Component(gameObject)
	{ name = "Score"; };
	virtual ~Score() = default;
	virtual void OnDestroy()override {
		if (scoreManager) {
			scoreManager->score += score;
		}
	}
};