#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 256) out;


in VS_OUT {
    vec3 vs_normal;
    vec2 TexCoord;
    vec3 world_pos;
    vec3 view_point;
    vec4 light_config;
    mat3 tbn;
    vec3 t;
    vec3 b;
    vec3 n;
} gs_in[];

out GS_OUT {
    vec3 color;
} gs_out;
uniform bool has_tbn;
uniform mat4 vp;
const float MAGNITUDE = 0.5;

uniform int emit_num;
uniform mat4 normal_m;
uniform sampler2D normal_map;

#define POINTLIGHT 1.f
#define DIRECTIONLIGHT 0.f

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = vp * vec4(gs_in[index].world_pos + gs_in[index].vs_normal * MAGNITUDE, 1.0);
    EmitVertex();
    EndPrimitive();
}
void GenerateLine(int index, vec3 dir,vec3 color,vec2 coor)
{
    vec3 uvb = vec3(coor, 1.0-coor[0]-coor[1]); 
    vec3 pos = gs_in[0].world_pos * uvb[0] + gs_in[1].world_pos * uvb[1] + gs_in[2].world_pos * uvb[2];
     gl_Position = vp * vec4(pos,1.0);
    gs_out.color = color;
    EmitVertex();
    gl_Position = vp * vec4(pos + dir * MAGNITUDE, 1.0);
    gs_out.color = color;
    EmitVertex();
    EndPrimitive();
}
void GenerateLine(vec2 uv)
{
     uv[0] = min(1.0, max(uv[0], 0.0));
     uv[1] = min(1.0, max(uv[1], 0.0));
     vec3 uvb = vec3(uv, 1.0-uv[0]-uv[1]); 
     vec3 pos = gs_in[0].world_pos * uvb[0] + gs_in[1].world_pos * uvb[1] + gs_in[2].world_pos * uvb[2];
     gl_Position = vp * vec4(pos,1.0);
     gs_out.color = vec3(0.0,1.0,1.0);
     EmitVertex();
     vec3 normal;
     if(has_tbn)
     {
        vec2 uv = gs_in[0].TexCoord * uvb[0] + gs_in[1].TexCoord * uvb[1] + gs_in[2].TexCoord * uvb[2];
        normal = normalize(gs_in[0].tbn * normalize(texture(normal_map, uv).xyz * 2.0 - 1.0));
        normal = normalize((normal_m * vec4(normal, 0.0)).xyz);
     }
     else
        normal = gs_in[0].vs_normal * uvb[0] + gs_in[1].vs_normal * uvb[1] + gs_in[2].vs_normal * uvb[2];
     
     gl_Position = vp * vec4(pos + normal * MAGNITUDE, 1.0);
     gs_out.color = vec3(0.0,1.0,1.0);
     EmitVertex();
     EndPrimitive();
   
     return;
     float type = gs_in[0].light_config.a;
     vec3 dir;
     if(type == POINTLIGHT)
     {
        vec3 world_dir = normalize(gs_in[0].light_config.xyz - pos);
       // if(dot(world_dir, normal)<0.0)
          //  return;
        dir = vec3(vp * vec4(world_dir, 0.0));
     }
     else if(type == DIRECTIONLIGHT)
        dir = normalize(-gs_in[0].light_config.xyz);
     gl_Position = vp * vec4(pos,1.0);
     gs_out.color = vec3(0.0,1.0,0.0);
     EmitVertex();
     gl_Position = vp * vec4(pos + dir * MAGNITUDE, 1.0);
     gs_out.color = vec3(0.0,1.0,0.0);
     EmitVertex();

     EndPrimitive();
}
void main()
{
    /*
    if(!has_tbn)
        return;
    GenerateLine(0, gs_in[0].t,vec3(1.0,0.0,0.0), vec2(0.3,0.0));
    GenerateLine(0, gs_in[0].b,vec3(0.0,1.0,0.0), vec2(0.3,0.0));
    GenerateLine(0, gs_in[0].n,vec3(0.0,0.0,1.0), vec2(0.3,0.0));
    GenerateLine(1, gs_in[0].t,vec3(1.0,0.0,0.0), vec2(0.3,0.0));
    GenerateLine(1, gs_in[0].b,vec3(0.0,1.0,0.0), vec2(0.3,0.0));
    GenerateLine(1, gs_in[0].n,vec3(0.0,0.0,1.0), vec2(0.3,0.0));
    GenerateLine(2, gs_in[0].t,vec3(1.0,0.0,0.0), vec2(0.3,0.0));
    GenerateLine(2, gs_in[0].b,vec3(0.0,1.0,0.0), vec2(0.3,0.0));
    GenerateLine(2, gs_in[0].n,vec3(0.0,0.0,1.0), vec2(0.3,0.0));
    return;
    */
    int num = max(emit_num, 1);
    int edge_num = num;
    float uv_interval = 1.0 / float(edge_num);
    for(int u = 0; u <= edge_num;u++)
    {
        for(int v = 0; v <= edge_num - u;v++)
        {
            GenerateLine(vec2(u *  uv_interval, v * uv_interval));

        }
    }
}