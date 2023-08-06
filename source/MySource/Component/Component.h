#pragma once
#include <string>
#include <vector>
#include <memory>
#include "DataStruct/useImGui.h"
#include "DataStruct/MovePram.h"
#include "Collision/Collider.h"

class GameObject;
class Component;
using ComponentPtr = std::shared_ptr<Component>;/*コンポーネントのポインタ*/
using ComponentList = std::vector<ComponentPtr>;/*コンポーネントポインタの配列*/

/*ダメージ情報*/
struct Damage
{
	float amount = 0;		/*ダメージ量*/
	int type = 0;			/*ダメージの種類*/
};

/*コンポーネントの基底クラス*/
class Component 	
{
public:
	Component(std::shared_ptr<GameObject> gameObject)
		:gameObject(gameObject) {};
	virtual ~Component() = default;

	/*さいしょのUpdate直前で呼び出される*/
	virtual void Start() {};
	
	/*枚フレーム呼び出される*/
	virtual void Update() {};

	/*衝突が起きたときに呼び出される*/
	virtual void OnCollision(GameObject& other,
		std::vector<Collider::Tag> tagList) {};

	/*ダメージを受けたときに呼び出される*/
	virtual void OnTakeDamage(GameObject& other,
		const Damage& damage) {};

	virtual void OutCollision() {};
	
	/*ゲームオブジェクトがエンジンから削除されたときに呼び出される*/
	virtual void OnDestroy() {};

	virtual void CustomEvent() {};

	virtual void Debug() {};

	std::string name = ""; /*コンポーネント名*/
	bool isStart = false;
	bool isCollision = false;
	std::shared_ptr<GameObject> gameObject = nullptr;
};