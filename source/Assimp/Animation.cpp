#include "Animation.h"
#include "Debug/Debug.h"

Animation::Animation(const std::string& animationPath, Model* model,bool loop)
	:isLoop(loop)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath,
		aiProcess_Triangulate
	);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		LOG_ERROR("ERROR::ASSIMP:: %s", importer.GetErrorString());
		return;
	}
	assert(scene && scene->mRootNode);

	LOG("アニメーション %s を読み込みました", animationPath.c_str());
	auto animation = scene->mAnimations[0];
	duration = static_cast<float>(animation->mDuration);
	ticksPerSecond = static_cast<int>(animation->mTicksPerSecond);
	
	ReadMissingBones(animation, *model);
	ReadHierarchyData(rootNode, scene->mRootNode);
	for (auto& itr:bones){
		itr.InitChashData(duration, 0, static_cast<float>(ticksPerSecond));
	}
}

Animation::Animation(const std::string& animationPath, std::shared_ptr<Model>& model,bool loop)
	:isLoop(loop)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath,
		aiProcess_Triangulate
	);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		LOG_ERROR("ERROR::ASSIMP:: %s", importer.GetErrorString());
		return;
	}
	assert(scene && scene->mRootNode);

	//LOG("アニメーション %s を読み込みました", animationPath.c_str());
	auto& animation = scene->mAnimations[0];
	duration = static_cast<float>(animation->mDuration);
	ticksPerSecond = static_cast<int>(animation->mTicksPerSecond);
	
	ReadMissingBones(animation, *model);
	ReadHierarchyData(rootNode, scene->mRootNode);
	for (auto& itr : bones) {
		itr.InitChashData(duration, 0, static_cast<float>(ticksPerSecond));
	}
}


Bone* Animation::FindBone(int id)
{
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&](Bone& Bone){
			return Bone.GetBoneID() == id;
		}
	);
	if (iter == bones.end()) return nullptr;
	else return &(*iter);
}


void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
	int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;
		
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		bones.push_back(Bone(channel->mNodeName.data,
			boneInfoMap[channel->mNodeName.data].id, channel));
	}
	
	this->boneInfoMap = boneInfoMap;
}


void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelper::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.id = boneInfoMap[src->mName.data].id;
	dest.childrenCount = src->mNumChildren;

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}