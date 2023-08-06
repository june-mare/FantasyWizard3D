#pragma once
#include <vector>
#include <memory>
#include <string>
#include <type_traits>
#include "DataStruct/Status.h"
#include "2DImage/Texture.h"
#include "2DImage/SpritePriority.h"
#include "Component/Component.h"
#include "ObjectTag.h"
#include "ObjectSprite.h"
#include "Transform.h"
#include "State/IState.h"
#include "Component/Renderer.h"
#include "../Assimp/Model.h"
#include "../Assimp/Mesh.h"
#include "../Assimp/Animator.h"
#include "Collision/Collision3D.h"
#include "DataStruct/AnimUseList.h"
#include "../Engine/Engine.h"
#include "Collision/Collider.h"


class GameObject;
using GameObjectPtr = std::shared_ptr<GameObject>;//�Q�[���I�u�W�F�N�g�̃|�C���^
using GameObjectList = std::vector<GameObjectPtr>;//�Q�[���I�u�W�F�N�g�̂ۂ񂽔z��

class MeshRenderer;
using MeshRendererPtr = std::shared_ptr<MeshRenderer>;
class Shader;

class GameObject
	:public std::enable_shared_from_this<GameObject>
{
private:
	//�X�^�[�g��ʂ������ǂ���
	bool isStarted = false;
	//���̃Q�[���I�u�W�F�N�g���폜���邩�ǂ����̃t���O
	bool isDead = false;
	/*�擾���邽�߂܂��̓R���W�����v�Z�̃^�O*/
	std::string tag = "untag";
	//�t���X�^���J�����O�p�̕ϐ�
	std::unique_ptr<BoundingVolume> frustumShape = nullptr;

protected:
	//��ʂɉf���Ă��邩�ǂ�����bool
	bool isActive = false;
	//���̃I�u�W�F�N�g�̖��O
	std::string name;
	//�g�p����X�e�[�^�X�p�����[�^
	std::shared_ptr<STATUS::Status> status = nullptr;
	//�s���̐���
	std::shared_ptr<STATE::IState>  state = nullptr;
public:
	//�Փ˔���R���W����AABB
	enum class Rigid {
		Static,
		Dynamic,
	};
	std::shared_ptr<AABB> aabb = nullptr;
	std::shared_ptr<Sphere> sphere = nullptr;
	std::vector<ColliderPtr> colliderList;
	Rigid rigid = Rigid::Dynamic;

	//�A�j���[�V�����̈ʒu�����Z���邩�ǂ���
	bool animPosFlag = false;
	//�A�j���[�V�������̈ʒu
	glm::vec3 animPosition{};
	//�g�p����A�j���[�V������⊮���郊�X�g
	std::shared_ptr<AnimUseList> animUseList = nullptr;
	/*2D�摜�̕\���D�揇��(����@�D��x)*/
	SpritePriority layer = SpritePriority::prioObject1;
	//2D�摜�̏��
	ObjectSpriteList spriteList = {};
	//�O������
	glm::vec3 front = { 1.0f,0.0f,0.0f };
	//�ʒu�A��]�A�傫��
	Transform transform = {};
	//�R���|�[�l���g���X�g
	ComponentList componentList = {};
	//3D���f���̏��
	std::vector<std::shared_ptr<Model>> models = {};
	//�A�j���[�V�����𓮂������߂̕�
	std::shared_ptr<Animator> animator = nullptr;
	//�g�p����}�e���A����id 
	int materialID = 0;
public:
	GameObject() = default;
	virtual ~GameObject()noexcept = default;
	/*Actor�݂����Ɍp�������ē��ꂽ���R���|�[�l���g��ǉ�����*/
	virtual void ActorStart()noexcept;
	virtual void ActorUpdate()noexcept;
	/*�R���|�[�l���g�����̑���*/
	virtual void Start()noexcept;
	virtual void Update()noexcept;
	virtual void OnCollision(GameObject& other, std::vector<Collider::Tag> tagList)noexcept;
	virtual void OnDestroy()noexcept;
	virtual void TakeDamage(GameObject& other, const Damage& damage)noexcept;
	virtual void OutCollision();

	/*�R���W�����̒ǉ�*/
	template <typename T>
	std::shared_ptr<T> AddCollision() {
		auto collision = std::make_shared<T>();
		colliderList.push_back(collision);
		return collision;
	}

	/*�R���|�[�l���g�̑����A�܂��͎擾*/
	template<typename T>
	std::shared_ptr<T> AddComponent()
	{
		std::shared_ptr<T> p = std::make_shared<T>(shared_from_this());
		componentList.push_back(p);
		return p;
	}
	template <typename T>
	std::shared_ptr<T> GetConponent()noexcept
	{
		auto p = std::make_shared<T>(shared_from_this());

		for (auto itr : componentList) {
			if (itr->name == p->name)
			{
				p.reset();
				return std::dynamic_pointer_cast<T>(itr);
			}
		}

		return nullptr;
	}
	template <typename T>
	void RemoveConponent()noexcept
	{
		auto p = std::make_shared<T>(shared_from_this());
		for (auto itr : componentList) {
			if (itr->name == p->name)
			{
				componentList.erase(
					std::remove(componentList.begin(), componentList.end(), itr),
					componentList.end());
				p.reset();
				itr.reset();

				break;
			}
		}
	}

	//����̈ʒu��������̕��������̃V���[�g�J�b�g
	void AddPosition(glm::vec3 addPosition);

	// �`��̃f�[�^�𑗂�o��
	void PutDrawData(Frustum& frustum, int& display, bool isInstance = false);

	/*�X�e�[�^�X�̍쐬*/
	void CreateStatus(const char* path, const char* name);

	/*�A�j���[�^�[�̍쐬*/
	std::shared_ptr<Animator>
		CreateAnimator(Animation* animation = nullptr);

	/*���f���̍쐬(�����Ńt���X�^���J�����O�p�̃R���W����������Ă�)*/
	std::shared_ptr<Model> CreateModel(std::string name, std::string path);

	/*�A�j���[�^�[�������Ă��邩�ǂ���*/
	bool HasAnimator()const noexcept;

	/*�X�e�[�g�̕ύX�ƐV�K�쐬*/
	template <typename T>
	void SetState()noexcept {
		if (state != nullptr) {
			state->Finalize();
			state.reset();
		}
		state = std::make_shared<T>();
		state->Initialize();
		state->giveGameObject(shared_from_this());
	}

	/*�X�v���C�g�̑���*/
	void AddSpriteTexture(const Texcoord& tc, TexturePtr tex,
		float x = 0, float y = 0, float scale = 1, float rotation = 0, float alpha = 1)noexcept;
	void AddSprite(const Texcoord& tc, TexturePtr tex,
		float x = 0, float y = 0, float scale = 1, float rotation = 0)noexcept;
	void AddSprite(const Texcoord& tc, std::string name, float x, float y, float scale, float rotation) noexcept;
	const std::vector<ObjectSprite>& GetSprite()const { return spriteList; }

	/*�Q�b�^�[�Z�b�^�[*/
	inline std::shared_ptr<STATUS::Status>& GetStatus()
	{
		return status;
	}

	inline void SetName(std::string name)noexcept
	{
		this->name = name;
	}
	inline std::string GetName()const noexcept
	{
		return this->name;
	}

	inline bool IsDead()const noexcept
	{
		return this->isDead;
	}
	inline void IsDead(const bool value)noexcept
	{
		this->isDead = value;
	}

	inline void SetTag(std::string tag)noexcept
	{
		this->tag = TAG::CheckforResistation(tag);
	}
	inline std::string GetTag()const noexcept
	{
		return this->tag;
	}

	inline bool IsStarted()const { return isStarted; }
	inline void IsStarted(bool value) { this->isStarted = value; }

	/*�f�o�b�O�̏���o*/
	void Debug()noexcept;
};