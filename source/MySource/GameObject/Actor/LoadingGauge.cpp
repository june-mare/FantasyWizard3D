#include "LoadingGauge.h"
#include "DataStruct/useImGui.h"


LoadingGauge::LoadingGauge(VertexArrayPtr vao, VertexArrayPtr gaugevao,
	ShaderPtr shader, TexturePtr gauge, TexturePtr gaugeBar)
	:vao(vao),gaugeVao(gaugevao), shader(shader), texGauge(gauge), texGaugeBar(gaugeBar)
{
	shader->Use();
	
	glProgramUniform4f(shader->id, 2,viewSize.x,viewSize.y,-1,-1);
}

void LoadingGauge::Draw()
{	
	if (percentage == 1.0f)return;
	glBindTextures(0, 1, nullptr);
	glBindVertexArray(*vao);
	shader->Use();
	
	glProgramUniform3f(shader->id, 1,position.x,position.y,position.z);
	glProgramUniform4f(shader->id, 3,0,0,1,1);
	const GLuint gauge = *texGauge;
	glBindTextures(0, 1,&gauge);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindTextures(0, 1, nullptr);
		
	glBindVertexArray(*gaugeVao);
	glProgramUniform3f(shader->id, 1,100.f,position.y,position.z);
	glProgramUniform4f(shader->id, 3,0,0,percentage,1);
	const GLuint gaugeBar = *texGaugeBar;
	glBindTextures(0, 1, &gaugeBar);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);		
	glBindTextures(0, 1, nullptr);
}
