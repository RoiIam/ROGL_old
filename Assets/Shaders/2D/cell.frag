#version 440
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec4 col = vec4(1, 0.6, 0, 1.0);
uniform int type;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
    discard;
	//if(type == 0)
	//texcolor no change
	if(type == 1)
	texColor+=vec4(1, 0, 0, 1.0);
	if(type == 2)
	texColor+=vec4(0, 1, 0, 1.0);
	if(type == 3)
	texColor+=vec4(0, 0, 1, 1.0);
	

    FragColor = texColor;
}