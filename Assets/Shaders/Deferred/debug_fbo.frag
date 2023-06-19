// fragment shader
#version 330 core
out vec4 FragColor;
in  vec2 TexCoords;

uniform sampler2D fboAttachment; //no need to bind its auto set...

void main()
{
    //FragColor = vec4(1,1,1,1);
    FragColor = texture(fboAttachment, TexCoords).rrrr;// change this so we get nice ssao debug
} 