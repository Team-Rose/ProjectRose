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

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

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

void main()
{
	vec4 texColor = Input.Color;
	texColor *= texture(u_Textures[0], Input.TexCoord * Input.TilingFactor);

	if(texColor.a == 0.0)
		discard;

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightPos = vec3(1.0, 2.0, 0.5);
	
	float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Input.Normal);
	vec3 lightDir = normalize(lightPos - Input.FragPos);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 1.0;
	vec3 viewDir = normalize(Input.ViewPos - Input.FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 1.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	

	color = vec4((specular * ambient * diffuse),1.0) * texColor;
	
	color2 = v_EntityID;
}