#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <glad/glad.h>

/*�Q�[���J�n���獡�܂ł̎��Ԃ��i�[*/
struct GameTime
{
	float totalTime = 0;
	int second = 0, minute = 0, hour = 0;
	void Calc();
	std::string GetTimeString();	
};

/*FPS�̊Ǘ�*/
class Time
{
public:
	//fps�ɂ�鏈���̕ω���}���邽�߂̕�
	static float deltaTime;
public:	
	//�ŏ��ɂ����s������
	static void Initialize();
	//�X�V����
	static void Execute();
	//fps���w��̕��ɂ��邽�ߎ~�܂点�鏈��
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
	* @return �v���O�����J�n�����獡�܂ł����������Ԃ��������Z�ŕԂ�
	*/
	static std::string GetTotalTimeString();
#pragma endregion
private:
	//���݂�fps���i�[����
	static float fps;
	//�t���[���񐔂̃J�E���g
	static int frameCount;
	//�w��fps��
	static float idealCount;
	//�v���O�����J�n����
	static float startTime;
	//�������Ԋ��Z���i�[
	static GameTime time;
	//�t���[���̊J�n����
	static float frameStartTime;
	//�t���[������
	static float frameTime;
	//���z�̃t���[������
	static int idealTime;
	//�O��̃t���[������
	static float frameTimeOld;
	//�O��ƍ���̃t���[���̍�
	static float progressTime;	
};