#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1)in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT{
    vec3 vs_normal;
    vec2 TexCoord;
    vec3 world_pos;
    vec3 view_point;
    vec4 light_config;
    mat3 tbn;
    vec3 t;
    vec3 b;
    vec3 n;
}vs_out;

uniform bool has_tbn;
uniform vec3 viewpos;
uniform vec4 config;
uniform mat4 m;
uniform mat4 vp;
uniform mat4 normal_m;


void main()
{
   gl_Position = vp * m * vec4(aPos, 1.0);
   vs_out.TexCoord = aTexCoord;
   vs_out.world_pos = vec3(m * vec4(aPos, 1.0));
   vs_out.vs_normal = normalize((normal_m * vec4(aNormal, 0.0)).xyz);
   vs_out.light_config = config;
   vs_out.view_point = viewpos;
   if(has_tbn)
   {
        vec3 t = normalize((normal_m * vec4(tangent, 0.0)).xyz);
        vec3 b = normalize((normal_m * vec4(bitangent, 0.0)).xyz);
        vec3 n = cross(t, b);
        vs_out.t = t;
        vs_out.b = b;
        vs_out.n = n;
        vs_out.tbn = mat3(t,b,n);
   }
}