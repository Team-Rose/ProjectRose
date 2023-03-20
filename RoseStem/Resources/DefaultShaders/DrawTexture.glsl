#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_ViewPosition;
};

struct VertexOutput
{
	vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 0.0, 1.0); 
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	color = texture(u_Textures[0], Input.TexCoord);
}