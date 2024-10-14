#version 330 core
#define FLT_MAX 1e20
layout(location = 0) out vec4 FragColor;
struct VS_OUT{
    vec3 vs_normal;
    vec2 TexCoord;
    vec4 lightspace_pos;
    vec3 world_pos;
    mat3 tbn;
};
layout(location = 0) in VS_OUT fs_in;

#define POINTLIGHT 1.f
#define DIRECTIONLIGHT 0.f
#define NOSHADOW 0
#define NORMALSHADOW 1
#define PCFSHADOW 2
#define PCSSSHADOW 3
uniform vec3 view_point;
//shadow
uniform float hpixellenth;
uniform float biascontrol;
uniform float biasoffset;
uniform int ShadowType;
uniform int core;
//light
#define LIGHT_COMPUTE_SHADOW 0.0f
struct Light {
    vec4 intensity; 
    vec4 v1;      
};
layout (std140) uniform RawLights {
    Light lights[]; 
};
uniform int light_num;

#define GetLightPos GetLightDir
vec3 GetLightDir(int id)
{
    return lights[id].v1;
}

//condition inf
uniform bool has_tbn;
layout (std140) uniform Matrices
{
    mat4 m;
    mat4 mvp;
    mat4 normal_m;
    mat4 lightspace_mvp;
};
layout (std140) uniform RawMaterial
{
    vec4 rawMaterialData1;
    vec4 rawMaterialData2;
    vec4 rawMaterialData3;
};
#define BASEMATERIAL 0
struct Material {
    vec3 materialDiffuse;
    vec3 materialSpecular;
    vec3 materialAmbient;
};
//skybox
#define CUBEMAP_SKYBOX 0
uniform bool correct_reflect_env;
uniform int skybox_type;
uniform float skybox_size;
//sampler
uniform sampler2D diffuse_texture;//diffuse
uniform sampler2D shadowmap;//shadow
uniform sampler2D normal_map;
uniform samplerCube skybox_texture;

Material AnalyseMaterial()
{
    Material material;
    int materialType = int(rawMaterialData1.w);
    if(materialType == BASEMATERIAL)
    {
        material.materialDiffuse = rawMaterialData1.xyz;
        material.materialSpecular = rawMaterialData2.xyz;
        material.materialAmbient = rawMaterialData3.xyz;
    }
    return material;
}
float getBias(vec3 lightdir, vec3 facenormal) {
    float _cos = max(0.001, dot(lightdir, facenormal));
    float _tan = (1.0 - _cos);
    float bias = hpixellenth * _tan;
    return bias * (biascontrol) + (biasoffset);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightdir, vec3 normal) {
    if(ShadowType == NOSHADOW) {
        return 1.0;
    }
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(projCoords.z > 1.0)
        return 0.0;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    float bias = getBias(lightdir, normal);
    if(ShadowType == NORMALSHADOW) {
        float closestDepth = texture(shadowmap, projCoords.xy).r;
        shadow = currentDepth > closestDepth + bias ? 0.0 : 1.0;
    } else if(ShadowType == PCFSHADOW) {
        vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
        for(int x = -core; x <= core; ++x) {
            for(int y = -core; y <= core; ++y) {
                float pcfDepth = texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth > pcfDepth + (1.0 + sqrt(x * x + y * y) * 0.333) * bias ? 0.0 : 1.0;
            }
        }
        shadow /= pow(2 * core + 1, 2);
    } else if(ShadowType == PCSSSHADOW) {
        //d_block:
        float d_block = 0.0;
        vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
        for(int x = -core; x <= core; ++x) {
            for(int y = -core; y <= core; ++y) {
                d_block += texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
            }
        }
        d_block /= pow(2 * core + 1, 2);

        float light_width = 10.0;//intensity.a;
        int core_lenth = int(light_width * d_block / (texture(shadowmap, projCoords.xy).r - d_block));
        int hl = max(min((core_lenth - 1) / 2, 4), 0);
        for(int x = -hl; x <= hl; ++x) {
            for(int y = -hl; y <= hl; ++y) {
                float pcfDepth = texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth > pcfDepth + (1.0 + sqrt(x * x + y * y) / 3.0) * bias ? 0.0 : 1.0;
            }
        }
        shadow /= pow(2 * hl + 1, 2);
    } else {

    }
    return shadow;
}

vec3 GetNormalInMap()
{
    vec3 normal = texture(normal_map, fs_in.TexCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    return normalize(vec3(normal_m * vec4(fs_in.tbn * normal,0.0)));
}
vec3 CorrectReflectEnv(vec3 dir)
{
    vec3 reflect_dir = dir;
    if(correct_reflect_env)
    {
        if(skybox_type == CUBEMAP_SKYBOX)
        {
            vec3 box_max = vec3(skybox_size) + view_point;
            vec3 box_min = -box_max;
            float t = FLT_MAX;
            for(int i = 0;i < 3; i++)
            {
                if(dir[i] == 0.0)
                    continue;
                t = min(t, (dir[i] > 0.0 ? (box_max[i] - fs_in.world_pos[i]) : (box_min[i] - fs_in.world_pos[i])) / dir[i]);
            }
            reflect_dir = normalize(fs_in.world_pos + t * dir - view_point);
        }
    }
    return reflect_dir;
}
void main() {
    FragColor = vec4(vec3(0.0,0.0,0.0), 1.0);
    Material material = AnalyseMaterial();
    vec3 diffuse = vec3(texture(diffuse_texture, fs_in.TexCoord)) * material.materialDiffuse;
    vec3 specular = material.materialSpecular;
    vec3 ambient = material.materialAmbient;
    vec3 normal, light_dir;
    if(has_tbn)
    {
        normal = GetNormalInMap();
    }
    else
    {
        normal = normalize(fs_in.vs_normal);
    }
    vec3 view = normalize(view_point - fs_in.world_pos);
    vec3 reflectDir = -reflect(view, normal);
    vec3 light_ambient = ambient * texture(skybox_texture, CorrectReflectEnv(reflectDir)).rgb;
    FragColor += vec4(light_ambient, 0.0);
    for(int i = 0; i < light_num; i++)
    {
        float type = lights[i].v1.w;
        vec3 light_intensity = vec3(lights[i].intensity);
        if(type == DIRECTIONLIGHT) 
        {
            light_dir = -vec3(lights[i].v1);
            float shadowFact = ShadowCalculation(fs_in.lightspace_pos, light_dir, normal);
            if(shadowFact == 0.0)
            {
                return;
            }
            light_intensity  = shadowFact * light_intensity;
        } 
        else if(type == POINTLIGHT) 
        {
            vec3 light_pos = vec3(lights[i].v1);
            light_dir = normalize(light_pos - fs_in.world_pos);
            float d = sqrt(length(light_pos - fs_in.world_pos));
            if(d < 1.5)
            {
                FragColor = vec4(1.0, 1.0, 1.0 ,1.0);
                return;
            }
            float atten = 1.0 + 0.7 * d + 0.052 * d * d;
            light_intensity /= atten;
        } 
        //diffuse
        vec3 light_diffuse = light_intensity * (max(0.0, dot(light_dir, normal))) * diffuse;
        FragColor += vec4(light_diffuse, 0.0);
        //specular
        vec3 half_angle = normalize(view + light_dir);
        vec3 light_specular = light_intensity * pow(max(dot(half_angle, normal), 0.0), 32) * specular;
        FragColor += vec4(light_specular, 0.0);
    }
}