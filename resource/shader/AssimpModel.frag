#version 450 core
out vec4 FragColor;

layout (location=0)in vec3 FragPos;
layout (location=1)in vec3 Normal;
layout (location=2)in vec2 TexCoords;
layout (location=3)flat in int MaterialID;
layout (location=4)in vec3 outShadowPosition;

layout (binding=0)uniform sampler2D texture_diffuse1;
layout (binding=1)uniform sampler2D texture_specular1;
layout (binding=2)uniform sampler2D texture_normal1;
layout(binding=5)uniform sampler2DShadow texture_Depth;

struct Material{
    vec4 baseColor;	
    float roughness;	
    float metallic;	
    float shininess;	
    float emission;	
};

struct DirctionLight{
 vec3 direction; 
 vec3 color; 
};

struct PointLight{
    vec3 position;
    vec3 color;
    // x constant
    // y linear
    // z quadratic
    //vec3 pointParam;
};

struct SpotLight{
    vec3 direction;		
    vec3 position;
    vec3 color;
    //x cutOff
    //y outerCutOff
    vec2 cutOffParam;	
    // x constant
    // y linear
    // z quadratic
    vec3 pointParam;
};

struct Camera
{
   vec3 position;
   float dammy;
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

vec3 CalculateDirectionalLight(vec3 normal,vec3 viewDir,vec4 texColor,Material material);
vec3 CalculatePointLight(vec3 normal, vec3 viewDir,vec4 texDiff,Material material);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir,float specularFactor,vec4 texDiff,float shadow,Material material); 
void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camera.position - FragPos);
    Material material = materialList[MaterialID];  
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    vec3 ambient = vec3(0.4,0.2,0.4);
    
    //鏡面反射のファクター
    //float specularFactor = material.metallic * material.roughness;
    //float shadow = float(texture(texture_Depth,outShadowPosition));
     
    vec3 result = CalculateDirectionalLight(norm, viewDir,texColor,material);    
    
    result += CalculatePointLight(norm,viewDir,texColor,material);
    
//    for(int i = 0;i < spotlightCount;++i){
//       result += CalculateSpotLight(spotlightList[i],norm,viewDir,specularFactor,texColor,shadow,material);
//    }

    result *= ambient;
    result = clamp(result, 0.0, 1.0);
    result *= material.emission;
    FragColor = vec4(result,material.baseColor.a);
}

vec3 CalculateDirectionalLight(vec3 normal,vec3 viewDir,vec4 texColor,Material material)
{    
    // ライトの方向ベクトルを取得
    vec3 lightDirection = normalize(-dirlight.direction);
    
    // 拡散光の計算
    //theta 
    float diff = max(dot(normal, lightDirection), 0.0);
    
    vec3 diffuseColor = diff * dirlight.color * texColor.rgb;
    
    // スペキュラ光の計算
    vec3 halfVector = normalize(lightDirection + viewDir);
    //dotNH
    float specularIntensity = pow(max(dot(normal, halfVector), 0.0),material.shininess);
    
    vec3 specular = diffuseColor * specularIntensity;
    // フラグメントの色を計算 
    return specular + diffuseColor;// + specularColor));       
};

vec3 CalculatePointLight(vec3 normal, vec3 viewDir,vec4 texDiff,Material material)
{
    vec3 d = vec3(0);
    vec3 s = vec3(0);
    for(int i = 0;i < pointlightCount;++i){
         // ライトの方向ベクトルを取得
        //ポイントライトの届く距離を計算
        vec3 lightDirection = normalize(pointlightList[i].position - FragPos);
        float lengthSq = dot(lightDirection,lightDirection);
        
        float diff = 1;
        float specularIntensity = 1;
        if(lengthSq > 0){
        
            // 拡散光の計算
            vec3 direct = normalize(lightDirection);
            diff = max(dot(normal, direct), 0.0);
            
            // 鏡面反射の計算
            vec3 halfVector = normalize(direct + viewDir);
            specularIntensity = pow(max(dot(normal, halfVector), 0.0),material.shininess);            
        }

        float normalizeFactor = max(dot(normal, lightDirection), 0.0);
        float intensity = 1.0 / (1.0 + lengthSq); // 距離による明るさの減衰        
        vec3 color = pointlightList[i].color * diff * intensity;
        d += color;
        s += color * specularIntensity * normalizeFactor;       
    }
    const float invPi = 1 / acos(-1); // πの逆数
    d *= invPi;
    return  d + s;// + specularColor) ));
};

//vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir,float specularFactor,vec4 texDiff,float shadow,Material material)
//{
//    for(int i = 0;i < spotlightCount;++i){
//    vec3 lightDirection = normalize(light.position - FragPos);
//    float spotEffect = dot(lightDirection, normalize(-light.direction));
//
//    if (spotEffect > cos(radians(light.outerCutOff))) {
//        float intensity = smoothstep(cos(radians(light.cutOff)), cos(radians(light.outerCutOff)), spotEffect);
//        
//        vec3 ambientColor = light.ambient;
//       
//        float diff = max(dot(normal, lightDirection), 0.0);
//        vec3 diffuseColor = light.diffuse * diff * texDiff.rgb;
//       
//        vec3 reflectionDirection = reflect(-lightDirection, normal);
//        float specularIntensity = pow(max(dot(viewDir, reflectionDirection), 0.0),material.shininess);
//        vec3 specularColor = light.specular * specularIntensity;
//        
//        return (ambientColor + (1.0 - spotEffect) * (diffuseColor + spotEffect * specularColor)) * intensity;
//    }
//    }
//    return vec3(0.0);
//};
