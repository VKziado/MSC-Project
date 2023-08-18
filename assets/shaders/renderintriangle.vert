#version 460

const vec2 inPos[3]={{-1,-1},{3,-1},{-1,3}};

layout(location=0) out vec2 vs_texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
    gl_Position=vec4(inPos[gl_VertexID],0,1);
    vs_texcoord = inPos[gl_VertexID] * 0.5 + 0.5;
}