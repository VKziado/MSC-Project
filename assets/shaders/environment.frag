#version 450
layout(location = 0) out vec4 outColor;
struct VS_OUT
{
	vec3 position;	
	vec4 color;
	vec3 normal;
	vec2 texcoord;
};
layout(location = 0) in VS_OUT fs_in;

layout(binding=1) uniform UBOVP
{
	mat4 V;
	mat4 P;
	vec3 eyePos;
};

layout(binding=3) uniform UBOMaterial
{
	vec4 baseColor;
	float shininess;
	int baseColorTexIndex;
	int useVertexColor;
};
layout(binding=4) uniform UBOEnv
{
    vec3 ambientColor;
    float ambientIntensity;
};

layout(binding=0) uniform sampler2D tex[8];
void main()
{
	//diectional light
	vec3 V=normalize(eyePos-fs_in.position);
	vec3 N=fs_in.normal;
	vec4 albedo=baseColor;
	if(useVertexColor>0)
		albedo=fs_in.color;	
	if(baseColorTexIndex>=0)
		albedo=texture(tex[baseColorTexIndex],fs_in.texcoord);
    vec3 col=pow(ambientIntensity*ambientColor*albedo.rgb,vec3(0.4545));
	outColor=vec4(col,1);
}