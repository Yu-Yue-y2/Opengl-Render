#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1)in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
struct VS_OUT{
    vec3 vs_normal;
    vec2 TexCoord;
    vec4 lightspace_pos;
    vec3 world_pos;
    vec3 view_point;
    vec4 light_config;
    mat3 tbn;
};
layout (location = 0) out VS_OUT vs_out;

uniform bool has_tbn;
uniform vec3 viewpos;
uniform vec4 config;
layout (std140) uniform Matrices
{
    mat4 m;
    mat4 mvp;
    mat4 normal_m;
    mat4 lightspace_mvp;
};

void main()
{
   gl_Position = mvp * vec4(aPos, 1.0);
   vs_out.TexCoord = aTexCoord;
   vs_out.lightspace_pos = lightspace_mvp * vec4(aPos, 1.0);
   vs_out.world_pos = vec3(m * vec4(aPos, 1.0));
   if(has_tbn)
   {
        vec3 t = normalize(vec3(vec4(tangent, 0.0)));
        vec3 b = normalize(vec3(vec4(bitangent, 0.0)));
        vec3 n = cross(t, b);
        vs_out.tbn = mat3(t,b,n);
   }
   vs_out.vs_normal = (normal_m * vec4(aNormal, 0.0)).xyz;
   vs_out.light_config = config;
   vs_out.view_point = viewpos;
}