#pragma once
#include <vector>
#include <list>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <assimp/scene.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "GlmHelper.h"
#include "AnimResultLocation.h"

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

namespace {
	const float fps120 = 0.0083f;
	const float fps90 = 0.0111f;
	const float fps60 = 0.0166f;
}

class Bone
{
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		:
		name(name),
		id(ID),
		localTransform(1.0f)
	{
		numPositions = channel->mNumPositionKeys;

		for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
		{
			KeyPosition data;
			data.position = AssimpGLMHelper::GetGLMVec(channel->mPositionKeys[positionIndex].mValue);
			data.timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
			positions.push_back(data);
		}

		numRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
		{
			KeyRotation data;
			data.orientation = AssimpGLMHelper::GetGLMQuat(channel->mRotationKeys[rotationIndex].mValue);
			data.timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
			rotations.push_back(data);
		}

		numScalings = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < numScalings; ++keyIndex)
		{
			KeyScale data;
			data.scale = AssimpGLMHelper::GetGLMVec(channel->mScalingKeys[keyIndex].mValue);
			data.timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
			scales.push_back(data);
		}
	}

	glm::mat4 GetLocalTransform() { return localTransform; }
	std::string GetBoneName() const { return name; }
	int GetBoneID() { return id; }

	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < numPositions - 1; ++index)
		{
			if (animationTime < positions[index + 1].timeStamp)
				return index;
		}
		assert(0);
		return 0;
	}

	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < numRotations - 1; ++index)
		{
			if (animationTime < rotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
		return 0;
	}

	int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < numScalings - 1; ++index)
		{
			if (animationTime < scales[index + 1].timeStamp)
				return index;
		}
		assert(0);
		return 0;
	}

	void InitChashData(float duration, int fpsType,float ticksecond) {

		float fps = 0.f;
		if (fpsType == 0)
			fps = fps120;
		else if (fpsType == 1)
			fps = fps90;
		else
			fps = fps60;


		int count = static_cast<int>(duration / fps);
		float time = 0.f;
		chashData.SetIncreas(fps);
		chashData.SetDuration(duration);
		chashData.SetTickTime(ticksecond);
		chashData.ReserveList(count);
		for (int i = 0; i < count; i++) {

			glm::mat4 translation = InterpolatePosition(time);
			glm::mat4 rotation = InterpolateRotation(time);
			glm::mat4 scale = InterpolateScaling(time);
			chashData.PushLocation(glm::mat4(translation * rotation * scale));
			time += fps;
		}
	}

	glm::mat4& GetLocation(float count) {
		return chashData.GetLocation(count);
	}

	glm::vec3 GetPosision(float count) {				
		return  positions[GetPositionIndex(count)].position - positions.begin()->position;
	}

	inline int ChashSize() {
		return chashData.GetSize();
	}

	inline glm::vec3 GetDistanceTraveled() {
		return positions.back().position - positions.begin()->position;
	}

private:

	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	glm::mat4 InterpolatePosition(float animationTime)
	{
		if (1 == numPositions)
			return glm::translate(glm::mat4(1.0f), positions[0].position);

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp,
			positions[p1Index].timeStamp, animationTime);
		glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position
			, scaleFactor);
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 InterpolateRotation(float animationTime)
	{
		if (1 == numRotations)
		{
			auto rotation = glm::normalize(rotations[0].orientation);
			return glm::toMat4(rotation);
		}
		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp,
			rotations[p1Index].timeStamp, animationTime);
		glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation
			, scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 InterpolateScaling(float animationTime)
	{
		if (1 == numScalings)
			return glm::scale(glm::mat4(1.0f), scales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp,
			scales[p1Index].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale
			, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}

	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;
	int numPositions;
	int numRotations;
	int numScalings;
	glm::mat4 localTransform;
	std::string name;
	int id;
	AnimResultLocation chashData = {};
};
