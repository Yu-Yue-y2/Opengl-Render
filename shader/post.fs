#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;
#define HDR_NONE 0
#define HDR_REINHARD 1
#define HDR_EXP 2
#define HDR_FILMIC 3
#define HDR_ACES 4
uniform sampler2D fbo_color;//screenTexture
uniform float exposure;
uniform int hdr_type;
void main()
{ 
    vec3 hdrColor = texture(fbo_color, TexCoords).rgb;
    vec3 ldrColor;
    if(hdr_type == HDR_NONE)
    {
        ldrColor = hdrColor;
    }
    else if(hdr_type == HDR_REINHARD)
    {
         ldrColor = hdrColor / (hdrColor + vec3(1.0));
    }
    else if(hdr_type == HDR_EXP)
    {
        ldrColor = vec3(1.0) - exp(-hdrColor * exposure);
    }
    //const float gamma = 2.2;
    //ldrColor = pow(ldrColor, vec3(1.0 / gamma));
    FragColor = vec4(ldrColor, 1.0);
}