#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

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

layout(location = 12) in uvec4 JOINTS_0;
layout(location = 13) in uvec4 JOINTS_1;
layout(location = 14) in uvec4 JOINTS_2;
layout(location = 15) in uvec4 JOINTS_3;

layout(location = 16) in vec4 WEIGHTS_0;
layout(location = 17) in vec4 WEIGHTS_1;
layout(location = 18) in vec4 WEIGHTS_2;
layout(location = 19) in vec4 WEIGHTS_3;



layout(location = 0) out vec3 POSITION_OUT;
layout(location = 1) out vec3 NORMAL_OUT;
layout(location = 2) out vec4 TANGENT_OUT;
layout(location = 3) out vec4 CUSTOM_OUT;

layout(location = 4) out vec2 TEXCOORD_0_OUT;
layout(location = 5) out vec2 TEXCOORD_1_OUT;
layout(location = 6) out vec2 TEXCOORD_2_OUT;
layout(location = 7) out vec2 TEXCOORD_3_OUT;

layout(location = 8) out vec4 COLOR_0_OUT;
layout(location = 9) out vec4 COLOR_1_OUT;
layout(location = 10) out vec4 COLOR_2_OUT;
layout(location = 11) out vec4 COLOR_3_OUT;

layout(location = 12) out uvec4 JOINTS_0_OUT;
layout(location = 13) out uvec4 JOINTS_1_OUT;
layout(location = 14) out uvec4 JOINTS_2_OUT;
layout(location = 15) out uvec4 JOINTS_3_OUT;

layout(location = 16) out vec4 WEIGHTS_0_OUT;
layout(location = 17) out vec4 WEIGHTS_1_OUT;
layout(location = 18) out vec4 WEIGHTS_2_OUT;
layout(location = 19) out vec4 WEIGHTS_3_OUT;

layout(set = 0, binding = 0) uniform sampler2D TEXTURE_0_SOURCE[];
layout(set = 0, binding = 1) uniform sampler2D TEXTURE_1_SOURCE[];
layout(set = 0, binding = 2) uniform sampler2D TEXTURE_2_SOURCE[];
layout(set = 0, binding = 3) uniform sampler2D TEXTURE_3_SOURCE[];

void main() {

    gl_Position = vec4(POSITION,1);
    POSITION_OUT = POSITION;
    NORMAL_OUT = NORMAL;
    COLOR_0_OUT = vec4(1,0,0,0.8);
}
