#version 450

layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec3 NORMAL;
layout(location = 2) in vec4 TANGENT;
layout(location = 3) in vec4 CUSTOM;

layout(location = 4) in vec2 TEXCOORD_0;
layout(location = 5) in vec2 TEXCOORD_1;
layout(location = 6) in vec2 TEXCOORD_2;
layout(location = 7) in vec2 TEXCOORD_3;

layout(location = 8) in vec4 COLOR_0;
layout(location = 9) in vec4 COLOR_1;
layout(location = 10) in vec4 COLOR_2;
layout(location = 11) in vec4 COLOR_3;

layout(location = 16) in vec4 WEIGHTS_0;
layout(location = 17) in vec4 WEIGHTS_1;
layout(location = 18) in vec4 WEIGHTS_2;
layout(location = 19) in vec4 WEIGHTS_3;

layout(location = 0) out vec4 outColor;





void main() {


    //vec4 texColor = texture(textureSampler, fragTexCoord);
    outColor = COLOR_0;
    //outColor = vec4(texColor.rgb * COLOR_0, texColor.a)

}
