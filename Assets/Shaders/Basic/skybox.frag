#version 440
out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    //FragColor = vec4(1.0f,0,0,1.0f);
    FragColor = texture(skybox, TexCoords);
}