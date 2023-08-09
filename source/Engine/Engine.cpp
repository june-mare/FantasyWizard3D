#include "Engine.h"
#include "Time/Time.h"
#include "System/InputSystem.h"
#include "DataStruct/ImGuiTextGlyphRange.h"
#include "Window/Viewport.h"
#include "Shader/Shader.h"
#include "GameObject/ObjectManager.h"
#include "GameObject/ObjectTag.h"
#include "Debug/MyDebugImGui.h"
#include "Scene/TitleScene.h"

#define DEBUG

size_t CalcSsboSize(size_t n)
{
	/*
	n�ُ킩��256�̔{����Ԃ�
	OpenGL�ł�SSBO�̃A���C�����g�̍ő�l��256�ƌ��߂��Ă���
	*/
	const size_t alignment = 256;	/*�A���C�����g�i�f�[�^�̋��E���j*/
	return ((n + alignment - 1) / alignment) * alignment;
	return size_t();
}

void CursorposCallback(GLFWwindow* window, double x, double y) {
	ObjectManager& manager = ObjectManager::Instance();
	auto camera = manager.GetActiveCamera();
	if (camera == nullptr)return;
	camera->ProcessMouseMovement(static_cast<float>(x), static_cast<float>(y));

	Viewport& viewport = Viewport::Instance();
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if (mouseX > viewport.GetViewSizeX() - 5) {
		glfwSetCursorPos(window, 0,mouseY);
		camera->SetOldMouseX(0);
	}
	if (mouseX < 5) {
		glfwSetCursorPos(window, viewport.GetViewSizeX(), mouseY);
		camera->SetOldMouseX(static_cast<float>(viewport.GetViewSizeX()));
	}
}

void ScrollCallback(GLFWwindow* const window, double x, double y) {
	ObjectManager& manager = ObjectManager::Instance();
	auto camera = manager.GetActiveCamera();
	if (camera == nullptr)return;
	camera->ProcessMouseScroll(static_cast<float>(y));
}

int Engine::Run()
{
	int ret = Initialize();

	if (!ret) {
		MainLoop();
	}
	Finalize();
	return ret;
}

int Engine::Initialize()
{
	GLFW_WINDOW::Window& window = GLFW_WINDOW::Window::Instance();
	Viewport& viewport = Viewport::Instance();
	window.Init(viewport.GetViewSizeX(), viewport.GetViewSizeY(), projectName.c_str());
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		return GL_TRUE;

	/*�V�X�e���̍쐬*/
	{
		InputSystem::Initialize("resource/txt/Input.txt");
		TAG::Initialize("resource/txt/Input.txt");
		STATUS::StatusManager::GetInstance().RegistStatusFromFile("resource/txt/Input.txt", "player");
		Time::Initialize();
	}

	/*Sprite��VAO�쐬*/
	{
		CreateVAO(-0.5f, -0.5f, 0.5f, 0.5f, vbo, ibo, vao);
		CreateVAO(0.f, -0.5f, 1.f, 0.5f, gaugeVbo, gaugeIbo, gaugeVao);
	}

	/*SSBO�̍쐬*/
	{
		ssboSprite = MappedBufferObject::Create(
			CalcSsboSize(500 * sizeof(Sprite)),
			GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

		ssboAnim = MappedBufferObject::Create(
			CalcSsboSize(500 * sizeof(AnimData)),
			GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

		ssboMesh = MappedBufferObject::Create(
			CalcSsboSize(500 * sizeof(MeshData)),
			GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

		ssboCamera = MappedBufferObject::Create(
			CalcSsboSize(5 * sizeof(CameraData)),
			GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	}

	/*Shader�̍쐬�Ə����e�N�X�`���̍쐬*/
	{
		CreateShader("sprite", "resource/shader/standard_2D.vert", "resource/shader/standard_2D.frag");
		CreateShader("model", "resource/shader/AssimpModel.vert", "resource/shader/AssimpModel.frag");
		CreateShader("anim", "resource/shader/AssimpAnimation.vert", "resource/shader/AssimpModel.frag");
		CreateShader("collision", "resource/shader/Collision.vert", "resource/shader/Collision.frag");
		//Bloom�p�V�F�[�_
		auto shaderH = CreateShader("HPF", "resource/shader/simple2D.vert", "resource/shader/high_pass_filter.frag");
		auto shaderD = CreateShader("downSampling", "resource/shader/simple2D.vert", "resource/shader/down_sampling.frag");
		auto shaderU = CreateShader("upSampling", "resource/shader/simple2D.vert", "resource/shader/up_sampling.frag");
		//Blur�p�V�F�[�_
		auto shaderB = CreateShader("Blur", "resource/shader/standard_2D.vert", "resource/shader/gaussian_blur.frag");
		//Shadow�p�V�F�[�_
		CreateShader("shadow", "resource/shader/shadow.vert", "resource/shader/shadow.frag");
		CreateShader("shadowAnim", "resource/shader/shadowAnim.vert", "resource/shader/shadow.frag");
		auto shaderG = CreateShader("gauge", "resource/shader/gauge.vert", "resource/shader/gauge.frag");

		//FadeShader
		fadeShader = CreateShader("fade", "resource/shader/simple2D.vert", "resource/shader/fade.frag");

		texFadeColor = Texture::Create(
			"resource/textures/Loading.png", GL_LINEAR, Texture::Usage::for2D);
		texFadeRule = Texture::Create(
			"resource/textures/line_rule.png", GL_LINEAR, Texture::Usage::for3DLinear);

		auto texGauge = Texture::Create(
			"resource/textures/gaugeFrame.png", GL_LINEAR, Texture::Usage::for2D);

		auto texGaugeBar = Texture::Create(
			"resource/textures/gaugeBar.png", GL_LINEAR, Texture::Usage::for2D);

		loadGauge = std::make_shared<LoadingGauge>(vao, gaugeVao, shaderG, texGauge, texGaugeBar);

		const glm::mat4 matBloomM = {
			glm::vec4(2, 0, 0, 0),
			glm::vec4(0,-2, 0, 0),
			glm::vec4(0, 0, 2, 0),
			glm::vec4(0, 0, 0, 1),
		};
		const glm::mat4 matBloomVP = glm::mat4(1);
		const GLuint bloomShaderIDs[]{
			shaderH->id,
			shaderD->id,
			shaderU->id,
			fadeShader->id,
		};
		for (auto prog : bloomShaderIDs) {
			glProgramUniformMatrix4fv(prog, 0, 1, GL_FALSE, &matBloomM[0][0]);
			glProgramUniformMatrix4fv(prog, 1, 1, GL_FALSE, &matBloomVP[0][0]);
		}
	}

	/*FBO�̍쐬*/
	{
		// �Q�[���E�B���h�E�pFBO���쐬
		const glm::vec2 viewSize = viewport.GetViewSize();
		fboGameWindow = FramebufferObject::Create(
			static_cast<int>(viewSize.x),
			static_cast<int>(viewSize.y), FboType::colorDepth);

		//�V���h�E�pFBO
		fboShadow = FramebufferObject::Create(
			static_cast<int>(viewSize.x),
			static_cast<int>(viewSize.y), FboType::depth);

		// ���C����ʂ̃u���[���G�t�F�N�g�pFBO���쐬
		int bloomX = static_cast<int>(viewSize.x);
		int bloomY = static_cast<int>(viewSize.y);
		for (auto& element : fboBloomList) {
			bloomX /= 2;
			bloomY /= 2;
			element = FramebufferObject::Create(bloomX, bloomY, FboType::color);
		}
	}

	/*ImGui�J�n����*/
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(window.GetGLWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450");

		io.Fonts->AddFontFromFileTTF("resource/ttf/x12y16pxMaruMonica.ttf",
			26.f, nullptr, GetGlyphRangeJapanese());
	}

	/*�Q�[���}�l�[�W���[�̏����쐬*/
	{
		ObjectManager& manager = ObjectManager::Instance();
		manager.CreateLighting();
		manager.CreateMaterial();
	}

	/*�^�C�g���V�[���̏����ݒ�*/
	{
		scene = std::make_shared<TitleScene>();
		scene->Initialize();
	}

	/*�g�p����R�[���o�b�N�֐�*/
	{
		glfwSetScrollCallback(window.GetGLWindow(), ScrollCallback);
		glfwSetCursorPosCallback(window.GetGLWindow(), CursorposCallback);
	}

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	return NULL;
}

int Engine::MainLoop()
{
	GLFW_WINDOW::Window& window = GLFW_WINDOW::Window::Instance();
	Viewport& viewport = Viewport::Instance();
	ObjectManager& manager = ObjectManager::Instance();

	auto meshShader = GetShader("model");
	auto animShader = GetShader("anim");

	while (!window.ShouldClose()) {

		//�ꎞ�I�����I������
		if (InputSystem::GetKey(GLFW_KEY_ESCAPE)) {
			break;
		}

		glfwPollEvents();
		Time::Execute();

		/*ImGui�t���[���J�n����*/
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		//��~���̓Q�[���I�u�W�F�N�g�̏��������s���Ȃ�
		if (!stopManager.ActiveCheck())goto StopOrSlowry;
		if (!slowryManager.ActiveCheck())goto StopOrSlowry;

		/*�Q�[���X�V����*/
		{
			//�V�[���̐؂�ւ�
			if (nextScene) {
				if (fadeState == FadeState::closed) {
					nextScene->Loading();
					scene = std::move(nextScene);
				}
				else
					StartFadeOut();
			}
			//�V�[���̍X�V
			if (scene) {
				if (!scene->isLoad) {
					scene->Loading();
					loadGauge->SetPercentage(scene->GetLoadPercentage());
				}
				else if (fadeState == FadeState::open)
					scene->Execute();
				/*�Q�[���I�u�W�F�N�g�X�V*/
				manager.Execute();		//���C�e�B���O�Amaterial�A�߂ނ��ҁ[
			}
		}

		/*��~���̔�΂����ʒu*/
	StopOrSlowry:

		/*�f�o�b�O*/
		{
#ifdef DEBUG
			DEBUG_IMGUI::DebugManager& debug = DEBUG_IMGUI::DebugManager::Instance();
			if (InputSystem::GetKey(GLFW_KEY_0)) {
				if(debug.SwitchingDebug())
					glfwSetInputMode(window.GetGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				else
					glfwSetInputMode(window.GetGLWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			debug.Execute("resource/txt/Input.txt");
#endif // DEBUG
		}

		//��ʏ�������
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, *fboGameWindow);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, *fboShadow);
			glClear(GL_DEPTH_BUFFER_BIT);
			for (auto& bloom : fboBloomList) {
				glBindFramebuffer(GL_FRAMEBUFFER, *bloom);
				glClear(GL_COLOR_BUFFER_BIT);
			}
		}

		/*�������R�s�[&Bind*/
		{
			MemcpyObject();
			MemcpySprites();
			/*lighting,camera,material Bind*/
			BindSSBO();
		}

		/*�����_�����O*/
		{
			// �`�����Q�[���E�B���h�E�ɕύX
			glBindFramebuffer(GL_FRAMEBUFFER, *fboGameWindow);
			viewport.LoadScreenSize();

			/*�X�J�C�{�b�N�X�`��*/
			DrawSkyBox();

			/*�e(�`�悷��Əd����)*/
			{
			/*	glm::mat4 shadowMap = CreateDepthMap();
				const GLuint texDepth = *fboShadow->GetDepthTexture();
				
				glBindTexture(GL_TEXTURE_SHADOW, texDepth);
				glBindTextureUnit(5, texDepth);
				const glm::mat4 matTexture = {
					glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
					glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
					glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
					glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
				};
				const glm::mat4 matShadowTexture = matTexture * shadowMap;
				
				glUseProgram(meshShader->id);
				meshShader->SetMat4("shadowMat", matShadowTexture);
				
				glUseProgram(animShader->id);
				animShader->SetMat4("shadowMat", matShadowTexture);
				glBindFramebuffer(GL_FRAMEBUFFER, *fboGameWindow);
				viewport.LoadScreenSize();
			*/
			}

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			/*3D�I�u�W�F�N�g�̕`��p*/
			Draw3D(meshShader,animShader);


			//BloomEffect��FBO�Ɍv�Z
			glBindVertexArray(*vao);
			if (manager.GetActiveCamera() != nullptr) {
				DrawBloomEffect();
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			
			//�t���[���o�b�t�@��ʏ�ɖ߂�
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			viewport.LoadScreenSize();

			/*z���W�n�Ɋ֌W�Ȃ����̕`��*/
			ssboOffset = 0;
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			if (!manager.GetGameObjectList().empty())
				DrawGameWindow(viewport.GetViewSize());

			DrawUI();

			/*�V�[���J�ڃG�t�F�N�g(UI�����O�ʂɏo��)*/
			if (fadeState != FadeState::open) {
				DrawFadeEffect();
				if (fadeState == FadeState::closed)
					loadGauge->Draw();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		/*Unbind*/
		UnbindSSBO();

		/*�t���[���I������*/
		{
			Time::Wait();
			window.SwapBuffers();
		}
	}

	return NULL;
}

int Engine::Finalize()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	InputSystem::Finalize();
	TAG::Finalize();
	return NULL;
}

void Engine::MemcpyObject()
{
	ObjectManager& manager = ObjectManager::Instance();
	std::shared_ptr<Camera> camera = manager.GetActiveCamera();
	if (camera == nullptr)return;

	Viewport& viewport = Viewport::Instance();
	/*������J�����O*/
	Frustum camFrustum = createFrustumFromCamera(*camera,
		viewport.GetWindowAspect(), glm::radians(camera->Zoom), camera->Near, camera->Far);

	int displayCount = 0;
	int totalCount = 0;

	/*�`��f�[�^���擾*/
	const auto& mapList = manager.GetMap();
	for (const auto& map : mapList) {
		map->PutDrawData(camFrustum, displayCount, true);
	}
	const auto& objectList = manager.GetGameObjectList();
	for (const auto& gameObject : objectList) {
		gameObject->PutDrawData(camFrustum, displayCount, true);
	}
	instanceData.SetDataList();


#ifdef DEBUG
	DEBUG_IMGUI::DebugManager& debug
		= DEBUG_IMGUI::DebugManager::Instance();
	totalCount += static_cast<int>(mapList.size());
	totalCount += static_cast<int>(objectList.size());
	debug.SetDisplayDrawCount(displayCount);
	debug.SetTotalObjectCount(totalCount);
#endif // DEBUG

	/*�f�[�^���������ɃR�s�[*/
	{
		ssboMesh->WaitSync();
		uint8_t* p = ssboMesh->GetMappedAddress();
		for (const auto& meshdata : instanceData.meshDataList) {
			memcpy(p, meshdata.second.data(), meshdata.second.size() * sizeof(MeshData));
			p += CalcSsboSize(meshdata.second.size() * sizeof(MeshData));
		}
		ssboAnim->WaitSync();
		p = ssboAnim->GetMappedAddress();
		for (const auto& animdata : instanceData.animDataList) {
			memcpy(p, animdata.second.data(), animdata.second.size() * sizeof(AnimData));
			p += CalcSsboSize(animdata.second.size() * sizeof(AnimData));
		}
	}
}

void Engine::MemcpySprites()
{
	Viewport& viewport = Viewport::Instance();
	ObjectManager& manager = ObjectManager::Instance();

	spriteListPostProcess.clear();
	spriteListPostProcess.shrink_to_fit();
	/*�R�s�[��o�b�t�@��؂�ւ���*/
	ssboSprite->WaitSync();

	uint8_t* p = ssboSprite->GetMappedAddress();

	if (!manager.GetGameObjectList().empty()) {
		// �|�X�g�v���Z�X�p�X�v���C�g��ǉ�
		const glm::vec2 gameWindowPos = viewport.GetViewSize();
		spriteListPostProcess.push_back(
			Sprite{ gameWindowPos.x / 2 , gameWindowPos.y / 2, 1, 0, Texcoord{0, 1, 1, -1} });
		const size_t size = spriteListPostProcess.size() * sizeof(Sprite);
		memcpy(p, spriteListPostProcess.data(), size);
		p += CalcSsboSize(size);
	}

	auto layerList = manager.GetUILayer();
	for (const auto& layer : layerList) {
		memcpy(p, layer.spriteList.data(), layer.spriteList.size() * sizeof(Sprite));
		p += CalcSsboSize(layer.spriteList.size() * sizeof(Sprite));
	}
}

void Engine::DrawSkyBox()
{
	ObjectManager& manager = ObjectManager::Instance();
	std::shared_ptr<Camera> camera = manager.GetActiveCamera();

	auto skybox = manager.GetSkyBox();
	if (skybox != nullptr) {
		skybox->Draw(*camera);
	}
}

void Engine::Draw3D(ShaderPtr shaderMesh, ShaderPtr shaderAnim,bool shadow)
{
	//�`��
	DrawMeshs(shaderMesh,shadow);
	DrawAnims(shaderAnim,shadow);

	glBindTextures(0, 1, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Engine::DrawSpriteList(const SpriteList& spriteList, GLuint tex, size_t& spriteSsboOffset)
{
	/*�`��f�[�^���Ȃ���΃X�L�b�v*/
	const size_t size = CalcSsboSize(spriteList.size() * sizeof(Sprite));
	if (size == 0)
		return;
	/*�p�b�`���w�肷��*/
	glBindTextures(0, 1, &tex);

	/*�X�v���C�g�pSSBO�����蓖�Ă�*/
	ssboSprite->Bind(0, spriteSsboOffset, size);

	/*vec3�^�̃��jf�O���ϐ��Ƀf�[�^���R�s�[���܂�*/
	glDrawElementsInstanced(GL_TRIANGLES,
		6,		//�C���f�b�N�X�̐�
		GL_UNSIGNED_SHORT, 0,
		static_cast<GLsizei>(spriteList.size()));

	spriteSsboOffset += size;
}

void Engine::DrawMeshs(ShaderPtr shader,bool shadow)
{
	glUseProgram(shader->id);
	size_t ssboOffset{};
	for (auto itr : instanceData.meshDataList) {

		/*�`��f�[�^���Ȃ���΃X�L�b�v*/
		const size_t size = CalcSsboSize(itr.second.size() * sizeof(MeshData));
		if (size == 0)
			return;

		itr.first.Draw(*shader, shadow);
		/*�X�v���C�g�pSSBO�����蓖�Ă�*/
		ssboMesh->Bind(2, ssboOffset, size);

		/*vec3�^�̃��jf�O���ϐ��Ƀf�[�^���R�s�[���܂�*/
		glDrawElementsInstanced(GL_TRIANGLES,
			static_cast<unsigned int>(itr.first.indices.size()),		//�C���f�b�N�X�̐�
			GL_UNSIGNED_INT, 0,
			static_cast<GLsizei>(itr.second.size()));

		ssboOffset += size;

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
}

void Engine::DrawAnims(ShaderPtr shader, bool shadow)
{
	glUseProgram(shader->id);
	size_t ssboOffset{};
	for (auto itr : instanceData.animDataList) {

		/*�`��f�[�^���Ȃ���΃X�L�b�v*/
		const size_t size = CalcSsboSize(itr.second.size() * sizeof(AnimData));
		if (size == 0)
			return;

		itr.first.Draw(*shader, shadow);
		/*�X�v���C�g�pSSBO�����蓖�Ă�*/
		ssboAnim->Bind(1, ssboOffset, size);

		/*vec3�^�̃��jf�O���ϐ��Ƀf�[�^���R�s�[���܂�*/
		glDrawElementsInstanced(GL_TRIANGLES,
			static_cast<unsigned int>(itr.first.indices.size()),		//�C���f�b�N�X�̐�
			GL_UNSIGNED_INT, 0,
			static_cast<GLsizei>(itr.second.size()));

		ssboOffset += size;

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
}

void Engine::DrawUI()
{
	ObjectManager& manager = ObjectManager::Instance();
	Viewport& viewport = Viewport::Instance();
	glUseProgram(GetShader("sprite")->id);
	// �Q�[���E�B���h�E�p�̃X�v���C�g�f�[�^��SSBO�ɃR�s�[
	auto layerList = manager.GetUILayer();

	const glm::vec2 viewSize = viewport.GetViewSize();
	glProgramUniform4f(*GetShader("sprite"), 2, viewSize.x, viewSize.y, -1.f, -1.f);
	/*�X�v���C�g�z���`��*/
	size_t ssboOffset = 0;
	for (const auto& layer : layerList) {
		DrawSpriteList(layer.spriteList, *layer.tex, ssboOffset);
	}

	//glBindVertexArray(0);
}

void DrawTextureToFbo(FramebufferObject& fbo,
	Texture& texture, GLenum wrapMode)
{
	const Texture& texFbo = *fbo.GetColorTexture();
	glViewport(0, 0, texFbo.width, texFbo.height);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	texture.SetWrapMode(wrapMode);
	glBindTextureUnit(0, texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void Engine::DrawBloomEffect()
{
	// �u���[���G�t�F�N�g�p��GL�R���e�L�X�g��ݒ�
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// 1. �Q�[����ʗpFBO���獂�P�x�����𒊏o
	auto shader = GetShader("HPF");
	glUseProgram(shader->id);
	glDisable(GL_BLEND); // �����Ȃ�
	{
		// ���P�x������FBO�̖����������Z�����̂ŁA���̉e����ł��������߂ɖ����Ŋ���
		const float strength = bloomStrength / static_cast<float>(std::size(fboBloomList));
		glProgramUniform2f(shader->id, 100, bloomThreshold, strength);
		DrawTextureToFbo(*fboBloomList[0], *fboGameWindow->GetColorTexture(),
			GL_CLAMP_TO_BORDER);
	}

	// 2. �k���ڂ������s���A1�i������FBO�ɃR�s�[
	shader = GetShader("downSampling");
	glUseProgram(shader->id);
	for (size_t i = 1; i < std::size(fboBloomList); ++i) {
		DrawTextureToFbo(*fboBloomList[i], *fboBloomList[i - 1]->GetColorTexture(),
			GL_CLAMP_TO_BORDER);
	}


	// 3. �g��ڂ������s���A1�i�傫��FBO�ɉ��Z����
	shader = GetShader("upSampling");
	glUseProgram(shader->id);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE); // ���Z����
	for (size_t i = std::size(fboBloomList) - 1; i > 0; ) {
		--i;
		DrawTextureToFbo(*fboBloomList[i], *fboBloomList[i + 1]->GetColorTexture(),
			GL_CLAMP_TO_EDGE);
	}

	// 4. �Ō�̊g��ڂ������s���A�Q�[����ʗpFBO�ɉ��Z����
	{
		DrawTextureToFbo(*fboGameWindow, *fboBloomList[0]->GetColorTexture(),
			GL_CLAMP_TO_EDGE);
	}

}

void Engine::DrawFadeEffect()
{
	Viewport& viewport = Viewport::Instance();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	viewport.LoadScreenSize();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(*vao);
	glUseProgram(fadeShader->id);;

	// �e�N�X�`����OpenGL�R���e�L�X�g�Ɋ��蓖�Ă�
	const GLuint tex[] = { *texFadeColor, *texFadeRule };
	glBindTextures(0, 2, tex);

	// �������l��GPU�������ɃR�s�[
	if (fadeState == FadeState::fadein) {
		const float fadeThreshold = (fadeTimer / fadeTotalTime);
		glProgramUniform1f(*fadeShader, 100, fadeThreshold);
	}
	else if (fadeState == FadeState::fadeout) {
		const float fadeThreshold = 1.f - (fadeTimer / fadeTotalTime);
		if (0.05f > fadeThreshold)
			glProgramUniform1f(*fadeShader, 100, 0.f);
		else
			glProgramUniform1f(*fadeShader, 100, fadeThreshold);
	}
	if (fadeState == FadeState::closed) {
		glProgramUniform1f(*fadeShader, 100, 0.f);
	}

	// �G�t�F�N�g��`��
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glBindTextures(0, 1, nullptr);

	// �^�C�}�[�̍X�V
	if (fadeState != FadeState::closed) {
		fadeTimer += Time::deltaTime;
	}

	// �^�C�}�[�������쎞�Ԉȏ�ɂȂ������Ԃ��X�V
	if (fadeTimer > fadeTotalTime) {
		fadeTimer = fadeTotalTime;
		// �������l�̕ω��������u0����1�v�Ȃ�open�A����ȊO��closed��Ԃɂ���
		if (fadeFrom < fadeTo)
			fadeState = FadeState::open;
		else
			fadeState = FadeState::closed;
	}
}

void Engine::DrawGameWindow(glm::vec2 viewSize)
{
	// �ڂ������x��0���傫����΂ڂ����V�F�[�_���g��
	if (blurStrength > 0) {
		glUseProgram(GetShader("Blur")->id);	
		glProgramUniform4f(GetShader("Blur")->id, 2,
			viewSize.x, viewSize.y, -1, -1);
		glProgramUniform1f(GetShader("Blur")->id, 100, blurStrength);
		DrawSpriteList(spriteListPostProcess,
			*fboGameWindow->GetColorTexture(), ssboOffset);
	}
	else {
		glUseProgram(GetShader("sprite")->id);		
		glProgramUniform4f(GetShader("sprite")->id, 2,
			static_cast<float>(viewSize.x),
			static_cast<float>(viewSize.y), -1, -1);
		DrawSpriteList(spriteListPostProcess,
			*fboGameWindow->GetColorTexture(), ssboOffset);
	} // blurStrength
}

void Engine::DrawCollision()
{
	ObjectManager& manager = ObjectManager::Instance();

	auto& objectList = manager.GetGameObjectList();
	for (const auto& object : objectList) {
		for (const auto& collision : object->colliderList) {
			collision->DebugDraw(GetShader("collision"), object->transform.getModelMatrix());
		}
	}

	objectList = manager.GetMap();
	for (const auto& object : objectList) {
		for (const auto& collision : object->colliderList) {
			collision->DebugDraw(GetShader("collision"), object->transform.getModelMatrix());
		}
	}

}

glm::mat4 Engine::CreateDepthMap()
{
	ObjectManager& manager = ObjectManager::Instance();
	auto camera = manager.GetActiveCamera();
	if (camera == nullptr)return glm::mat4();

	glBindFramebuffer(GL_FRAMEBUFFER, *fboShadow);

	const GLuint texDepth = *fboShadow->GetDepthTexture();
	GLint w, h;
	glGetTextureLevelParameteriv(texDepth, 0, GL_TEXTURE_WIDTH, &w);
	glGetTextureLevelParameteriv(texDepth, 0, GL_TEXTURE_HEIGHT, &h);
	glViewport(0, 0, w, h);

	glDisable(GL_BLEND); // �A���t�@�u�����h�𖳌���

	// �e�p�r���[�s����쐬
	const glm::mat4 matShadowView = camera->GetViewMatrix();

	// �e�p�v���W�F�N�V�����s����쐬
	const glm::mat4 matShadowProj = glm::ortho(
		-1500.f, 1500.f,
		-1500.f, 1500.f,
		camera->Near, camera->Far);

	// �e�p�r���[�v���W�F�N�V�����s����쐬
	const glm::mat4 matShadow = matShadowProj * matShadowView;

	auto shaderMesh = GetShader("shadow");
	auto shaderAnim = GetShader("shadowAnim");
	// matShadow�����j�t�H�[���ϐ��ɃR�s�[
	glUseProgram(shaderMesh->id);
	glProgramUniformMatrix4fv(shaderMesh->id, 1, 1, GL_FALSE, &matShadow[0][0]);
	
	glUseProgram(shaderAnim->id);
	glProgramUniformMatrix4fv(shaderAnim->id, 1, 1, GL_FALSE, &matShadow[0][0]);
	
	Draw3D(shaderMesh, shaderAnim);

	return matShadow;
}

void Engine::BindCamera()
{
	ObjectManager& manager = ObjectManager::Instance();
	auto camera = manager.GetActiveCamera();
	if (camera == nullptr) return;
	ssboCamera->WaitSync();
	uint8_t* p = ssboCamera->GetMappedAddress();

	camera->UpdateDatas();
	cameraData.position = camera->Position;
	cameraData.view = camera->view;
	cameraData.projection = camera->projection;

	memcpy(p, &cameraData.position.x, sizeof(CameraData));
	p += CalcSsboSize(sizeof(CameraData));

	const size_t size = CalcSsboSize(sizeof(CameraData));
	ssboCamera->Bind(7, 0, size);
}

void Engine::UnbindCamera()
{
	glBindBufferRange(ssboCamera->GetType(), 7, 0, 0, 0);
	ssboCamera->SwapBuffers();
}

void Engine::BindSSBO()
{
	/*�o�C���f�B���O*/
	ObjectManager& manager = ObjectManager::Instance();
	BindCamera();
	manager.BindLighting();
	manager.BindMaterial();
}

void Engine::UnbindSSBO()
{
	ObjectManager& manager = ObjectManager::Instance();
	instanceData.DataClear();/*�C���X�^���X�`��p�f�[�^�폜*/
	UnbindCamera();
	manager.UnbindLighting();
	manager.UnbindMaterial();
	glBindBufferRange(ssboAnim->GetType(), 1, 0, 0, 0);
	ssboAnim->SwapBuffers();
	glBindBufferRange(ssboMesh->GetType(), 2, 0, 0, 0);
	ssboMesh->SwapBuffers();
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 0, 0, 0);
	ssboSprite->SwapBuffers();
	glBindTextures(0, 1, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Engine::CreateVAO(float x, float y, float ax, float ay, BufferObjectPtr& vbo, BufferObjectPtr& ibo, VertexArrayPtr& vao)
{
	/*���_�f�[�^��GPU�������ɃR�s�[*/
	struct Vertex
	{
		glm::vec3 pos;/*���_���W*/
		glm::vec2 uv;   /*�e�N�X�`��*/
	};
	const Vertex vertexdata[] =
	{
		{ glm::vec3(x,y, 0.0f),glm::vec2(0, 1)},
		{ glm::vec3(ax,y, 0.0f), glm::vec2(1, 1)},
		{ glm::vec3(ax, ay, 0.0f), glm::vec2(1, 0)},
		{ glm::vec3(x, ay, 0.0f),glm::vec2(0, 0)},
	};
	vbo = BufferObject::Create(sizeof(vertexdata), vertexdata);


	/*�C���f�b�N�X�f�[�^��GPU�������ɃR�s�[*/
	/*�^�̓V���[�g*/
	const GLshort indexdata[] =
	{
		0,1,2,2,3,0,
	};
	ibo = BufferObject::Create(sizeof(indexdata), indexdata);

	/*���_�f�[�^�̍\������ݒ�*/
	{
		vao = VertexArray::Create();
		glBindVertexArray(*vao);
		glBindBuffer(GL_ARRAY_BUFFER, *vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
		/*�L���ɂ��钸�_�����̔z��C���f�b�N�X*/
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(const void*)offsetof(Vertex, uv.x));
		glBindVertexArray(0);
	}

}

void Engine::StartFadeIn(float time)
{
	if (fadeState == FadeState::fadein)return;
	// ��Ԃ��t�F�[�h���s���ɂ���
	fadeState = FadeState::fadein;

	// �G�t�F�N�g�ɂ����鎞�Ԃ�ݒ肵�A�o�ߎ��Ԃ����Z�b�g
	fadeTotalTime = time;
	fadeTimer = 0;

	// �������l���u0����1�ɕω�����v�悤�ɐݒ�
	fadeFrom = 0;
	fadeTo = 1;
}

void Engine::StartFadeOut(float time)
{
	// ��Ԃ��t�F�[�h���s���ɂ���
	if (fadeState == FadeState::fadeout)return;
	fadeState = FadeState::fadeout;

	// �G�t�F�N�g�ɂ����鎞�Ԃ�ݒ肵�A�o�ߎ��Ԃ����Z�b�g
	fadeTotalTime = time;
	fadeTimer = 0;

	// �������l���u1����0�ɕω�����v�悤�ɐݒ�
	fadeFrom = 1;
	fadeTo = 0;
}

ShaderPtr Engine::CreateShader(std::string name, const char* vert, const char* frag, const char* geo)
{
	for (auto& itr : shaders) {
		if (itr->name == name)return itr;
	}
	auto shader = Shader::Create(vert, frag, geo);
	shader->name = name;
	shaders.push_back(shader);
	return shader;
}

ShaderPtr Engine::GetShader(std::string name)
{
	for (auto& itr : shaders) {
		if (itr->name == name)
			return itr;
	}
	return nullptr;
}

TexturePtr Engine::CreateTexture(std::string name, GLenum filterMode, Texture::Usage usage)
{
	for (auto& itr : textures) {
		if (itr->GetName() == name)
			return itr;
	}
	auto texture = Texture::Create(name, filterMode, usage);
	textures.push_back(texture);
	return texture;
}

TexturePtr Engine::GetTexture(std::string name)
{
	for (auto& itr : textures) {
		if (itr->GetName() == name)
			return itr;
	}
	return nullptr;
}

TextureList Engine::GetTextures()
{
	return textures;
}

std::shared_ptr<Model> Engine::LoadModel(std::string name, std::string path)
{
	auto itr = models.find(name);
	if (itr != models.end())return itr->second;

	auto model = std::make_shared<Model>(path.c_str());
	model->name = name;

	models[name] = model;
	return model;
}

std::shared_ptr<Model> Engine::GetModel(std::string name)
{
	auto itr = models.find(name);
	if (itr != models.end())return itr->second;

	return nullptr;
}

std::shared_ptr<AnimUseList> Engine::GetAnimUseList(std::string name)
{
	for (auto& itr : charaAnimData) {
		if (itr->GetName() == name)
			return itr;
	}
	return std::make_shared<AnimUseList>();
}

void Engine::SetAnimUseList(std::shared_ptr<AnimUseList>& dataList) {
	charaAnimData.push_back(dataList);
}

AnimationPtr Engine::CreateAnimation(const char* path, std::shared_ptr<Model> model, bool loop)
{
	for (auto& anim : animationList) {
		if (!std::strcmp(anim->GetName().c_str(), path)) {
			anim->SetLoop(loop);
			return anim;
		}
	}
	auto anim = std::make_shared<Animation>(path, model, loop);
	unsigned int id = static_cast<unsigned int>(animationList.size());
	anim->SetName(path);
	anim->SetId(id);
	animationList.push_back(anim);
	return anim;
}

AnimationPtr Engine::GetAnimation(unsigned int id)
{
	for (auto& itr : animationList)
	{
		if (id == itr->GetId())
			return itr;
	}
	return nullptr;
}

AnimationPtr Engine::CreateAnimation(const char* path, std::string model, bool loop)
{
	for (auto& anim : animationList) {
		if (!std::strcmp(anim->GetName().c_str(), path)) {
			anim->SetLoop(loop);
			return anim;
		}
	}
	auto m = GetModel(model);
	auto anim = std::make_shared<Animation>(path, m, loop);
	unsigned int id = static_cast<unsigned int>(animationList.size());
	anim->SetName(path);
	anim->SetId(id);
	animationList.push_back(anim);
	return anim;
}
