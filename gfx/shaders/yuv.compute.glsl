#version 430
layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba8, binding = 0) uniform image2D rgbImage;
layout(rgba8, binding = 1) uniform image2D yuvImage;

const float Kr = 0.299;
const float Kg = 0.587;
const float Kb = 0.114;

const mat3 RGB2YUV = mat3(Kr,
                          Kg,
                          Kb,
                          -0.5 * Kr / (1.0 - Kb),
                          -0.5 * Kg / (1.0 - Kb),
                          0.5,
                          0.5,
                          -0.5 * Kg / (1.0 - Kr),
                          -0.5 * Kb / (1.0 - Kr));

void main(void)
{
  vec4 rgb = imageLoad(rgbImage, ivec2(gl_GlobalInvocationID.xy));

  vec3 yuv = transpose(RGB2YUV) * rgb.rgb;

  imageStore(yuvImage,
             ivec2(gl_GlobalInvocationID.xy),
             vec4(yuv.x, yuv.y + 0.5, yuv.z + 0.5, 0));
}
