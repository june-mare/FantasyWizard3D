/*
*AssimpAnimation.frag
*/
#version 450 core
out vec4 FragColor;
layout (location=0)in vec3 FragPos;
layout (location=1)in vec3 Normal;
layout (location=2)in vec2 TexCoords;
layout (location=3)flat in int MaterialID;
layout (location=4)in vec3 outShadowPosition;
//layout (location=5)in mat3 TBN;
//layout (location=6)in vec3 TotalNormal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
layout(binding=5)uniform sampler2DShadow texture_Depth;

struct Material{
   vec4 baseColor; 
   float roughness;	
   float shininess;
   float matallic;	
   vec3 specular;
};

struct DirctionLight{
    vec3 direction; 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
    vec3 direction;		
    vec3 position;		
    float cutOff;		
    float outerCutOff;	
    vec3 ambient;
    vec3 diffuse;		
    vec3 specular;		
    float constant;		
    float linear;	
    float quadratic;	
};

struct Camera
{
   vec3 position;
   //float dammy;
   mat4 projection;
   mat4 view;
};

layout(std430,binding=3)readonly buffer MaterialDataBlock
{
  Material materialList[];
};

layout(std430,binding=4)readonly buffer DirLightDataBlock
{
  DirctionLight dirlight;
};

layout(std430,binding=5)readonly buffer PointLightDataBlock
{
  int pointlightCount;
  int pointdammy[3];
  PointLight pointlightList[];
};

layout(std430,binding=6)readonly buffer SpotLightDataBlock
{
  int spotlightCount;
  int spotdammy[3];
  SpotLight spotlightList[];
};

layout(std430,binding=7)readonly buffer cameraDataBlock
{
  Camera camera;
};

vec3 CalcDirLight(DirctionLight light, vec3 normal, vec3 viewDir,vec3 texDiff,float shadow);

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 texDiff,float shadow);

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 texDiff,float shadow);

void main()
{     
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camera.position - FragPos);

    vec3 result;
    float shadow = float(texture(texture_Depth,outShadowPosition));
    vec3 texDiff =  vec3(texture(texture_diffuse1, TexCoords));

    result = CalcDirLight(dirlight, norm, viewDir,texDiff,shadow);
    for(int i = 0;i < pointlightCount;++i){
       result += CalcPointLight(pointlightList[i], norm, FragPos,viewDir,texDiff,shadow);
    }
    for(int i = 0;i < spotlightCount;++i){
       result += CalcSpotLight(spotlightList[i], norm, FragPos,viewDir,texDiff,shadow);
    }

    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirctionLight light, vec3 normal, vec3 viewDir,vec3 texDiff,float shadow)
{
    vec3 lightDir = normalize(-light.direction.xyz);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
    materialList[MaterialID].shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * materialList[MaterialID].specular.x;;
    return ((diffuse + specular) * shadow + ambient);
} 

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 texDiff,float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
    materialList[MaterialID].shininess);
    // attenuation
    float dist    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + 
  			     light.quadratic * (dist * dist));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * materialList[MaterialID].specular.x;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return ((diffuse + specular) * shadow + ambient);
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 texDiff,float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
    materialList[MaterialID].shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction.xyz)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * materialList[MaterialID].specular.x;;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return ((diffuse + specular) * shadow + ambient);
}