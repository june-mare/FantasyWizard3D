#pragma once
/*
* @brief �Q�[���I�u�W�F�N�g�Ɏg�p����X�e�[�^�X�̊Ǘ���ǂݏ���
* @auther Junya Sasou
* @date  2023/03/22
*/
#include <unordered_map>
#include <stdarg.h>
#include <string>

namespace STATUS
{
	/*��s�錾*/
	struct Status;

	/*�w�肵���t�@�C������X�e�[�^�X��ǂݍ���
	* @param const char* �e�L�X�g�t�@�C���̃p�X
	* @param std::string �擾�������I�u�W�F�N�g�̖��O
	* @return �擾�ł����X�e�[�^�X*/
	Status ReadFileToStatus(const char* path, const char* name)noexcept;

	/*�w�肵���X�e�[�^�X���t�@�C���֕ۑ�����
	* @param const char* �e�L�X�g�t�@�C���̃p�X
	* @param std::string �擾�������I�u�W�F�N�g�̖��O*/
	void SaveStatusToFile(const char* path, Status status)noexcept;

	/*
	* @brief �Q�[���Ɏg����Ԉُ��enum
	*/
	enum class Condision
	{
		normal,
		poizon,
		freeze,
	};

	/**
	* @brief �Q�[���I�u�W�F�N�g�Ŏg���p�����[�^�ۑ��p�\����
	*/
	struct Status
	{
	private:
		std::string name;
		std::unordered_map<std::string, float>
			package;
		Condision condision = Condision::normal;
	public:
		/*�w�肵���X�e�[�^�X�̒l�̃Q�b�^�[*/
		inline float GetStatusValue(const char* statusname)const noexcept
		{
			if (package.find(statusname) != package.end())
				return package.at(statusname);
			return 0.0f;
		}
		/*�X�e�[�^�X�̃}�b�v�̃Q�b�^�[*/
		inline std::unordered_map<std::string, float>&
			GetStatuses()noexcept
		{
			return package;
		}
		/*��Ԉُ�̃Q�b�^�[*/
		inline Condision GetCondision()const noexcept
		{
			return condision;
		}
		/*���O�̃Q�b�^�[*/
		inline std::string GetName()const noexcept
		{
			return name;
		}
		/*���O�̃Z�b�^�[*/
		inline void SetName(std::string name)noexcept
		{
			this->name = name;
		}
		/*�����̃p�����[�^�̂ݒǉ�*/
		inline void SetStatusValue(const char* statusname, float value)noexcept
		{
			if (package.find(statusname) != package.end())
				package[statusname] = value;
		}
		/*������V�K�̃p�����[�^�ǉ�*/
		inline void PushStatusValue(const char* statusname, float value)noexcept
		{
			package[statusname] = value;
		}
		/*��Ԉُ�̃Z�b�^�[*/
		inline void SetCondision(Condision _condision)noexcept
		{
			condision = _condision;
		}
		/*�ꊇ�����ƐV�K�̂��p�����[�^�ǉ�
		* @param int �������������������̐�
		* @param ... �ϒ������@std::string, float ...�̏��ŋL�ڂ���*/
		void SetStatusValues(int num, ...)noexcept
		{
			va_list args;
			va_start(args, num);
			std::string name = {};float param = 0.0f;
			for (int i = 0; i < num; i++)
			{
				if (i % 2 == 0)
					name = va_arg(args, char*);
				else
				{
					param = static_cast<float>(va_arg(args, double));
					PushStatusValue(name.c_str(), param);
				}
			}
			va_end(args);
		}

		Status operator=(const Status& _status)
		{
			name = _status.name;
			package.clear();
			package.insert(_status.package.begin(), _status.package.end());
			condision = _status.condision;
			return _status;
		}

		float operator[](const char* name) {
			return GetStatusValue(name);
		}
		

	};

	/*
	* @brief �X�e�[�^�X��ǂݍ���ł����̃f�[�^�����X�g�Ƃ��ď�������
	*/
	class StatusManager
	{
	private:
		StatusManager() {};
		~StatusManager() {
			SaveAllStatusToFile("resource/txt/Input.txt");
		}
		std::vector<Status> statuses = {};
	public:
		static StatusManager& GetInstance()
		{
			static StatusManager instance;
			return instance;
		}
		
		/*
		* @brief ���̃f�[�^����擾����i�����ꍇ�ǂݍ��ށj
		* @param const char* �t�@�C���܂ł̃p�X
		* @param string �擾�������I�u�W�F�N�g�̖��O
		*/
		Status GetStatus(const char* path, const char* name)
		{
			for (auto itr : statuses)
			{
				if (itr.GetName().c_str() == name)
				{
					Status status;
					status = itr;
					return status;
				}
			}
			Status status = ReadFileToStatus(path, name);
			statuses.push_back(status);
			return status;
		}
		
		/*
		* 
		*/
		void SetStatus(Status status)
		{
			for (auto itr : statuses) {
				if (itr.GetName() == status.GetName())
					return;
			}
			statuses.push_back(status);			
		}

		/*
		* @brief �����Ă��邷�ׂĂ̌��f�[�^���Z�[�u����
		* @param const char* �t�@�C���܂ł̃p�X
		*/
		void SaveAllStatusToFile(const char* path)noexcept
		{
			for (auto itr : statuses)
			{
				SaveStatusToFile(path, itr);
			}
		}
		
		/*
		* @brief �}�l�[�W���[�ɃX�e�[�^�X��o�^����
		* @param const char* �t�@�C���܂ł̃p�X
		* @param const char* �擾����X�e�[�^�X��
		*/
		Status RegistStatusFromFile(const char* path, const char* name)noexcept{			
			Status status = ReadFileToStatus(path, name);
			statuses.push_back(status);
			return status;
		}

		/*
		* @brief ���f�[�^�ɒǉ��y�ѕύX����ꍇ�ɂ̂ݎg��
		* @param Status �ǉ��܂��͕ύX����X�e�[�^�X�^
		*/
		void SetOriginalStatus(Status status)noexcept
		{
			for (auto itr : statuses)
			{
				if (!std::strcmp(itr.GetName().c_str(), status.GetName().c_str()))
				{
					itr = status;
					return;
				}
			}
			Status copy = status;
			statuses.push_back(copy);
		}
	};

};
