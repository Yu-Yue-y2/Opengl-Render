#version 330 core
out vec4 FragColor;
//in vec3 offset;
void main()
{             
   gl_FragDepth = gl_FragCoord.z;
   //FragColor = vec4(vec3(3.0 * length(offset)),1.0);
  // FragColor = vec4(1.0,0.0,0.,1.0);
}