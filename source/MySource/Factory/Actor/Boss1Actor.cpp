#include "Boss1Actor.h"

#include "../../2DImage/CharacterList.h"
#include "../../Audio/EasyAudio.h"
#include "../../Audio/AudioSettings.h"
#include "../../State/MapOneBossState.h"
#include "../../DataStruct/DataStructInclude.h"

void Boss1Actor::Initialize() noexcept
{
	//基本処理
	{
		gameObject->layer = prioEnemy;
		gameObject->tag = GameObjectTag::enemy;
	}

	auto tcList = CharacterTCList<Boss1>();
	//画像処理
	{
		gameObject->AddSpriteTexture(tcList[0],
			gameObject->engine->FindWithTexture("resource/image/objects.tga"));
		auto anim = gameObject->AddComponent<Animator2D>();
		anim->clip.resize(tcList.size());
		for (int i = 0; i < tcList.size(); i++)
		{
			anim->AddSpriteTexture(i, tcList[i], 1.5);
		}
		anim->interval = 0.2f;
		anim->loopTime = true;
	}

	//コンポーネント処理
	{
		gameObject->AddComponent<BulletWays>();
		
		float collisionSize = (IMG_OBJECTS_SIZE * 0.4f);
		auto collition = gameObject->AddComponent<Box2DCollision>();
		collition->AddCollision({ -tcList[0].sx * collisionSize, -tcList[0].sy * collisionSize,
			tcList[0].sx * collisionSize, tcList[0].sy * collisionSize
			});
		collition->Start();

		health = gameObject->AddComponent<Health>();
		health->health = 250;

		auto explosion = gameObject->AddComponent<Explosion>();
		explosion->count = 2;

		auto audio = gameObject->AddComponent<AudioEvent>();
		audio->files.onTakeDamage = AudioPlayer::SE::enemyExplosionS;

		auto damage = gameObject->AddComponent<DamageSource>();
		damage->targetName.push_back("player");
		damage->isOnce = false; damage->interval = 1.0f; damage->damageInfo.amount = 3;

		move.speed = 100.0f;
		move.acc = 50.0f;
		move.accIncrease = 0.5f;
		move.moveVector.x = -1;
	}
}

void Boss1Actor::Execute(float deltaTime) noexcept
{
	auto nowPosition = gameObject->transform.getLocalPosition();
	if (nowPosition.x > 1200)
		move.moveVector.x = -1;
	else if (nowPosition.x < 80)
		move.moveVector.x = 1;

	move.Acceleration();
	gameObject->AddPosition(
		move.IncreasePos(deltaTime));

	if (health->health <= 125 && !isHerf)
		SummonObject();
}

void Boss1Actor::SummonObject() noexcept
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			auto data = gameObject->engine->Create<GameObject>("object",
				426.f + (j * 426.f),
				48.f + (i * 96.f))
				->AddComponent<EndSpawn>();
			data->startEffectType = 15; data->spawnType = SpawnType::objectspawn;
			data->size = 2; data->life = 20; data->randomWeapon = true; data->randomRecovery = true;
		}
	}
	isHerf = true;
}