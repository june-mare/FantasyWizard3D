#pragma once
#include "Component.h"
#include "../GameObject/GameObject.h"
#include <string>
#include "../Engine/Engine.h"

/*テキストコンポーネント*/
class Text :public Component
{
public:
	std::string text;
	float scale = 1;
public:
	Text(GameObjectPtr gameObject)
		:Component(gameObject) { name = "text"; };
	virtual ~Text() = default;

	virtual void Update()override {
		Engine& engine = Engine::Instance();
		const float fontSizeX = 16 * scale;
		const float fontSizeY = 32 * scale;
		gameObject->spriteList.clear();
		auto tex = engine.GetTexture("resource/textures/image/font.tga");
		for (size_t i = 0; i < std::size(text); ++i) {
			const char c = text[i];
			gameObject->AddSpriteTexture(
				{
				static_cast<float>(c % 16) / 16,
				static_cast<float>(c / 16) / 8,
				1.0f / 16.0f,1.0f / 8.0f
				},
				tex,
				fontSizeX * (0.5f + static_cast<float>(i)),
				fontSizeY * 0.5f, scale);
		}
	}
	virtual void Debug()override
	{
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ImGui::Text(text.c_str());
			ImGui::DragFloat("scale", &scale);
		}
	}

	void SetText(const std::string& t, float s = 1) {
		text = t;
		scale = s;
	}
};
