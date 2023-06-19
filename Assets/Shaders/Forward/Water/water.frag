#version  440
out vec4 FragColor;

uniform vec4 col = vec4(1, 0.0, 0, 1.0);
uniform float mixVal = 0.5;//in vec2 TexCoords;
in vec4 clipSpace;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

void main()
{

    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 +0.5;
    vec2 reflectionTexCoords = vec2(ndc.x, 1.0-ndc.y);
    vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoords);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoords);//vec4 reflectionColor = texture(reflectionTexture,TexCoords);//vec4 refractionColor = texture(refractionTexture,TexCoords);//FragColor = col;// mix(reflectionColor,refractionColor,0.5);
    FragColor = mix(reflectionColor, refractionColor, mixVal);///FragColor = reflectionColor,refractionColor;
}