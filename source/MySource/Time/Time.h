#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <glad/glad.h>

/*ゲーム開始から今までの時間を格納*/
struct GameTime
{
	float totalTime = 0;
	int second = 0, minute = 0, hour = 0;
	void Calc();
	std::string GetTimeString();	
};

/*FPSの管理*/
class Time
{
public:
	//fpsによる処理の変化を抑えるための物
	static float deltaTime;
public:	
	//最初にだけ行う処理
	static void Initialize();
	//更新処理
	static void Execute();
	//fpsを指定の物にするため止まらせる処理
	static void Wait();	

	static void DebugTextOut();
#pragma region GetterSetter
	
	static void SetFPS(float value);
	static float GetFPS();	
	static float GetTotalSecond();

	static int GetSecond();
	static int GetMinute();	
	static int GetHour();
	
	/*
	* @return プログラム開始時から今までかかった時間を現実換算で返す
	*/
	static std::string GetTotalTimeString();
#pragma endregion
private:
	//現在のfpsを格納する
	static float fps;
	//フレーム回数のカウント
	static int frameCount;
	//指定fps回数
	static float idealCount;
	//プログラム開始時間
	static float startTime;
	//現実時間換算を格納
	static GameTime time;
	//フレームの開始時間
	static float frameStartTime;
	//フレーム時間
	static float frameTime;
	//理想のフレーム時間
	static int idealTime;
	//前回のフレーム時間
	static float frameTimeOld;
	//前回と今回のフレームの差
	static float progressTime;	
};