#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;
layout(location = 3) in vec4 inColor;

struct VS_OUT
{
	vec3 position;	
	vec4 color;
	vec3 normal;
	vec2 texcoord;
};
out gl_PerVertex
{
	vec4 gl_Position;
};
layout(location = 0) out VS_OUT vs_out;

layout(binding=0) uniform UBOM
{
	mat4 M;
};
layout(binding=1) uniform UBOVP
{
	mat4 V;
	mat4 P;
	vec3 eyePos;
};

void main() 
{
  vs_out.position=(M*vec4(inPos, 1)).xyz;
  gl_Position =  P*V*vec4(vs_out.position, 1);
  vs_out.color = inColor;
  vs_out.normal= mat3(M)*normalize(inNormal);
  vs_out.texcoord= inTexcoord;
}
