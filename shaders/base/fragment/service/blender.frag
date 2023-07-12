#version 450
#extension GL_EXT_nonuniform_qualifier : require


#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308




layout(set = 0, binding = 0) uniform sampler2D colors[]; //I delete Sampler2D directive
layout(set = 0, binding = 1) uniform sampler2D depths[]; //I delete Sampler2D directive

layout(push_constant) uniform PushConstants {
  int layers_cnt;
} pushConstants;



layout(location = 0) in vec2 texCoord;
layout(location = 1) in vec3 fragColor;

layout(location = 2) out vec4 outColor;

float maxDepth = FLT_MIN;

void main() {
    outColor = vec4(fragColor, 1.0);
    
    for (int i = 0; i < pushConstants.layers_cnt; i++) {
        vec4 color = texture(colors[i], texCoord); 
        float depth = texture(depths[i], texCoord).r; 
        if (maxDepth < depth){
            maxDepth = depth;
            outColor.r = min(outColor.r * color.a + color.r, FLT_MAX);
            outColor.g = min(outColor.g * color.a + color.g, FLT_MAX);
            outColor.b = min(outColor.b * color.a + color.b, FLT_MAX);
        }else{
            outColor.r = min(outColor.r + color.r * outColor.a, FLT_MAX);
            outColor.g = min(outColor.g + color.g * outColor.a, FLT_MAX);
            outColor.b = min(outColor.b + color.b * outColor.a, FLT_MAX);  
        }
        
        outColor.a = min(color.a + outColor.a, FLT_MAX);
        
    }
    
}
