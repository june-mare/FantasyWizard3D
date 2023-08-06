#include "Gage.h"
#include "../Engine/Engine.h"

void Gage::Start()
{
	Engine& engine = Engine::Instance();
	sizex = static_cast<float>(engine.GetTexture("resource/image/expGage.tga")->width);
	startPos = gameObject->transform.getLocalPosition().x;
}

void Gage::Update()
{
	glm::vec3 additionalPos = gameObject->transform.getLocalPosition();

	stopPos = (baseData * sizex);
	if (stopPos + startPos != additionalPos.x)
	{

		additionalPos.x +=
			(baseData * sizex) * Time::deltaTime;

		if (stopPos + startPos + 2 > additionalPos.x
			&& stopPos + startPos - 2 < additionalPos.x)
			gameObject->transform.setLocalPosition(additionalPos);
	}
	if ((0.99 * sizex) + startPos < additionalPos.x)
	{
		additionalPos.x = startPos;
		gameObject->transform.setLocalPosition(additionalPos);
	}
}