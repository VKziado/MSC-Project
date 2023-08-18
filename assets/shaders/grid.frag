#version 450

layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform ViewUniforms {
    mat4 view;
    mat4 proj;
    vec3 pos;
    float near;
    float far;
} view;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    //vec4 color = vec4(vec3(min(line, 1.0)),1);
    // z axis
    float axisWidth=0.1;
    if(fragPos3D.x > -axisWidth * minimumx && fragPos3D.x < axisWidth* minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -axisWidth  * minimumz && fragPos3D.z < axisWidth * minimumz)
        color.x = 1.0;
    return color;
}
float computeDepth(vec3 pos) {
    vec4 clip_space_pos = view.proj * view.view * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w)*0.5+0.5;
}
float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = view.proj * view.view * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * view.near * view.far) / (view.far + view.near - clip_space_depth * (view.far - view.near)); // get linear value between 0.01 and 100
    return linearDepth / view.far; // normalize
}
void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));
    if(abs(fading)<1e-6)
        discard;

    outColor = (grid(fragPos3D, 10, true) + grid(fragPos3D, 1, true))* float(t > 0); // adding multiple resolution for the grid
    outColor.a *= fading;
    if(outColor.a<1e-6)
        discard;
}