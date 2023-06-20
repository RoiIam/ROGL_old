#version 440
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 cameraPos;
uniform vec3 lightPos;


uniform float tiling = 10; //should be scaled according to the water tile size, this imapcts size of ripples


out vec2 TexCoords;
out vec4 clipSpace;
out vec3 toCamVector;
out vec3 fromLightVector;


void main()
{

    vec4 worldPos = model * vec4(aPos, 1.0f);

    clipSpace = projection * view * worldPos;
    TexCoords = aTexCoords * tiling;
    gl_Position = clipSpace;
    toCamVector =  cameraPos - worldPos.xyz;
    fromLightVector =  worldPos.xyz -lightPos;

}