#include "Time.h"
#include "DataStruct/useImGui.h"
#include <GLFW/glfw3.h>

void GameTime::Calc()
{
	//小数点以下の切り捨て
	int nowSecond = static_cast<int>(std::floor(totalTime / 1000.f));
	second = nowSecond % 60;
	minute = (nowSecond / 60) % 60;
	hour = (nowSecond / 3600);
}

std::string GameTime::GetTimeString()
{
	std::string r;
	if (hour < 10)
	{
		r += std::to_string(0);
	}
	r += std::to_string(hour); r += ":";
	if (minute < 10)
	{
		r += std::to_string(0);
	}
	r += std::to_string(minute); r += ":";
	if (second < 10)
	{
		r += std::to_string(0);
	}
	r += std::to_string(second);
	return r;
}

void Time::Initialize()
{
	startTime = static_cast<float>(glfwGetTime()) * 1000.f;
	frameTime = static_cast<float>(glfwGetTime()) * 1000.f;
	frameTimeOld = static_cast<float>(glfwGetTime()) * 1000.f;
}

void Time::Execute()
{
	//1000　＝　1秒
	//かかった時間を持つ
	frameTime = static_cast<float>(glfwGetTime()) * 1000;
	//開始してから今までの時間を格納
	time.totalTime = frameTime - startTime;
	//かかった秒数を現実時間で計算
	time.Calc();

	//今のフレームでかかった時間を入れる
	progressTime = frameTime - frameStartTime;
	//指定のfps回数で行けるように次のフレームでの理想時間を取得
	idealTime = static_cast<int> (frameCount * (1000.0f / idealCount));

	//指定のfps回数で今までかかったfpsを摘出し、
	//次のフレームに移るため現在までの時間を次のフレーム開始に入れる
	frameCount++;
	if (frameCount == idealCount)
	{
		fps = 1000.0f / (progressTime / idealCount);
		frameCount = 0;
		frameStartTime = frameTime;
	}

	//前回と今回のフレームの差をm秒でいれdeltaを格納
	deltaTime = static_cast<float>(frameTime - frameTimeOld) / 1000.0f;
	frameTimeOld = frameTime;
}

void Time::Wait()
{
	if (idealTime > progressTime)
	{
		Sleep(static_cast<DWORD>(idealTime - progressTime));
	}
}

void Time::DebugTextOut()
{
	static float setfps = idealCount;
	float oldFps = setfps;
	ImGui::Begin("Times");
	ImGui::Text(Time::GetTotalTimeString().c_str());
	float fps = Time::GetFPS();
	ImGui::DragFloat("FPS", &fps);
	ImGui::DragFloat("deltaTime", &Time::deltaTime);
	ImGui::SliderFloat("IdealFPS", &setfps, 3, 500);
	ImGui::End();

	if (oldFps != setfps)
	{
		setfps = floor(setfps);
		Time::SetFPS(setfps);
	}
}

void Time::SetFPS(float value)
{
	idealCount = value;
	frameCount = 0;
	frameTime = 1 / idealCount;
}

float Time::GetFPS()
{
	return fps;
}

float Time::GetTotalSecond()
{
	return std::floor(time.totalTime / 1000);
}

int Time::GetSecond()
{
	return time.second;
}

int Time::GetMinute()
{
	return time.minute;
}

int Time::GetHour()
{
	return time.hour;
}

std::string Time::GetTotalTimeString()
{
	return time.GetTimeString();
}

float Time::fps = 0.f;
int Time::frameCount = 0;
float Time::idealCount = 120.f;
float Time::startTime = 0.f;
GameTime Time::time = {};
float Time::frameStartTime = 0.f;
int Time::idealTime = 0;
float Time::frameTime = 0.f;
float Time::frameTimeOld = 0.f;
float Time::progressTime = 0.f;
float Time::deltaTime = 0.f;