#version 440
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoords;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    //gl_Position = projection * view * model * vec4(aPos, 1.0f);
    gl_Position = projection * view * model *  vec4(aPos+ aNormal*0.009f, 1.0f); // commentes , extrude along normals not just scale
}