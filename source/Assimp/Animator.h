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
	//アニメーションの登録と経過時間の初期化
	Animator(Animation* animation);
	Animator();

	//タイムの更新
	void UpdateAnimation();

	//アニメーションを変更経過時間などの更新
	void PlayAnimation(Animation* pAnimation);

	//ボーンの位置の更新
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4& parentTransform);

	//マトリクス配列を送る
	std::vector<glm::mat4>& GetFinalBoneMatrices();

	//登録されているアニメーションのIDを取得
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
	//ボーンのマトリクス配列
	std::vector<glm::mat4> finalBoneMatrices = {};
	//登録されているアニメーション
	Animation* currentAnimation = nullptr;
	//次のアニメーションを計算
	Animation* nextAnimation = nullptr;
	//ベースボーン自体
	Bone* baseBone = nullptr;
	//最終的なベースボーンの座標位置
	glm::vec3 lastBasePos = {};
	//ベースボーンの名前
	std::string baseBoneName = "";
	//経過時間
	float currentTime = 0.f;
	//ラストデータを取得したかどうか
	bool lastCheck = false;

};