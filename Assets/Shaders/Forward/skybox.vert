#version 440
layout (location = 0) in vec3 aPos; // position has attribute position 0
out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = vec3(-aPos.x, aPos.y, aPos.z); // Flip X so that words make sense again //thnks comments
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
