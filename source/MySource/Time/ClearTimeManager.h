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
	@pram num  �N���A�����}�b�v
	@pram time �N���A�����^�C��
	*/
	void ClearTime(int num, float time);
	/*
	@pram num �w�肷��}�b�v
	@return �N���A�^�C��
	*/
	float ClearTime(int num);
	//���o�^����Ă���N���A�^�C�����L�^����
	void SaveTimes()noexcept;
	//�L�^����Ă���N���A�^�C����o�^����
	void LoadTimes()noexcept;
};

#endif // !CLEARTIMEMANAGER_H_INCLUDE