#version 330 core
layout(location = 0) out vec4 FragColor;
struct VS_OUT{
    vec3 vs_normal;
    vec2 TexCoord;
    vec4 lightspace_pos;
    vec3 world_pos;
    vec3 view_point;
    vec4 light_config;
    mat3 tbn;
};
layout(location = 0) in VS_OUT fs_in;

#define POINTLIGHT 1.f
#define DIRECTIONLIGHT 0.f
#define NOSHADOW 0
#define NORMALSHADOW 1
#define PCFSHADOW 2
#define PCSSSHADOW 3

//light
uniform vec4 intensity;

//shadow
uniform float hpixellenth;
uniform float biascontrol;
uniform float biasoffset;
uniform int ShadowType;
uniform int core;

//condition inf
uniform bool hasm_texture;
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

        float light_width = intensity.a;
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

void main() {
    FragColor = vec4(vec3(0.0,0.0,0.0), 1.0);
    //return;
    Material material = AnalyseMaterial();
    vec3 diffuse = hasm_texture ? vec3(texture(diffuse_texture, fs_in.TexCoord)) : material.materialDiffuse;
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

    vec3 view = normalize(fs_in.view_point - fs_in.world_pos);
    vec3 reflectDir = -reflect(view, normal);
    float type = fs_in.light_config.a;
    vec3 light_intensity = intensity.rgb;
     //ambient
    vec3 light_ambient = ambient * texture(skybox_texture, reflectDir).rgb;
    FragColor += vec4(light_ambient, 0.0);
    if(type == DIRECTIONLIGHT) 
    {
        light_dir = -fs_in.light_config.xyz;
        float shadowFact = ShadowCalculation(fs_in.lightspace_pos, light_dir, normal);
        if(shadowFact == 0.0)
        {
            return;
        }
        light_intensity  = shadowFact * light_intensity;
    } 
    else if(type == POINTLIGHT) 
    {
        vec3 light_pos = fs_in.light_config.rgb;
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
    else 
    {
        return;
    }
   
    //diffuse
    vec3 light_diffuse = light_intensity * (max(0.0, dot(light_dir, normal))) * diffuse;
    FragColor += vec4(light_diffuse, 0.0);
    //specular
    vec3 half_angle = normalize(view + light_dir);
    vec3 light_specular = light_intensity * pow(max(dot(half_angle, normal), 0.0), 32) * specular;
    FragColor += vec4(light_specular, 0.0);
}
