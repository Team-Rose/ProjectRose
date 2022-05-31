#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_ViewPosition;
};

layout(std140, binding = 1) uniform ObjectAndSceneData
{
	mat4 u_Transform;
	vec4 u_Color;
	int u_EntityID;
};

layout (location = 0) out vec4 v_Color;
layout (location = 1) out vec2 v_TexCoord;
layout (location = 2) out float v_TilingFactor;

layout (location = 3) out vec3 v_Normal;
layout (location = 4) out vec3 v_FragPos;
layout (location = 5) out vec3 v_ViewPos;

layout (location = 6) out flat int v_EntityID;

void main()
{
	v_Normal =  mat3(transpose(inverse(u_Transform))) * a_Normal;
	v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
	v_ViewPos = u_ViewPosition;

	v_Color = vec4(1.0,1.0,1.0,1.0);
	v_TexCoord = a_TexCoord;
	v_TilingFactor = 1;
	v_EntityID = u_EntityID;

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_ViewPosition;
};

layout(std140, binding = 1) uniform ObjectAndSceneData
{
	mat4 u_Transform;
	vec4 u_Color;
	int u_EntityID;
};

layout(std140, binding = 2) uniform LightData
{
	DirLight u_DirLight;
    PointLight u_PointLight;
    SpotLight u_SpotLight;
};

layout(std140, binding = 3) uniform ShaderProps
{
	float u_Shininess;
};

layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoord;
layout (location = 2) in float v_TilingFactor;

layout (location = 3) in vec3 v_Normal;
layout (location = 4) in vec3 v_FragPos;
layout (location = 5) in vec3 v_ViewPos;

layout (location = 6) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32]; //0 = diffuse 1 = specular

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(u_Textures[0], v_TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Textures[0], v_TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_Textures[1], v_TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    // attenuation
    float distancee = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distancee + light.quadratic * (distancee * distancee));      
    // combine results
    vec3 ambient = light.ambient * vec3(texture(u_Textures[0], v_TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Textures[0], v_TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_Textures[1], v_TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(u_Textures[0], v_TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Textures[0], v_TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_Textures[1], v_TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(v_ViewPos - v_FragPos);

    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);
    // phase 2: point lights
    //for(int i = 0; i < 1; i++)
    result += CalcPointLight(u_PointLight, norm, v_FragPos, viewDir);    
    // phase 3: spot light
    CalcSpotLight(u_SpotLight, norm, v_FragPos, viewDir);    

	color = vec4(result,1.0);
	color2 = v_EntityID;
}


