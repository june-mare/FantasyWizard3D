#include "TitleScene.h"
#include "Window/GLFWWindow.h"
#include "Component/text.h"
#include "GameObject/GameObject.h"
#include "Audio/AudioSettings.h"
#include "Component/AudioEvent.h"
#include "Component/FadeController.h"
#include "2DImage/TileMapTexcoord.h"
#include "2DImage/TileMapScroll.h"
#include "../Engine/Engine.h"
#include "GameObject/ObjectManager.h"
#include "Time/Time.h"
#include "System/InputSystem.h"
#include "VillageScene.h"

const float TitleFont[] =
{
	0.f,//1822,    1x2 ��藧��
	144.f,//1360,    4x5 �傫�����̖�
	288.f,//1891,    1x3 �X��
	432.f,//1916,    2x2 ������
	530.f ,//154,     2x2 ����O��
	683.f,//1741,    2x4 �e���g
	817.f ,//240,     3x3 ���_��
	991.f,//2256,    4x5 �傫���܂邢��	
	1192.f,//1717,    1x3 �΂̒�
	1282.f,//1736,    5x5 �ł����e���g
	1432.f,//         1x1 ������
	1592.f, //			1x1 ��
	1698.f, //			1x1 ��	

};

const SpriteSize TitleFontSize[] =
{
	{144.f,283.f},      //F,    1x2 ��藧��
	{144.f,283.f},	    //a,    4x5 �傫�����̖�
	{144.f,283.f},	    //n,    1x3 �X��
	{98.f, 283.f },	    //t,    2x2 ������
	{153.f, 283.f },    //a,     2x2 ����O��
	{134.f, 283.f },    //s,    2x4 �e���g
	{174.f, 283.f },    //y,     3x3 ���_��
	{210.f,283.f},	    //w,    4x5 �傫���܂邢��		
	{90.f,283.f},	    //,    1x3 �΂̒�
	{150.f, 283.f},	    //1736,    5x5 �ł����e���g
	{160.f, 283.f },	//         1x1 ������
	{106.f ,283.f },	//			1x1 ��
	{134.f ,283.f },	//			1x1 ��
	{0.f ,283.f },	//			1x1 ��
};

/*�^�C�g����ʂ�����������*/

bool TitleScene::Initialize() {
	ObjectManager& manager = ObjectManager::Instance();
	/*�Q�[���I�u�W�F�N�g���폜*/
	manager.ResetAllObjectList();

	/*UI���C���[���쐬*/
	const size_t bgLayer		= manager.AddUILayer("resource/textures/image/title.png",	10, GL_LINEAR );
	const size_t textLayer		= manager.AddUILayer("resource/textures/image/font.tga",		10, GL_LINEAR );
	const size_t titletextLayer = manager.AddUILayer("resource/textures/image/TitleFont.tga",10, GL_LINEAR );

	/*�w�i�摜*/
	auto uiBackground = manager.CreateUI<GameObject>(
		bgLayer, "title_bg", glm::vec3(960.f, 640.f ,0.f));
	uiBackground->AddSpriteTexture({ 0,0,1,1 }, 0, 0, 0.0f, 1.0);
	/*�^�C�g�����S*/
	
	/*�Q�[���̊J�n�������e�L�X�g*/
	const char strPressEnter[] = "Press Enter";
	const float fontSizeX = 24;
	const float x =
		1420 - static_cast<float>(std::size(strPressEnter) - 1) * fontSizeX;
	auto uiPressEnter = manager.CreateUI<GameObject>(
		textLayer, "press enter",glm::vec3( x, 100.f, 0.f));
	auto textPressEnter = uiPressEnter->AddComponent<Text>();
	textPressEnter->SetText(strPressEnter, 2);
	uiPressEnter->layer = SpritePriority::prioObject1;
	uiBackground->layer = SpritePriority::prioObject2;

	float upbase = 50;
	float downbase = 300;

	for (int i = 0; i < 13; i++)
	{
		if (i < 7) {
			auto uiFont = manager.CreateUI<GameObject>(
				titletextLayer, "title_font",
				glm::vec3(upbase + TitleFontSize[i].x / 2, 600,0.f));
			uiFont->AddSpriteTexture(
				MakeTexcoord(TitleFont[i], 0, TitleFontSize[i].x, TitleFontSize[i].y, 1832, 283),
				0, 0, 0, 0.6f);
			fades[i] = uiFont->AddComponent<FadeController>();
			fades[i]->fadeWhich = false; fades[i]->fadeSpeed = 0.5;
			upbase += TitleFontSize[i].x / 2 + 15; fades[i]->fadeObject = uiFont;
		}
		else
		{
			if (i == 7) {
				auto uiFont = manager.CreateUI<GameObject>(
					titletextLayer, "title_font",glm::vec3( downbase + TitleFontSize[i].x / 2 - 20, 400,0.f));
				uiFont->AddSpriteTexture(
					MakeTexcoord(TitleFont[i], 0, TitleFontSize[i].x, TitleFontSize[i].y, 1832, 283),
					0, 0, 0, 0.6f);
				fades[i] = uiFont->AddComponent<FadeController>();
				fades[i]->fadeSpeed = 0.5;
				fades[i]->fadeWhich = false; fades[i]->fadeObject = uiFont;
				downbase += TitleFontSize[i].x / 2 + 15;
			}
			else
			{
				auto uiFont = manager.CreateUI<GameObject>(
					titletextLayer, "title_font",glm::vec3(downbase + TitleFontSize[i].x * 0.5f, 400.f,0.f));
				uiFont->AddSpriteTexture(
					MakeTexcoord(TitleFont[i], 0.f, TitleFontSize[i].x, TitleFontSize[i].y, 1832.f, 283.f),
					0, 0, 0, 0.6f);
				fades[i] = uiFont->AddComponent<FadeController>();
				fades[i]->fadeSpeed = 0.5f;
				fades[i]->fadeWhich = false; fades[i]->fadeObject = uiFont;
				downbase += TitleFontSize[i].x / 2 + 15;
			}
		}
	}

	Audio::Play(AudioPlayer::bgm, AudioPlayer::BGM::title, 1.5, true);

	return true;
}

void TitleScene::Execute()
{
	Engine& engine = Engine::Instance();
	if (allInterval > 0) {
		interval -= Time::deltaTime;
		if (interval < 0) {
			int a = rand() % 13;

			if (fades[a]->trriger != true)
			{
				fades[a]->trriger = true;
			}
			interval = 0.3f;
		}
		for (auto itr : fades)
		{
			itr->CustomEvent2(Time::deltaTime);
		}
	}
	else
	{
		if (fadein == false) {
			int count = 0;
			for (int i = 0; i < 13; i++)
			{
				fades[i]->trriger = false;
				fades[i]->FadeOut(Time::deltaTime);
				if (fades[i]->fadeObject->spriteList[0].spriteList[0].alpha <= 0.00f)
					count++;
			}
			if (count == 13)
				fadein = true;
		}
		else
		{
			int count = 0;
			for (int i = 0; i < 13; i++)
			{
				fades[i]->trriger = false;
				fades[i]->FadeIn(Time::deltaTime);
				if (fades[i]->fadeObject->spriteList[0].spriteList[0].alpha >= 1.00f)
					count++;
			}
			if (count == 13)
			{
				fadein = false;
				allInterval = 10;
			}
		}
	}
	allInterval -= Time::deltaTime;
	if (InputSystem::GetKey(GLFW_KEY_ENTER)) {
		Audio::PlayOneShot(AudioPlayer::SE::enter);
		ObjectManager& manager = ObjectManager::Instance();		
		engine.SetNextScene<VillageScene>();
	}
	
}