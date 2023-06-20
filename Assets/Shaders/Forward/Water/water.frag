#version  440
out vec4 FragColor;

uniform vec4 col = vec4(1, 0.0, 0, 1.0);
uniform float fresnelStrength = 0.5;
uniform float distortionStrength = 0.02;
uniform vec2 moveFactor = vec2(0,0);
uniform vec3 lightColor;



in vec2 TexCoords;
in vec4 clipSpace;
in vec3 toCamVector;
in vec3 fromLightVector;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvTexture;
uniform sampler2D normalMapTexture;
uniform sampler2D depthMap;


uniform float zNear;
uniform float zFar;
uniform float alphaDepth = 5.0;


float shineDamper = 20;
float reflectivity = 0.5;

void main()
{

    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 +0.5;
    vec2 reflectionTexCoords = vec2(ndc.x, 1.0-ndc.y);
    vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
    //get depth
    float depth = texture(depthMap,refractionTexCoords).r;
    float floorDistance = 2.0 * zNear*zFar/ (zFar+zNear - (2.0*depth-1.0) * (zFar-zNear) );
    float depth2 = gl_FragCoord.z;
    float waterDistance = 2.0 * zNear*zFar/ (zFar+zNear - (2.0*depth2-1.0) * (zFar-zNear) );
    float waterDepth = floorDistance-waterDistance;

    //vec2 distortion1 = texture(dudvTexture, vec2 (TexCoords.x+moveFactor.x,TexCoords.y+moveFactor.y)).rg * 2.0 - 1.0;

    //or use
    //vec2 distortion1 = texture(dudvTexture, vec2 (TexCoords.x+moveFactor.x,TexCoords.y)).rg * 2.0 - 1.0;
    //vec2 distortion2 = texture(dudvTexture, vec2 (-TexCoords.x+moveFactor.x,TexCoords.y+moveFactor.x)).rg * 2.0 - 1.0;
    //vec2 finalDistortion =distortion1+distortion2;
    //reflectionTexCoords +=finalDistortion;
    //refractionTexCoords +=finalDistortion;


    //new distortion calc
    vec2 distortedTexCoords = texture(dudvTexture, vec2(TexCoords.x + moveFactor.x, TexCoords.y)).rg*0.1;
    distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor.y);
    vec2 totalDistortion = (texture(dudvTexture, distortedTexCoords).rg * 2.0 - 1.0) * distortionStrength;

    totalDistortion *clamp(waterDepth/alphaDepth*5 ,0.0, 1.0); //closer to tje edge, the lesser the distortion

    //distortion1 *=distortionStrength;

    //reflectionTexCoords +=distortion1;
    reflectionTexCoords +=totalDistortion;
    reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
    reflectionTexCoords.y = clamp(reflectionTexCoords.y, 0.001, 0.999);


    //reflectionTexCoords +=distortion1;
    refractionTexCoords +=totalDistortion;
    refractionTexCoords = clamp(refractionTexCoords,0.001,0.999);



    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoords);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoords);//vec4 reflectionColor = texture(reflectionTexture,TexCoords);//vec4 refractionColor = texture(refractionTexture,TexCoords);//FragColor = col;// mix(reflectionColor,refractionColor,0.5);

    //get normal
    vec4 normalMapColor = texture(normalMapTexture, distortedTexCoords);
    vec3 normal =vec3(normalMapColor.r*2.0-1.0,normalMapColor.b*1.8,normalMapColor.g*2.0-1.0);
    normal = normalize(normal);

    vec3 viewVector = normalize(toCamVector);
    float fresnelFactor = dot(viewVector, normal);
    fresnelFactor=pow(fresnelFactor, fresnelStrength);
    fresnelFactor = clamp(fresnelFactor, 0.0, 1.0);


    //calc spec lightning
    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlights = lightColor * specular * reflectivity;

    //also dampen the highlights near edges
    specularHighlights *= clamp(waterDepth/alphaDepth ,0.0, 1.0);


    //vec4 finalColor = mix(reflectionColor, refractionColor, fresnelStrength);
    vec4 finalColor = mix(reflectionColor, refractionColor, fresnelFactor);
    finalColor += vec4(specularHighlights,0.0);
    finalColor= mix(finalColor, vec4(0,0.3,0.5,1.0),0.2);
    finalColor.a = clamp(waterDepth/alphaDepth ,0.0, 1.0);

    //finalColor= vec4(waterDepth/50); //test of depth map

    FragColor= finalColor;

}