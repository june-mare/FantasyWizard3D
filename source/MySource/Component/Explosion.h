#pragma once
#include "Component.h"
#include "GameObject/GameObject.h"
#include "2DImage/SpritePriority.h"
#include "Animator2D.h"
#include "GameObject/ObjectManager.h"
#include "../Engine/Engine.h"

class Explosion :public Component
{
public:
	int count = 3;
public:
	Explosion(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "Explosion";
	};
	virtual void Debug()override
	{
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ImGui::SliderInt("count", &count, 1, 5);
		}

	}
	virtual ~Explosion() = default;
	virtual void OnDestroy()override
	{
		/*爆発アニメーションデータ*/
		const Texcoord tc[] =
		{
			MakeTexcoord(320,368,32,32),
			MakeTexcoord(352,368,32,32),
			MakeTexcoord(384,368,32,32),
			MakeTexcoord(416,368,32,32),
			MakeTexcoord(448,368,32,32),
			MakeTexcoord(480,368,32,32),
		};
		/*座標データ*/
		const float x[] = { 0,12,20,-16,12 };
		const float y[] = { 0,20,12,-16,-20 };

		/*爆発を表示*/
		int i = static_cast<int>(std::size(x));
		if (count < i) {
			i = count;
		}
		ObjectManager& manager = ObjectManager::Instance();
		Engine& engine = Engine::Instance();
		for (--i; i >= 0; --i)
		{
			auto explosion = manager.CreateObj<GameObject>("explosion", gameObject->transform.getLocalPosition());
			explosion->AddSpriteTexture(tc[0],engine.GetTexture("resource/textures/image/objects.tga"));
			explosion->layer = SpritePriority::prioExplosion;
			auto anim = explosion->AddComponent<Animator2D>();
			anim->loopTime = false;
			anim->killWhenEnd = true;
			anim->clip.resize(std::size(tc));
			for (size_t frameNo = 0; frameNo < std::size(tc); ++frameNo) {
				anim->AddSpriteTexture(frameNo, tc[frameNo], 1, x[i], y[i]);
			}
		}
	}
};

