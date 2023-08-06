#pragma once
#include <vector>
#include <map>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <functional>
#include "Bone.h"
#include "BoneInfo.h"
#include "Model.h"

//�m�[�h�ɕK�v�ȃf�[�^�Ǝ��̃m�[�h�ɂȂ��ϐ����i�[
struct AssimpNodeData
{
	//�ʒu�A��]�A�傫��
	glm::mat4 transformation = {};
	//���O
	std::string name = "";
	//children�̐�
	int childrenCount = 0;
	//�m�[�h�̔z��
	std::vector<AssimpNodeData> children = {};
	//�m�[�h��id
	int id = 0;
};

class Animation
{
public:
	/*���f���̓ǂݍ��݂Ə�����*/
	Animation(const std::string& animationPath, Model* model,bool loop = true);
	Animation(const std::string& animationPath, std::shared_ptr<Model>& model,bool loop = true);
	Animation() = default;
	
	~Animation() = default;
	
	//�{�[���ɓo�^����Ă���id�Ŏw���bone�������擾����
	Bone* FindBone(int id);

	/*�Q�b�^�[�Z�b�^�[*/
	inline float GetTicksPerSecond() { return static_cast<float>(ticksPerSecond); }
	inline float GetDuration() { return duration; }
	inline const AssimpNodeData& GetRootNode() { return rootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap(){ return boneInfoMap; }
	inline void SetName(std::string name) { this->name = name; }
	inline const std::string GetName() { return this->name; }
	inline void SetId(unsigned int num) { id = num; }
	inline unsigned int GetId()const { return id; }
	inline void SetLoop(bool value) { isLoop = value; }
	inline bool const CheckLoop() { return isLoop; }	
private:
	/*���f���̃{�[�����ƃA�j���[�V�����̃{�[��������v������*/
	void ReadMissingBones(const aiAnimation* animation, Model& model);
	/*�m�[�h�̐���A�\������v������*/
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	//���[�v����A�j���[�V�������ǂ���
	bool isLoop = true;
	/*�A�j���[�V��������肷�邽�߂̕ϐ�*/
	unsigned int id = 0;
	std::string name = "";
	/*�A�j���[�V�����̎��Ԃ̒���*/
	float duration = 0.f;
	//�L�[�|�W�V�����ɓ��B����܂ł̎���
	int ticksPerSecond = 0;
	//�A�j���[�V�����ɓo�^����Ă���Bone���
	std::vector<Bone> bones = {};
	//���g�݂̏��ԂȂ�
	AssimpNodeData rootNode = {};
	//id�ƃI�t�Z�b�g�𖼑O�Ŋi�[
	std::map<std::string, BoneInfo> boneInfoMap = {};
public:
	/*id�ł̔�r*/
	bool operator==(const Animation& other) {
		return this->GetId() == other.GetId();
	}
	bool operator!=(const Animation& other) {
		return this->GetId() != other.GetId();
	}
	void operator=(const Animation& other) {
		 this->id = other.id;
		 this->bones = other.bones;
		 this->boneInfoMap = other.boneInfoMap;
		 this->duration = other.duration;
		 this->name = other.name;
		 this->rootNode = other.rootNode;
	}
};