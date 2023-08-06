#include "NotOutOfRange.h"
#include <algorithm>
#include "../GameObject/GameObject.h"
#include "Window/Viewport.h"

void NOutOfBounds::Update()
{
	glm::vec2 viewSize =Viewport::Instance().Viewport::GetViewSize();
	
	auto tmp = gameObject->transform.getLocalPosition();

	tmp.x = std::clamp(static_cast<float>(tmp.x),
		16.0f, viewSize.x - 16.0f);
	tmp.y = std::clamp(static_cast<float>(tmp.y),
		16.0f, viewSize.y - 16.0f);

	gameObject->transform.setLocalPosition(tmp);
}