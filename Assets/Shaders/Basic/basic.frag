#version  440
out vec4 FragColor;

uniform vec4 col = vec4(1, 0.6, 0, 1.0);

void main()
{
    FragColor = col;
}