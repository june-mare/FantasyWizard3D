#pragma once
#include "Component/Component.h"
#include "GameObject/GameObject.h"
#include "2DImage/Sprite.h"
#include <vector>
#include <algorithm>



/*2Dアニメーションコンポーネント*/
class Animator2D :public Component
{
public:
	bool loopTime = true;	//ループ再生の有無
	bool killWhenEnd = false;//アニメ終了時にゲームオブジェクトを削除
	int startCount = 0;
	int endCount = 0;
	float timer = 0;//経過時間
	float interval = 0.1f;//フレームを進める間隔
	std::vector<SpriteList> clip;//アニメーションクリップ
public:
	Animator2D(GameObjectPtr gameObject)
		:Component(gameObject) { name = "Animator2D"; };
	virtual ~Animator2D() = default;


	virtual void Update()override
	{
		if (clip.empty()) {
			return;
		}

		//経過時間を更新
		timer += Time::deltaTime;
		if (endCount != 0)
		{
			const int size = static_cast<int>(endCount - startCount);
			int i = static_cast<int>(timer / interval);
			if (loopTime)
			{
				i %= size;//ループ再生
				/*end()のところまで図るのでアニメーション枚数+1のサイズを用意できる*/
			}
			else
			{
				/*アニメ終了時killWhenEndフラグがtrueならゲームオブジェクトを削除*/
				if (i >= size && killWhenEnd)
				{
					gameObject->IsDead(true);
				}
				i = std::clamp(i, 0, size - 1);
			}
			i += startCount;
			/*スプライトデータを再生位置のデータに更新*/
			gameObject->spriteList.begin()->spriteList = clip[i];
		}
		else 
		{
			//再生位置を計算
			const int size = static_cast<int>(clip.size());
			int i = static_cast<int>(timer / interval);
			if (loopTime)
			{
				i %= size;//ループ再生
				/*end()のところまで図るのでアニメーション枚数+1のサイズを用意できる*/
			}
			else
			{
				/*アニメ終了時killWhenEndフラグがtrueならゲームオブジェクトを削除*/
				if (i >= size && killWhenEnd)
				{
					gameObject->IsDead(true);
				}
				i = std::clamp(i, 0, size - 1);
			}
			/*スプライトデータを再生位置のデータに更新*/
			gameObject->spriteList.begin()->spriteList = clip[i];
		}

		if (timer > 888)timer = 0;
	}

	void AddSpriteTexture(size_t flameNo, const Texcoord& tc,
		float scale = 1.f, float x = 0.f, float y = 0.f, float rotation = 0.f) {
		if (flameNo >= clip.size()) {
			clip.resize(flameNo);
		}
		clip[flameNo].push_back({ x,y,0.f,0.f,tc });
		clip[flameNo].back().scale = scale;
		clip[flameNo].back().angle = rotation;
	}
	
	void AddSpriteTexture(size_t flameNo, const Texcoord& tc,void* a,
		 float x = 0.f, float y = 0.f,float scalex = 1.f,float scaley = 1.f, float rotation = 0.f) {
		if (flameNo >= clip.size()) {
			clip.resize(flameNo);
		}
		clip[flameNo].push_back({ x,y,0.f,0.f,tc });
		clip[flameNo].back().scaleX = scalex;
		clip[flameNo].back().scaleY = scaley;
		clip[flameNo].back().angle = rotation;
	}

	void Debug()override
	{
		
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ImGui::BulletText("loopTime", &loopTime);
			ImGui::BulletText("killWhenEnd", &killWhenEnd);

			ImGui::DragFloat("timer", &timer);
			ImGui::DragFloat("interval", &interval);
			//int a = 0; char* c;
		}
		
	}
};
