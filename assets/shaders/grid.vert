#version 450

out gl_PerVertex
{
	vec4 gl_Position;
};
layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;

layout(binding = 1) uniform ViewUniforms {
    mat4 view;
    mat4 proj;
    vec3 pos;
    float near;
    float far;
} view;

// Grid position are in xy clipped space
vec2 gridPlane[6] = vec2[](
    vec2(1, 1), vec2(-1, 1), vec2(-1, -1),
    vec2(-1, -1), vec2(1, -1),vec2(1, 1)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 viewInv, mat4 projInv) {
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec2 p = gridPlane[gl_VertexID];
    mat4 viewInv = inverse(view.view);
    mat4 projInv = inverse(view.proj);
    nearPoint = UnprojectPoint(p.x, p.y, -1.0,viewInv, projInv).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, viewInv, projInv).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 0 , 1.0); // using directly the clipped coordinates
}