#pragma once

/*音声プレイヤー番号*/
namespace AudioPlayer {
	inline constexpr int bgm = 0;/*再生に使うプレイヤー番号*/

	namespace BGM {
		inline constexpr char title[] =			"resource/audio/title.wav";
		inline constexpr char stage1[] =		"resource/audio/stage1.wav";
		inline constexpr char stage1Boss[] =	"resource/audio/stage1Boss.wav";
		inline constexpr char clear[] =			"resource/audio/clear.wav";
		inline constexpr char gameOver[] =		"resource/audio/gammeover.wav";
	}

	namespace SE {
		inline constexpr char playerShot[]			= "resource/audio/????";
		inline constexpr char playerExplosion[]		= "resource/audio/Explosion1.wav";
		inline constexpr char enemyExplosionS[]		= "resource/audio/Explosion2.wav";
		inline constexpr char enemyExplosionM[]		= "resource/audio/Explosion2.wav";
		inline constexpr char enemyExplosionL[]		= "resource/audio/Explosion2.wav";
		inline constexpr char gageUp[]				= "resource/audio/gage.wav";
		inline constexpr char cure[]				= "resource/audio/cure.wav";
		inline constexpr char select[]				= "resource/audio/select.wav";
		inline constexpr char enter[]				= "resource/audio/enter.wav";
		inline constexpr char laser[]				= "resource/audio/laser.wav";
	}
}

