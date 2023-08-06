#include <glad/glad.h>
#include <memory>
#include <vector>
#include "2DImage/Texture.h"

//先行宣言
class FramebufferObject;
using FramebufferObjectPtr = std::shared_ptr<FramebufferObject>;
using FramebufferObjectList = std::vector<FramebufferObjectPtr>;

enum class FboType
{
	color = 1, // カラーテクスチャだけ
	depth = 2, // 深度テクスチャだけ
	colorDepth = color | depth, // カラーテクスチャと深度テクスチャ
};

/**
* FboType同士の論理積
*/
constexpr int operator&(FboType a, FboType b)
{
	return static_cast<int>(a) & static_cast<int>(b);
}

class FramebufferObject
{
public:
	// FBOを作成
	static FramebufferObjectPtr Create(int width, int height, FboType type);

	// コンストラクタ・デストラクタ
	FramebufferObject(int width, int height, FboType type);
	~FramebufferObject();

	// コピーと代入を禁止
	FramebufferObject(const FramebufferObject&) = delete;
	FramebufferObject& operator=(const FramebufferObject&) = delete;

	// フレームバッファの管理番号を取得
	operator GLuint() const { return fbo; }

	// テクスチャの管理番号を取得
	const TexturePtr& GetColorTexture() const { return texColor; }
	const TexturePtr& GetDepthTexture() const { return texDepth; }
	std::string name = "";
private:
	GLuint fbo = 0; // フレームバッファオブジェクトのID
	TexturePtr texColor; // カラーテクスチャ
	TexturePtr texDepth; // 深度テクスチャ
};