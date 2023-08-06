#include "GameObject.h"
#include "Shader/Shader.h"
#include "Component/MeshRenderer.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "DataStruct/useImGui.h"
#include "../Engine/Engine.h"

void GameObject::ActorStart() noexcept
{}

void GameObject::ActorUpdate() noexcept
{
}

void GameObject::Start()noexcept
{
	if (isStarted) {
		return;
	}

	for (const auto& itr : componentList)
	{
		itr->Start();
	}

	isStarted = true;
}

void GameObject::Update()noexcept
{
	for (const auto& itr : componentList) {
		itr->Update();
	}
	auto name = this->GetName();
	if (HasAnimator() && isActive) {
		animator->UpdateAnimation();
		animPosition = transform.getRotationMat() *
			(transform.getLocalScale() * animator->GetTimeBasePos());
	}
	std::vector<unsigned int> eraseList{};
	for (unsigned int itr = 0; itr < colliderList.size();itr++) {
		if (colliderList[itr]->GetTag() == Collider::Tag::Player) {
			int breakPoint = 0;
		}
		colliderList[itr]->Execute();
		if (colliderList[itr]->CheckActiveTime()) {
			eraseList.push_back(itr);
		}
		if (colliderList[itr]->OnceCollide()) {
			if (std::find(eraseList.begin(), eraseList.end(), itr) == eraseList.end()) {
				eraseList.push_back(itr);
			}
		}
	}
	for (auto itr : eraseList) {
		colliderList.erase(colliderList.begin() + itr);
	}
}

void GameObject::OnCollision(GameObject& other, std::vector<Collider::Tag> tagList)noexcept
{
	for (const auto& itr : componentList) {
		itr->isCollision = true;
		itr->OnCollision(other, tagList);
	}
}

void GameObject::OnDestroy()noexcept
{
	for (const auto& itr : componentList)
	{
		itr->OnDestroy();
	}
}

void GameObject::TakeDamage(GameObject& other, const Damage& damage)noexcept
{
	for (const auto& itr : componentList)
	{
		itr->OnTakeDamage(other, damage);
	}
}

void GameObject::OutCollision()
{
	for (const auto& itr : componentList) {
		if (itr->isCollision) {
			itr->OutCollision();
			itr->isCollision = false;
		}
	}
}

void GameObject::AddPosition(glm::vec3 addPosition)
{
	transform.setLocalPosition(
		transform.getLocalPosition()
		+ addPosition
	);
}

void GameObject::PutDrawData(Frustum& frustum, int& display, bool isInstance)
{
	if (frustumShape == nullptr || models.empty())return;
	if (transform.IsDirty()) {
		transform.computeModelMatrix();
	}

	if (frustumShape->isOnFrustum(frustum, transform))
	{
		glm::mat4 model = transform.getModelMatrix();

		std::vector<glm::mat4> transforms = {};
		if (HasAnimator())
			transforms = animator->GetFinalBoneMatrices();
		Engine& engine = Engine::Instance();
		for (auto& itr : models) {
			for (auto& mesh : itr->meshes) {
				if (transforms.empty()) {
					engine.SetMeshData(materialID, mesh, model);
				}
				else {
					engine.SetAnimData(materialID, mesh, model, transforms);
				}
			}
		}
		display++; isActive = true;
	}
	else isActive = false;
}

void GameObject::CreateStatus(const char* path, const char* name)
{
	STATUS::StatusManager& statusManager = STATUS::StatusManager::GetInstance();

	if (status == nullptr)
		status = std::make_shared<STATUS::Status>();
	*status = statusManager.GetStatus(path, name);
}

std::shared_ptr<Animator> GameObject::CreateAnimator(Animation* animation)
{
	if (animator != nullptr)
		return animator;

	std::shared_ptr<Animator> anim = nullptr;
	if (animation != nullptr)
		anim = std::make_shared<Animator>(animation);
	else
		anim = std::make_shared<Animator>();

	animator = anim;
	return animator;
}

std::shared_ptr<Model> GameObject::CreateModel(std::string name, std::string path)
{
	Engine& engine = Engine::Instance();
	auto model = engine.LoadModel(name, path);
	frustumShape = std::make_unique<AABB>(generateAABB(*model.get()));
	models.push_back(model);
	return model;
}

bool GameObject::HasAnimator() const noexcept
{
	if (animator != nullptr)
		return true;
	return false;
}

void GameObject::AddSpriteTexture(const Texcoord& tc, TexturePtr tex, float x, float y, float scale, float rotation, float alpha)noexcept
{
	spriteList.push_back(ObjectSprite(x, y, 0, 0, tc, tex));
	spriteList.back().spriteList.begin()->scale = scale;
	spriteList.back().spriteList.begin()->alpha = alpha;
}

void GameObject::AddSprite(const Texcoord& tc, TexturePtr tex, float x, float y, float scale, float rotation) noexcept
{
	for (auto sprite : spriteList) {
		if (sprite.tex == tex) {
			sprite.spriteList.push_back(
				{ x, y, 0, 0, tc }
			);
		}
	}
}

void GameObject::AddSprite(const Texcoord& tc, std::string name, float x, float y, float scale, float rotation) noexcept
{
	for (auto sprite : spriteList) {
		if (sprite.name == name) {
			sprite.spriteList.push_back(
				{ x, y, 0, 0, tc }
			);
		}
	}
}

void GameObject::Debug() noexcept
{
	ImGui::Text(name.c_str());
	ImGui::BulletText(tag.c_str());
	int lay = static_cast<int>(layer);
	ImGui::InputInt("layer", &lay);

	glm::vec3 pos = transform.getLocalPosition();
	ImGui::DragFloat3("position", &pos.x);
	transform.setLocalPosition(pos);

	glm::vec3 rotate = transform.getLocalRotation();
	ImGui::DragFloat3("rotation", &rotate.x);
	transform.setLocalRotation(rotate);

	glm::vec3 scale = transform.getLocalScale();
	ImGui::DragFloat3("scale", &scale.x);
	transform.setLocalScale(scale);

	if (status != nullptr) {
		auto package = status->GetStatuses();
		if (!package.empty()) {
			ImGui::Text("StatusParamater");
			for (auto& itr : package) {
				float param = itr.second;
				ImGui::DragFloat(itr.first.c_str(), &param);
				status->SetStatusValue(itr.first.c_str(), param);
			}
		}
		ImGui::Spacing();
		ImGui::Separator();
	}

	if (!componentList.empty()) {
		if (ImGui::CollapsingHeader("ComponentList"))
			for (auto& itr : componentList) {
				itr->Debug();
				ImGui::Spacing();
				ImGui::Separator();
			}
	}

	if (state != nullptr) {
		ImGui::Text("State");
		state->Debug();
	}

	if (animUseList != nullptr) {
		auto& list = animUseList->GetList();
		for (const auto& itr : list) {
			if (ImGui::Button(itr.first.c_str(), ImVec2(100.f, 50.f))) {
				Engine& engine = Engine::Instance();
				animator->PlayAnimation(engine.GetAnimation(itr.second).get());
			}
		}
	}
	if (aabb != nullptr) {
		ImGui::Text("AABB");
		ImGui::DragFloat3("center", &aabb->center.x);
		ImGui::DragFloat3("extent", &aabb->extent.x);
	}
	if (sphere != nullptr) {
		ImGui::Text("Sphere");
		ImGui::DragFloat3("center", &sphere->center.x);
		ImGui::DragFloat("radius", &sphere->radius);
	}

	ImGui::Text("DeleteThis");
	if (ImGui::Button("Dead", ImVec2(100.f, 50.f)))
		this->isDead = true;
}
