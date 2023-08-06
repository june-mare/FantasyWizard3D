#version 450
layout(location=0)in vec2 inTexcoord;

uniform sampler2D texColor;

out vec4 outColor;

void main(){
	outColor = texture(texColor,inTexcoord) ;
};