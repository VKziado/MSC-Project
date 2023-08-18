#version 460
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D inColor;
layout(binding = 1) uniform sampler2D inAO;

layout(location = 0) in vec2 fs_texcoord;


layout(binding = 0) uniform ShadingParams
{
    int shadingMode;
};

void main() { 
    // fragColor = vec4(texture(inColor, fs_texcoord).rgb * texture(inAO, fs_texcoord).r, 1);
    switch(shadingMode)
    {
        case 0:
            fragColor = vec4(textureLod(inColor, fs_texcoord, 0).rgb, 1);
        break;
        case 1:
            fragColor = vec4(textureLod(inAO, fs_texcoord, 0).rrr, 1);
        break;
        case 2:
            fragColor = vec4(textureLod(inColor, fs_texcoord, 0).rgb*textureLod(inAO, fs_texcoord, 0).r, 1);
        break;
    }
}
