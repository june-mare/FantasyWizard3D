/*
*Collision.frag
*/
#version 450

uniform vec4 color;

out vec4 outColor;

void main(){
   if(color.a < 0.2)discard;
   outColor = color;
}