#pragma once
#ifndef OBJECTSPRITE_H_INCLUDE
#define OBJECTSPRITE_H_INCLUDE
#include "2DImage/Sprite.h"
#include "2DImage/Texture.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct ObjectSprite
{
	SpriteList spriteList;
	TexturePtr tex;
	ObjectSprite(const float x,const float y,const float sx,const float sy,
		Texcoord tc,
		TexturePtr tex)
	{
		spriteList.push_back({
		x, y, sx, sy, tc});

		this->tex = tex;
	}
	ObjectSprite(glm::vec4 value,Texcoord tc,TexturePtr tex)
	{
		spriteList.push_back({
		value.x, value.y, value.z, value.w, tc });

		this->tex = tex;
	}
	ObjectSprite(TexturePtr tex)noexcept :tex(tex){};
	ObjectSprite()noexcept = default;
	Sprite GetSprite(Texcoord tc)
	{
		for (auto& sprite : spriteList)
		{
			if (sprite.texcoord == tc)
			{
				return sprite;
			}
		}
		return Sprite();
	}
	std::string name;
};
using ObjectSpriteList = std::vector<ObjectSprite>;

ObjectSprite FindObjectSprite(std::string name, std::vector<ObjectSprite>& list);

#endif //!OBJECTSPRITE_H_INCLUDE