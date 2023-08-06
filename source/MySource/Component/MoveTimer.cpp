#include "MoveTimer.h"
#include "../Engine/Engine.h"


void MoveTimer::Start()
{
	startPos = gameObject->transform.getLocalPosition().x;
	movePoint = startPos - 500;
}

void MoveTimer::CustomEvent()
{
	auto nowPosition = gameObject->transform.getLocalPosition();

	if (trriger) {
		if (!stopFlg) {
			nowPosition.x -= moveSpeed * Time::deltaTime;
			if (nowPosition.x < movePoint)
				stopFlg = true;
		}
		if (stopFlg)interval -= Time::deltaTime;
		if (interval < 0)
		{
			fadeOut = true;
		}
		if (fadeOut)
		{
			nowPosition.x += moveSpeed * Time::deltaTime;
		}
		if (nowPosition.x > startPos && fadeOut)
		{
			trriger = false; stopFlg = false;
			fadeOut = false; interval = 3;
		}
		gameObject->transform.setLocalPosition(nowPosition);
	}
}
