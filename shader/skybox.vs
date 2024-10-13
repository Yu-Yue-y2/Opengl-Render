#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 vp;
out vec3 Textcoor;
void main()
{
    Textcoor = position;
    vec4 pos = vp * vec4(position, 1.0);
    gl_Position = pos.xyww;
}