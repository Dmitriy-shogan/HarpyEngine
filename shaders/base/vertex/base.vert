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



layout(location = 0) out vec3 fragColor;

void main() {
//     float angleX = radians(0.5); // Угол поворота по оси X в радианах
//     float angleY = radians(0.5); // Угол поворота по оси Y в радианах
//     

    mat4 view = mat4(
        -0.707107, -0.408248, 0.577350, 0.000000,
        0.707107, -0.408248, 0.577350, 0.000000,
        0.000000, 0.816497, 0.577350, 0.000000, 
        -0.000000, -0.000000, -3.464102, 1.000000);
    mat4 proj = mat4(
        0.750000, 0.000000, 0.000000, 0.000000,
        0.000000, -1.000000, 0.000000, 0.000000,
        0.000000, 0.000000, -1.020202, -1.000000,
        0.000000, 0.000000, -0.202020, 0.000000);
    
//     mat4 X = mat4(
//     1.0, 0.0, 0.0, 0.0,
//         0.0, 1.0, 0.0, 0.0,
//         0.0, 0.0, 1.0, 0.0,
//         0.0, 0.0, 0.0, 1.0
//     );

    gl_Position = vec4(POSITION, 1.0);
    fragColor = POSITION;
}
