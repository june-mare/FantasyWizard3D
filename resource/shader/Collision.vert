/*
* Collision,vert
*/
#version 450
layout(location=0)in vec3 aPos;
uniform mat4 model;

struct Camera
{
   vec3 position;
   float dammy;
   mat4 projection;
   mat4 view;
};

layout(std430,binding=7)readonly buffer cameraDataBlock
{
  Camera camera;
};

void main(){
  gl_Position = camera.projection * camera.view * model * vec4(aPos,0.f);
}