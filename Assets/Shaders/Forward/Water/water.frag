#version  440
out vec4 FragColor;

uniform vec4 col = vec4(1, 0.0, 0, 1.0);
uniform float fresnelStrength = 0.5;
uniform float distortionStrength = 0.02;
uniform vec2 moveFactor = vec2(0,0);

in vec2 TexCoords;
in vec4 clipSpace;
in vec3 toCamVector;


uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvTexture;

void main()
{

    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 +0.5;
    vec2 reflectionTexCoords = vec2(ndc.x, 1.0-ndc.y);
    vec2 refractionTexCoords = vec2(ndc.x, ndc.y);

    vec2 distortion1 = texture(dudvTexture, vec2 (TexCoords.x+moveFactor.x,TexCoords.y+moveFactor.y)).rg * 2.0 - 1.0;

    //or use
    //vec2 distortion1 = texture(dudvTexture, vec2 (TexCoords.x+moveFactor.x,TexCoords.y)).rg * 2.0 - 1.0;
    //vec2 distortion2 = texture(dudvTexture, vec2 (-TexCoords.x+moveFactor.x,TexCoords.y+moveFactor.x)).rg * 2.0 - 1.0;
    //vec2 finalDistortion =distortion1+distortion2;
    //reflectionTexCoords +=finalDistortion;
    //refractionTexCoords +=finalDistortion;


    distortion1 *=distortionStrength;
    reflectionTexCoords +=distortion1;
    reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
    reflectionTexCoords.y = clamp(reflectionTexCoords.y, 0.001, 0.999);


    refractionTexCoords +=distortion1;
    refractionTexCoords = clamp(refractionTexCoords,0.001,0.999);



    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoords);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoords);//vec4 reflectionColor = texture(reflectionTexture,TexCoords);//vec4 refractionColor = texture(refractionTexture,TexCoords);//FragColor = col;// mix(reflectionColor,refractionColor,0.5);

 vec3 viewVector = normalize(toCamVector);
    vec3 waterNormal = vec3(0,1,0);
float fresnelFactor = dot(viewVector, waterNormal);
    fresnelFactor=pow(fresnelFactor, fresnelStrength);


    //vec4 finalColor = mix(reflectionColor, refractionColor, fresnelStrength);
    vec4 finalColor = mix(reflectionColor, refractionColor, fresnelFactor);

    FragColor= mix(finalColor, vec4(0,0.3,0.5,1.0),0.2);

}