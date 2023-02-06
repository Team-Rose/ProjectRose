#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

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
layout(location = 1) out int entityid;

struct VertexOutput
{
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;

layout(std140, binding = 32) uniform Viewport
{
	int u_Samples;
};

layout (binding = 0) uniform sampler2D u_Screencapture[32];
layout (binding = 0) uniform isampler2D u_ScreencaptureI[32];
layout (binding = 0) uniform sampler2DMS u_ScreencaptureMS[32];
layout (binding = 0) uniform isampler2DMS u_ScreencaptureMSI[32];

vec4 MultiSampleTexture(sampler2DMS tex, ivec2 texCoord, int samples)
{
    vec4 result = vec4(0.0);
    for (int i = 0; i < samples; i++)
        result += texelFetch(tex, texCoord, i);

    result /= float(samples);
    return result;
}

void main()
{
	color = vec4(1.0, 0.0, 0.0, 1.0);
	entityid = -1;

	if(u_Samples != 1) {
		ivec2 texSize = textureSize(u_ScreencaptureMS[0]);
		ivec2 texCoord = ivec2(Input.TexCoord * texSize);

		color = MultiSampleTexture(u_ScreencaptureMS[0], texCoord, u_Samples);
		entityid = texelFetch(u_ScreencaptureMSI[1], texCoord, 0).r;
	} 
	if(u_Samples == 1)  {
		color = texture(u_Screencapture[0], Input.TexCoord);
		entityid = texture(u_ScreencaptureI[1], Input.TexCoord).r;
	}


}