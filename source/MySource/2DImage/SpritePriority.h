#pragma once

/*表示優先順位*/
enum class SpritePriority
{/*優先順位(高)*/
	prioObject1,
	prioPlayer,			/*自機*/
	prioObject2,
	prioEnemyBullet,	/*敵の弾*/
	prioPlayerBullet,	/*自機の弾*/
	prioExplosion,		/*爆発エフェクト*/
	prioEnemy,			/*敵*/
	prioLaser,
	prioShadow,
};/*優先順位(低)*/
