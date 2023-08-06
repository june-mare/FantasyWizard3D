#pragma once
#include <string>
#include <vector>
#include <memory>
#include "DataStruct/useImGui.h"
#include "DataStruct/MovePram.h"
#include "Collision/Collider.h"

class GameObject;
class Component;
using ComponentPtr = std::shared_ptr<Component>;/*�R���|�[�l���g�̃|�C���^*/
using ComponentList = std::vector<ComponentPtr>;/*�R���|�[�l���g�|�C���^�̔z��*/

/*�_���[�W���*/
struct Damage
{
	float amount = 0;		/*�_���[�W��*/
	int type = 0;			/*�_���[�W�̎��*/
};

/*�R���|�[�l���g�̊��N���X*/
class Component 	
{
public:
	Component(std::shared_ptr<GameObject> gameObject)
		:gameObject(gameObject) {};
	virtual ~Component() = default;

	/*���������Update���O�ŌĂяo�����*/
	virtual void Start() {};
	
	/*���t���[���Ăяo�����*/
	virtual void Update() {};

	/*�Փ˂��N�����Ƃ��ɌĂяo�����*/
	virtual void OnCollision(GameObject& other,
		std::vector<Collider::Tag> tagList) {};

	/*�_���[�W���󂯂��Ƃ��ɌĂяo�����*/
	virtual void OnTakeDamage(GameObject& other,
		const Damage& damage) {};

	virtual void OutCollision() {};
	
	/*�Q�[���I�u�W�F�N�g���G���W������폜���ꂽ�Ƃ��ɌĂяo�����*/
	virtual void OnDestroy() {};

	virtual void CustomEvent() {};

	virtual void Debug() {};

	std::string name = ""; /*�R���|�[�l���g��*/
	bool isStart = false;
	bool isCollision = false;
	std::shared_ptr<GameObject> gameObject = nullptr;
};