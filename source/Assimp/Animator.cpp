#include "Animator.h"
#include "Time/Time.h"


Animator::Animator(Animation* animation)
{
	currentTime = 0.0;
	currentAnimation = animation;

	finalBoneMatrices.reserve(82);

	for (int i = 0; i < 82; i++)
		finalBoneMatrices.push_back(glm::mat4(1.0f));
}

Animator::Animator() :
	currentTime(0.f),
	currentAnimation(nullptr)
{}

void Animator::UpdateAnimation() {
	if (currentAnimation) {
		currentTime += currentAnimation->GetTicksPerSecond() * Time::deltaTime;
		if (currentAnimation->CheckLoop())
			currentTime = fmod(currentTime, currentAnimation->GetDuration());
		else {
			if (CheckEndAnim())
				currentTime = currentAnimation->GetDuration();
		}
		glm::mat4 mat = glm::mat4(1.0f);
		CalculateBoneTransform(&currentAnimation->GetRootNode(), mat);
	}
}


void Animator::PlayAnimation(Animation* pAnimation)
{
	if (currentAnimation == nullptr) {
		currentAnimation = pAnimation;
		currentTime = 0.0f;
		lastCheck = false;
		baseBone = nullptr;
		lastBasePos = { 0,0,0 };
	}
	if (pAnimation != currentAnimation) {
		currentAnimation = pAnimation;
		currentTime = 0.0f;
		lastCheck = false;
		baseBone = nullptr;
		lastBasePos = { 0,0,0 };
	}
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4& parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;
	int nodeid = node->id;

	Bone* Bone = currentAnimation->FindBone(nodeid);

	bool root = false;
	if (Bone) {
		if (Bone->GetBoneName() == nodeName) {
			nodeTransform = Bone->GetLocation(currentTime);
			if (!lastCheck) {
				if (nodeName == baseBoneName) {
					baseBone = Bone;
					lastBasePos = Bone->GetDistanceTraveled();
					lastCheck = true;
				}
			}
		}
		else root = true;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	if (!root) {
		auto& boneInfoMap = currentAnimation->GetBoneIDMap();
		auto result = boneInfoMap.find(nodeName);
		if (result != boneInfoMap.end()) {
			finalBoneMatrices[result->second.id] =
				globalTransformation * result->second.offset;
		}
	}
	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4>& Animator::GetFinalBoneMatrices()
{
	return finalBoneMatrices;
}

glm::vec3 Animator::GetTimeBasePos()
{
	if (baseBone != nullptr) {
		if (currentAnimation != nullptr)
			if (CheckEndAnim())
				return baseBone->GetDistanceTraveled();
		return baseBone->GetPosision(currentTime);
	}
	return glm::vec3(0.f);
}
