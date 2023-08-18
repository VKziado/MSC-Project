#version 450
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;
struct VS_OUT {
    vec3 position;
    vec4 color;
    vec3 normal;
    vec2 texcoord;
};
layout(location = 0) in VS_OUT fs_in;

layout(binding = 1) uniform UBOVP {
    mat4 V;
    mat4 P;
    vec3 eyePos;
};

layout(binding = 2) uniform UBOLight {
    vec3 color;
    float intensity;
    vec3 direction;
}
uboLight;

layout(binding = 3) uniform UBOMaterial {
    vec4 baseColor;
    float shininess;
    int baseColorTexIndex;
    int normalTexIndex;
};
layout(binding = 0) uniform sampler2D tex[8];
void main() {
    // diectional light
    vec3 V = normalize(eyePos - fs_in.position);
    vec3 L = normalize(uboLight.direction);
    vec3 H = normalize(L + V);
    vec3 N = fs_in.normal;
    float NdotL = max(0, dot(N, L));
    float NdotH = max(0, dot(N, H));
    vec4 albedo = baseColor;
    if (baseColorTexIndex >= 0) albedo = texture(tex[baseColorTexIndex], fs_in.texcoord);
    vec3 ambientColor = vec3(0.1) * albedo.rgb;
    outColor =
        vec4(uboLight.color * uboLight.intensity * (albedo.rgb * NdotL + pow(NdotH, shininess)) + ambientColor, 1);
    outNormal = vec4(N,1);
}