#pragma once
#ifndef FADECONTROLLER_H_INCLUDE
#define FADECONTROLLER_H_INCLUDE
#include "ComponentInclude.h"

class FadeController
	:public Component
{
public:
	bool trriger = false;
	/*
	true = フェードイン
	false = フェードアウト
	*/
	bool fadeWhich = true;
	float fadeSpeed = 0.5f;

	GameObjectPtr fadeObject;

	FadeController(GameObjectPtr gameObject)
		: Component(gameObject) {}
	virtual ~FadeController() = default;

	void CustomEvent(float deltaTime)
	{
		if (trriger)
		{
			if (fadeWhich)
			{
				fadeObject->spriteList.begin()->spriteList.begin()->alpha += fadeSpeed * deltaTime;
			}
			else
			{
				fadeObject->spriteList.begin()->spriteList.begin()->alpha -= fadeSpeed * deltaTime;
			}
			if (fadeObject->spriteList.begin()->spriteList.begin()->alpha > 1 && fadeWhich == true)
				Swhich();
			if (fadeObject->spriteList.begin()->spriteList.begin()->alpha < 0 && fadeWhich == false)
			{
				trriger = false;
				fadeObject->spriteList.begin()->spriteList.begin()->alpha = 0;
				Swhich();
			}
		}
	}
	
	void CustomEvent2(float deltaTime)
	{
		if (trriger)
		{
			if (fadeWhich)
			{
				fadeObject->spriteList.begin()->spriteList.begin()->alpha += fadeSpeed * deltaTime;
			}
			else
			{
				fadeObject->spriteList.begin()->spriteList.begin()->alpha -= fadeSpeed * deltaTime;
			}
			if (fadeObject->spriteList.begin()->spriteList.begin()->alpha < 0 && fadeWhich == false)
				Swhich();
			if (fadeObject->spriteList.begin()->spriteList.begin()->alpha > 1 && fadeWhich == true)
			{
				trriger = false; 
				fadeObject->spriteList.begin()->spriteList.begin()->alpha = 1;
				Swhich();
			}
		}
	}

	void FadeIn(float deltaTime)
	{
		fadeObject->spriteList.begin()->spriteList.begin()->alpha += fadeSpeed * deltaTime;
		if (fadeObject->spriteList.begin()->spriteList.begin()->alpha > 1)
		{
			fadeWhich = false;
			fadeObject->spriteList.begin()->spriteList.begin()->alpha = 1;
		}
	}
	void FadeOut(float deltaTime)
	{

		fadeObject->spriteList.begin()->spriteList.begin()->alpha -= fadeSpeed * deltaTime;
		if (fadeObject->spriteList.begin()->spriteList.begin()->alpha < 0)
		{
			fadeWhich = true;
			fadeObject->spriteList.begin()->spriteList.begin()->alpha = 0;
		}
	}

	void Swhich()noexcept
	{
		fadeWhich = fadeWhich == true ? false : true;
	}

};

#endif // !FADECONTROLLER_H_INCLUDE