#pragma once
#include "ComponentInclude.h"
#include "GameObject/GameObject.h"
#include "2DImage/EffectList.h"

class EndEffect:
	public Component
{
public:
	int count = 1;
	int size = 1;

	EndEffect(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "EndEffect";
	}
	virtual ~EndEffect() = default;

	virtual void OnDestroy()override
	{
		const float x[] = { 0.0f,size * 16.0f,size * 20.0f,size * -24.0f,size * 24.0f };
		const float y[] = { 0.0f,size * 20.0f,size * 12.0f,size * -20.0f,size * -30.0f };

		Engine& engine = Engine::Instance();
		ObjectManager& manager = ObjectManager::Instance();

		/*”š”­‚ð•\Ž¦*/
		int i = static_cast<int>(std::size(x));
		if (count < i) {
			i = count;
		}
		for (--i; i >= 0; --i)
		{
			auto tc = EffectTCList<Effect1>();

			auto explosion = manager.CreateObj<GameObject>(
				"explosion",gameObject->transform.getLocalPosition());
			explosion->AddSpriteTexture(tc[0],engine.GetTexture("resource/textures/image/objects.tga"));
			explosion->layer = SpritePriority::prioExplosion;
			auto anim = explosion->AddComponent<Animator2D>();
			anim->loopTime = false;
			anim->killWhenEnd = true;
			anim->clip.resize(std::size(tc));
			for (size_t frameNo = 0; frameNo < std::size(tc); ++frameNo) {
				anim->AddSpriteTexture(frameNo, tc[frameNo], static_cast<float>(size), x[i], y[i]);
			}
		}		
	}

};


