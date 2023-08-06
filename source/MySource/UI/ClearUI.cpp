#include "ClearUI.h"
#include "2DImage/Sprite.h"
#include "2DImage/TileMapTexcoord.h"
#include "../Engine/Engine.h"
#include "Component/FadeController.h"

const float GameClear[] =
{
	0.f,		//1822,   G
	214.f,	//1360,   A
	428.f,	//1891,   M
	738.f,	//1916,   E

	930.f,	//154,    C
	1166.f,	//1741,   L
	1268.f,	//240,    E
	1440.f,	//2256,   A	
	1676.f,	//1717,   R
};

const SpriteSize GameClearSize[] =
{
	{214.f,326.f},  //G 
	{214.f,326.f},	//A 
	{310.f,326.f},	//M 
	{214.f,326.f},	//E 

	{236.f,326.f},	//C  
	{102.f,326.f},	//L 
	{172.f,326.f},	//A  
	{236.f,326.f},	//E 	
	{157.f,326.f},	//R 
};

void ClearUI::Initialize(Engine& engine) noexcept
{
	ObjectManager& manager = ObjectManager::Instance();
	const size_t titletextLayer = manager.AddUILayer("resource/image/ClearFont.tga",10, GL_LINEAR);
	float upbase = 128;
	for (int i = 0; i < 9; i++)
	{
		auto uiFont = manager.CreateUI<GameObject>(
			titletextLayer, "title_font",
			glm::vec3(upbase, 500 , 0.f));
		uiFont->AddSpriteTexture(
			MakeTexcoord(GameClear[i], 0, GameClearSize[i].x, GameClearSize[i].y, 1833, 326),
			0, 0, 0, 0.5, 0, 0);

		fades[i] = uiFont->AddComponent<FadeController>();
		fades[i]->fadeWhich = true; fades[i]->fadeSpeed = 1;
		fades[i]->fadeObject = uiFont;
		if (i < 4)upbase += 128;
		else upbase += 96;
	}
}

void ClearUI::Event(float deltaTime) noexcept
{
	interval -= deltaTime;
	if (allTime > 0) {
		interval -= deltaTime;
		if (interval < 0) {
			int a = rand() % 9;

			if (fades[a]->trriger != true)
			{
				fades[a]->trriger = true;
			}
			interval = 0.3f;
		}
		for (auto itr : fades)
		{
			itr->CustomEvent2(deltaTime);
		}
	}
	else
	{
		if (fadein == false) {
			int count = 0;
			for (int i = 0; i < 9; i++)
			{
				fades[i]->trriger = false;
				fades[i]->FadeOut(deltaTime);
				if (fades[i]->fadeObject->spriteList[0].spriteList[0].alpha <= 0.00f)
					count++;
			}
			if (count == 9)
			{
				fadein = true;
			}
		}
		else
		{
			int count = 0;
			for (int i = 0; i < 9; i++)
			{
				fades[i]->trriger = false;
				fades[i]->FadeIn(deltaTime);
				if (fades[i]->fadeObject->spriteList[0].spriteList[0].alpha >= 1.00f)
					count++;
			}
			if (count == 9)
			{
				fadein = false;
				allTime = 10;
			}
		}
	}
}