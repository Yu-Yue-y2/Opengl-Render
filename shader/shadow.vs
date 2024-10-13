#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 mvp;
//out vec3 offset;
void main()
{
    //vec4 pos = m * vec4(position, 1.0);
    //vec3 world_pos = pos.xyz / pos.w;
    //vec3 world_normal = (normal_m * vec4(aNormal, 0.0)).xyz;
    //float _cos = dot(world_normal, light_dir);
    //float _sin = (1 -  _cos);
    //float bias = (float(frustumSize) * _sin) / (float(shadowmapSize) * 2.0f);
   // world_pos -= world_normal * bias;
    //offset = world_normal * bias;
    gl_Position = mvp * vec4(position, 1.0f);
}