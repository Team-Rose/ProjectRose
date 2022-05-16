#type vertex
#version 450 core

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

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
	
	DirLight u_DirLight;
	int u_EntityID;
};

struct VertexOutput
{
	vec3 Normal;
	vec3 FragPos;
	vec3 ViewPos;
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};


layout (location = 0) out VertexOutput Output;
layout (location = 6) out flat int v_EntityID;

void main()
{
	Output.Normal =  mat3(transpose(inverse(u_Transform))) * a_Normal;
	Output.FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
	Output.ViewPos = u_ViewPosition;

	Output.Color = vec4(1.0,1.0,1.0,1.0);
	Output.TexCoord = a_TexCoord;
	Output.TilingFactor = 1;
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

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

layout(std140, binding = 1) uniform ObjectAndSceneData
{
	mat4 u_Transform;
	vec4 u_Color;
	
	DirLight u_DirLight;
	int u_EntityID;
};

layout(std140, binding = 2) uniform ShaderProps
{
	float u_Shininess;
};

struct VertexOutput
{
	vec3 Normal;
	vec3 FragPos;
	vec3 ViewPos;
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 6) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(u_Textures[0], Input.TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Textures[0], Input.TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(u_Textures[1], Input.TexCoord));
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 norm = normalize(Input.Normal);
    vec3 viewDir = normalize(Input.ViewPos - Input.FragPos);

    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);
	

	color = vec4(result,1.0);
	



	color2 = v_EntityID;
}


