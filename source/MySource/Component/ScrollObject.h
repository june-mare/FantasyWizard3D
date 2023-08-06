#pragma once
#ifndef SCROLLOBJECT_H_INCLUDE
#define SCROLLOBJECT_H_INCLUDE
#include "Component.h"
#include "../2DImage/TileMapScroll.h"

class ScrollObject :
	public Component
{
public:
	TileMapScrollPtr scrolldata;
	ScrollObject(GameObjectPtr gameObject)
		:Component(gameObject) {
		name = "ScrollObject";
	}
	virtual void Update()override
	{
		glm::vec3 additional = {0.f,0.f,0.f};
		switch (scrolldata->type)
		{
		case ScrollType::upScroll:
			if (scrolldata->baseY > scrolldata->endPosisionY)
				additional.y -= scrolldata->speed * Time::deltaTime;
			break;
		case ScrollType::downScroll:
			if (scrolldata->baseY < scrolldata->endPosisionY)
				additional.y += scrolldata->speed * Time::deltaTime;
			break;
		case ScrollType::rightScroll:
			if (scrolldata->baseX < scrolldata->endPosisionX)
				additional.x += scrolldata->speed * Time::deltaTime;
			break;
		case ScrollType::leftScroll:
			if (scrolldata->baseX > scrolldata->endPosisionX)
				additional.x -= scrolldata->speed * Time::deltaTime;
			break;
		}
		gameObject->AddPosition(additional);
	}
};

#endif // !SCROLLOBJECT_H_INCLUDE