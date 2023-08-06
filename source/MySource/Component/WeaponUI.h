#pragma once
#include "GameObject/GameObject.h"
#include "Component.h"
#include "2DImage/Sprite.h"
#include "../Engine/Engine.h"

class WeaponUI :public Component
{
public:
	//std::shared_ptr<AttachPlayer> player;
	int type = 0, oldType = 0;

	const Texcoord tcList[8] =
	{
		MakeTexcoord(0,0,32,132,257,133),
		MakeTexcoord(32,0,32,132,257,133),
		MakeTexcoord(64,0,32,132,257,133),
		MakeTexcoord(96,0,32,132,257,133),
		MakeTexcoord(128,0,32,132,257,133),
		MakeTexcoord(160,0,32,132,257,133),
		MakeTexcoord(192,0,32,132,257,133),
		MakeTexcoord(224,0,32,132,257,133),
	};
	WeaponUI(GameObjectPtr gameObject) :Component(gameObject) { name = "WeaponUI"; }
	virtual ~WeaponUI() = default;

	virtual void Start()override
	{
		Engine& engine = Engine::Instance();
		//player = gameObject->engine->FindWithGameObjectName("player")->GetConponent<AttachPlayer>();
		auto tex = engine.GetTexture("resource/image/WeaponUI.tga");
		gameObject->AddSpriteTexture(tcList[0], tex, 0, 0, 1);
	}

	virtual void Update()override
	{
		if (oldType != type)
		{
			Engine& engine = Engine::Instance();
			gameObject->spriteList.clear();
			auto tex = engine.GetTexture("resource/image/WeaponUI.tga");
			gameObject->AddSpriteTexture(tcList[type], tex, 0, 0, 1);
			oldType = type;
		}
	}
};