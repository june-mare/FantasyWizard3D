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

//ノードに必要なデータと次のノードにつなぐ変数を格納
struct AssimpNodeData
{
	//位置、回転、大きさ
	glm::mat4 transformation = {};
	//名前
	std::string name = "";
	//childrenの数
	int childrenCount = 0;
	//ノードの配列
	std::vector<AssimpNodeData> children = {};
	//ノードのid
	int id = 0;
};

class Animation
{
public:
	/*モデルの読み込みと初期化*/
	Animation(const std::string& animationPath, Model* model,bool loop = true);
	Animation(const std::string& animationPath, std::shared_ptr<Model>& model,bool loop = true);
	Animation() = default;
	
	~Animation() = default;
	
	//ボーンに登録されているidで指定のboneを見つけ取得する
	Bone* FindBone(int id);

	/*ゲッターセッター*/
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
	/*モデルのボーン情報とアニメーションのボーン情報を一致させる*/
	void ReadMissingBones(const aiAnimation* animation, Model& model);
	/*ノードの数や、構造を一致させる*/
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	//ループするアニメーションかどうか
	bool isLoop = true;
	/*アニメーションを特定するための変数*/
	unsigned int id = 0;
	std::string name = "";
	/*アニメーションの時間の長さ*/
	float duration = 0.f;
	//キーポジションに到達するまでの時間
	int ticksPerSecond = 0;
	//アニメーションに登録されているBone情報
	std::vector<Bone> bones = {};
	//骨組みの順番など
	AssimpNodeData rootNode = {};
	//idとオフセットを名前で格納
	std::map<std::string, BoneInfo> boneInfoMap = {};
public:
	/*idでの比較*/
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