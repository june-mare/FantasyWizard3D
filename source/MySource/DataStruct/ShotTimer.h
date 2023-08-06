#pragma once
#ifndef SHOTTIMER_H_INCLUDE
#define SHOTTIMER_H_INCLUDE

struct ShotTimer
{
private:
	float shotInterval = 0.7f;
	float rapidInterval = 0.0f;
	float shotTimer = 2.0f;
	float rapidTimer = 2.0f;
	int rapidCount = 0;
	int rapidMaxCount = 0;
public:
	inline void SetShotTime(float timer, float interval)noexcept
	{
		shotTimer = timer; shotInterval = interval;
	}
	inline void SetShotTimer(float timer)noexcept
	{
		shotTimer = timer;
	}
	inline void SetShotInterval(float interval)noexcept
	{
		shotInterval = interval;
	}
	inline void ResetShotTimer()noexcept
	{
		shotTimer = 0.0f;
	}
	inline void ResetRapidTimer()noexcept
	{
		rapidTimer = 0.0f;
	}
	inline void SetRapidTime(float timer, float interval)noexcept
	{
		rapidTimer = timer; rapidInterval = interval;
	}
	inline void SetRapidTimer(float timer)noexcept
	{
		rapidTimer = timer;
	}
	inline void SetRapidInterval(float interval)noexcept
	{
		rapidInterval= interval;
	}
	inline void SetRapidCount(int rCount)noexcept
	{
		rapidCount = rCount; rapidMaxCount = rCount;
	}

	inline void ExecuteShotTimer(float deltaTime)noexcept
	{
		shotTimer += deltaTime;
	}
	inline void ExecuteRapidTimer(float deltaTime)noexcept
	{
		rapidTimer += deltaTime;
	}
	/*�w��b�Ԍo���Ă��邩�ǂ������ׂ�*/
	inline bool ShotElapsedSeconds()noexcept
	{
		bool which = shotTimer >= shotInterval;
		if (which) shotTimer = 0.0f;
		return which;
	}
	/*�w��b�Ԍo���Ă��邩�ǂ������ׂ�*/
	inline bool RapidElapsedSeconds()noexcept
	{
		bool which = rapidTimer >= rapidInterval;
		if (which) rapidTimer = 0.0f;
		return which;
	}
	/*�A�ˉ񐔑O�ŒB���������ׂ�*/
	inline bool ZeroOrLessRpidCount()noexcept
	{
		rapidCount -= 1;
		bool which = rapidCount <= 0;
		if (which)rapidCount = rapidMaxCount;
		return which;
	}
	inline int GetRpidCount()noexcept {
		return rapidCount;
	}
	/*
	*@pram sTimer	�V���b�g�Ԋu�̏�������
	*@pram sInterval �V���b�g�C���^�[�o��
	*@pram rTimer	�A�˂̏�������
	*@pram rInterval �A�ˊԊu
	*@pram rCount	�A�˒e��
	*/
	void Init(float sTimer, float sInterval, float rTimer, float rInterval, int rCount)
	{
		SetShotTime(sTimer, sInterval);
		SetRapidTime(rTimer, rInterval);
		SetRapidCount(rCount);
	}
};

#endif // !SHOTTIMER_H_INCLUDE