#version 460

layout(location = 0) in vec3 inPos;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec4 outColor;

out gl_PerVertex
{
	vec4 gl_Position;
};
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
  gl_Position =  P*V*M*vec4(inPos, 1);
  outColor=inColor;
}
