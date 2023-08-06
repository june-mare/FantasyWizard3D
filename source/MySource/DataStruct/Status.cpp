#include "Status.h"
#include <fstream>
#include <sstream>
#include "System/FileSystem.h"

namespace STATUS
{
	/*�w�肵���t�@�C������X�e�[�^�X��ǂݍ���
	* @param const char* �e�L�X�g�t�@�C���̃p�X
	* @param std::string �擾�������I�u�W�F�N�g�̖��O
	* @return �擾�ł����X�e�[�^�X*/
	Status ReadFileToStatus(const char* path, const char* name) noexcept
	{
		Status status;
		status.SetName(name);
		FileSystem::ReadFile<std::string, float>(path, name, status.GetStatuses());
		return status;
	}

	/*�w�肵���X�e�[�^�X���t�@�C���֕ۑ�����
	* @param const char* �e�L�X�g�t�@�C���̃p�X
	* @param std::string �擾�������I�u�W�F�N�g�̖��O*/
	void SaveStatusToFile(const char* path, Status status)noexcept
	{
		FileSystem::SaveFile<std::string, float>(path, status.GetName().c_str(),
			status.GetStatuses());
	}

}