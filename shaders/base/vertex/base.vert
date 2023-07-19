#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;



layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    float angleX = radians(0.5); // Угол поворота по оси X в радианах
    float angleY = radians(0.5); // Угол поворота по оси Y в радианах
    
    mat4 model = mat4(
        1.000000, 0.000000, 0.000000, 0.000000,
        0.000000, 1.000000, 0.000000, 0.000000,
        0.000000, 0.000000, 1.000000, 0.000000,
        0.000000, 0.000000, 0.000000, 1.000000);
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


    mat4 X = mat4(
    1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    gl_Position = proj * view * model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}
