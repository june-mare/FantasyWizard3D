#pragma once
#include "Time.h"
#include "DataStruct/useImGui.h"

class Slowry
{
	int slowPace = 3;
	int count = 0;
	float slowTime = 0.f;
public:
	void Debug() {
		int pace = 0;
		float time = 0;
		ImGui::Begin("SlowManager");
		ImGui::InputInt("pace", &pace);
		if (pace)SetSlowPace(pace);
		ImGui::InputFloat("time", &time);
		if (time)SetSlowTime(time);
		ImGui::End();
	}
	inline void SetSlowTime(float time) {
		slowTime = time;
	}
	inline void SetSlowPace(int pace) {
		slowPace = pace;
	}

	Slowry() = default;
	~Slowry() = default;

	bool ActiveCheck()noexcept {
		if (slowTime <= 0)return true;
		slowTime -= Time::deltaTime;

		count++;
		if (count >= slowPace) {
			count = 0;
			return true;
		}

		return false;
	}
};

class Stop
{
	bool isStop = false;
	float stopTime = 0.f;
public:
	void Debug() {
		bool button = 0;
		float time = 0;
		ImGui::Begin("StopManager");
		if (ImGui::Button("Stop", ImVec2(200.f, 100.f)))
			StopSwitch();
		ImGui::InputFloat("time", &time);
		if (time)SetStopTime(time);
		ImGui::End();
	}
	bool ActiveCheck()noexcept {
		if (stopTime <= 0.f && !isStop)return true;
		stopTime -= Time::deltaTime;
		return false;
	}
	inline void StopSwitch()noexcept {
		isStop = isStop ? false : true;
	}
	inline void SetStopTime(float time) {
		stopTime = time;
	}
};