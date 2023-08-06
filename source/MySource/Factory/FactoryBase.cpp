#include "FactoryBase.h"
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_impl_glfw.h"
#include "../../ImGui/imgui_impl_opengl3.h"

void FactoryBase::Initialize() noexcept
{
}

void FactoryBase::Execute(float deltaTime) noexcept
{

}

void FactoryBase::Finalize() noexcept
{
}

void FactoryBase::Debug() noexcept
{
}

void FactoryBase::StateExecute(float deltaTime) noexcept
{
	state->Execute(deltaTime);
}

void FactoryBase::DebugAll() noexcept
{	
	if (ImGui::CollapsingHeader(this->name.c_str(), debugCheck))
	{
		this->Debug();
		this->state->Debug();
	}
}
