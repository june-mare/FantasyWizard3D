#pragma once
#ifndef CLEARTIMEMANAGER_H_INCLUDE
#define CLEARTIMEMANAGER_H_INCLUDE
#include <string>
#include <unordered_map>

class ClearTimeManager
{
private:
	ClearTimeManager() = default;
	virtual ~ClearTimeManager()=default;
	std::unordered_map<int, float> clearTimes;
public:
	static ClearTimeManager& Instance()
	{
		static ClearTimeManager instance;
		return instance;
	}
	/*
	@pram num  クリアしたマップ
	@pram time クリアしたタイム
	*/
	void ClearTime(int num, float time);
	/*
	@pram num 指定するマップ
	@return クリアタイム
	*/
	float ClearTime(int num);
	//今登録されているクリアタイムを記録する
	void SaveTimes()noexcept;
	//記録されているクリアタイムを登録する
	void LoadTimes()noexcept;
};

#endif // !CLEARTIMEMANAGER_H_INCLUDE