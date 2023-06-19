#version 440
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//out vec2 TexCoords;
out vec4 clipSpace;
void main()
{
    clipSpace = projection * view * model * vec4(aPos, 1.0f);
    //TexCoords = aTexCoords;
    gl_Position = clipSpace;
}