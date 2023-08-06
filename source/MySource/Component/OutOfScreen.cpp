#include "OutOfScreen.h"
#include "../GameObject/GameObject.h"

void OutOfScreen::Update()
{
	const float xMin = -32;
	const float xMax = 1248;
	const float yMin = -32;
	const float yMax = 704;

	auto position = gameObject->transform.getLocalPosition();

	if (position.x <= xMin || position.x >= xMax ||
		position.y <= yMin || position.y >= yMax)
	{
		gameObject->IsDead(true);
	}
}