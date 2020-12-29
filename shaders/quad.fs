#version 330 core
in vec2 TexCoord;
out vec3 FragColor;

uniform sampler2D screen;


void main()
{
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    FragColor = vec3(texture(screen, TexCoord).r);
}