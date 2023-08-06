#include "SpriteScaleRotation.h"
#include "../GameObject/GameObject.h"


void ScaleRotation::Update()
{
	/*‰½‚ç‚©‚ÌŒvŽZ*/
	if (target)
	{
		glm::vec3 vector =
			glm::normalize(gameObject->transform.getLocalPosition() + 
				(-target->transform.getLocalPosition()));

		angle = atan2(vector.x, vector.y);
		gameObject->transform.setLocalRotation(vector);
	}

	gameObject->spriteList.begin()->spriteList.begin()->angle = angle;
	gameObject->spriteList.begin()->spriteList.begin()->scale = scale;
}