#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Animation.h"
#include "Bone.h"


class Animator
{
public:
	//�A�j���[�V�����̓o�^�ƌo�ߎ��Ԃ̏�����
	Animator(Animation* animation);
	Animator();

	//�^�C���̍X�V
	void UpdateAnimation();

	//�A�j���[�V������ύX�o�ߎ��ԂȂǂ̍X�V
	void PlayAnimation(Animation* pAnimation);

	//�{�[���̈ʒu�̍X�V
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4& parentTransform);

	//�}�g���N�X�z��𑗂�
	std::vector<glm::mat4>& GetFinalBoneMatrices();

	//�o�^����Ă���A�j���[�V������ID���擾
	inline unsigned int GetAnimID()const {
		if (currentAnimation != nullptr)
			return currentAnimation->GetId();
		return 0;
	}
	inline unsigned int CheckLoop()const {
		if (currentAnimation != nullptr)
			return currentAnimation->CheckLoop();
		return true;
	}

	inline bool CheckEndAnim() {
		return currentAnimation->GetDuration() <= currentTime;
	}

	inline void SetBaseBoneName(std::string name) {
		baseBoneName = name;
	}

	inline glm::vec3 GetBaseBonePos()const {
		return lastBasePos;
	}
	
	glm::vec3 GetTimeBasePos();

private:
	//�{�[���̃}�g���N�X�z��
	std::vector<glm::mat4> finalBoneMatrices = {};
	//�o�^����Ă���A�j���[�V����
	Animation* currentAnimation = nullptr;
	//���̃A�j���[�V�������v�Z
	Animation* nextAnimation = nullptr;
	//�x�[�X�{�[������
	Bone* baseBone = nullptr;
	//�ŏI�I�ȃx�[�X�{�[���̍��W�ʒu
	glm::vec3 lastBasePos = {};
	//�x�[�X�{�[���̖��O
	std::string baseBoneName = "";
	//�o�ߎ���
	float currentTime = 0.f;
	//���X�g�f�[�^���擾�������ǂ���
	bool lastCheck = false;

};