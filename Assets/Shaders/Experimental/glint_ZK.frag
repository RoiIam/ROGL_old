#version 410
// Copyright (c) 2016, NVIDIA CORPORATION.  All rights reserved.
// Created by Tobias Zirr (KIT, NVIDIA) and Anton Kaplanyan (NVIDIA)
// License Creative Commons Attribution 3.0 Unported License.

// For a working self-contained example, find shadertoy at:
// https://www.shadertoy.com/view/ldVGRh

// Example code for the paper "Real-time Rendering of Procedural Multiscale Materials",
// Tobias Zirr (NVIDIA / KIT), Anton Kaplanyan (NVIDIA),
// in ACM SIGGRAPH Symposium on Interactive 3D Graphics and Games, February 2016.
// More info on https://research.nvidia.com/publication/real-time-rendering-procedural-multiscale-materials

//----------------------------------------------------------------------


in vec2 TexCoord;
in vec3 VertexPos;
in vec3 VertexNorm;
in vec3 VertexTang;

uniform struct LightInfo
{
    vec4 Position;// Light position in world coords
    vec3 L;// Intensity
} Light;

uniform struct MaterialInfo
{
    vec2 roughness;
    vec2 microRoughness;
    float searchConeAngle;
    float variation;
    float dynamicRange;
    float density;
    //vec2 roughness, vec2 microRoughness, float searchConeAngle, float variation,
    //float dynamicRange, float density)
} Material;

uniform sampler2D texture_diffuse1;
uniform vec3 CameraPosition;
layout(location = 0) out vec4 FragColor;

// math
float compMax(vec2 v) { return max(v.x, v.y); }
float maxNrm(vec2 v) { return compMax(abs(v)); }
mat2 inverse2(mat2 m) {
    return mat2(m[1][1], -m[0][1], -m[1][0], m[0][0]) / (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
}


float hash( float n ) { return fract(sin(mod(n, 3.14))*753.5453123); }
vec2 hash2( float n ) { return vec2(hash(n), hash(1.1 + n)); }

float erfinv(float x) {
    float w, p;
    w = -log((1.0-x)*(1.0+x));
    if(w < 5.000000) {
        w = w - 2.500000;
        p = 2.81022636e-08;
        p = 3.43273939e-07 + p*w;
        p = -3.5233877e-06 + p*w;
        p = -4.39150654e-06 + p*w;
        p = 0.00021858087 + p*w;
        p = -0.00125372503 + p*w;
        p = -0.00417768164 + p*w;
        p = 0.246640727 + p*w;
        p = 1.50140941 + p*w;
    }
    else {
        w = sqrt(w) - 3.000000;
        p = -0.000200214257;
        p = 0.000100950558 + p*w;
        p = 0.00134934322 + p*w;
        p = -0.00367342844 + p*w;
        p = 0.00573950773 + p*w;
        p = -0.0076224613 + p*w;
        p = 0.00943887047 + p*w;
        p = 1.00167406 + p*w;
        p = 2.83297682 + p*w;
    }
    return p*x;
}

// ray differentials
void calcDpDxy( in vec3 ro, in vec3 rd, in vec3 rdx, in vec3 rdy, in float t, in vec3 nor, 
                out vec3 dpdx, out vec3 dpdy ) {
    dpdx = 2.*t*(rdx*dot(rd,nor)/dot(rdx,nor) - rd) * sign(dot(rd, rdx));
    dpdy = 2.*t*(rdy*dot(rd,nor)/dot(rdy,nor) - rd) * sign(dot(rd, rdy));
}

// some microfacet BSDF geometry factors
// (divided by NoL * NoV b/c cancelled out w/ microfacet BSDF)
float geometryFactor(float NoL, float NoV, vec2 roughness) {
    float a2 = roughness.x * roughness.y;
    NoL = abs(NoL);
    NoV = abs(NoV);

    float G_V = NoV + sqrt((NoV - NoV * a2) * NoV + a2);
    float G_L = NoL + sqrt((NoL - NoL * a2) * NoL + a2);
    return 1. / (G_V * G_L);
}

ivec2 multilevelGridIdx(ivec2 idx) {
    return idx >> findLSB(idx);
}

//----------------------------------------------------------------------

// stable binomial 'random' numbers: interpolate between result for
// two closest binomial distributions where log_{.9}(p_i) integers
float binomial_interp(float u, float N, float p) {
    if(p >= 1.)
        return N;
    else if(p <= 1e-30)
        return 0.;

    // convert to distribution on ints while retaining expected value
    float cN = ceil(N);
    int iN = int(cN);
    p = p * (N / cN);
    N = cN;

    // round p to nearest powers of .9 (more stability)
    float pQ = .9;
    float pQef = log2(p) / log2(pQ);
    float p2 = exp2(floor(pQef) * log2(pQ));
    float p1 = p2 * pQ;
    vec2 ps = vec2(p1, p2);

    // compute the two corresponding binomials in parallel
    vec2 pm = pow(1. - ps, vec2(N));
    vec2 cp = pm;
    vec2 r = vec2(N);

    float i = 0.0;
    // this should actually be < N, no dynamic loops in ShaderToy right now
    for(int ii = 0; ii <= 17; ++ii)
    {
        if(u < cp.x)
            r.x = min(i, r.x);
        if(u < cp.y) {
            r.y = i;
            break;
        }
        // fast path
        if(ii > 16)
        {
            float C = 1. / (1. - pow(p, N - i - 1.));
            vec2 U = (u - cp) / (1. - cp);
            vec2 A = (i + 1. + log2(1. - U / C) / log2(p));
            r = min(A, r);
            break;
        }

        i += 1.;
        pm /= 1. - ps;
        pm *= (N + 1. - i) / i;
        pm *= ps;
        cp += pm;
    }

    // interpolate between the two binomials according to log p (akin to mip interpolation)
    return mix(r.y, r.x, fract(pQef));
}
// resort to gaussian distribution for larger N*p
float approx_binomial(float u, float N, float p) {
    if (p * N > 5.)
    {
        float e = N * p;
        float v = N * p * max(1. - p, 0.0);
        float std = sqrt(v);
        float k = e + erfinv(mix(-.999999, .999999, u)) * std;
        return min(max(k, 0.), N);
    }
    else
        return binomial_interp(u, N, p);
}

//----------------------------------------------------------------------

vec3 glints(vec2 texCO, vec2 duvdx, vec2 duvdy, mat3 ctf
  , vec3 lig, vec3 nor, vec3 view
  , vec2 roughness, vec2 microRoughness, float searchConeAngle, float variation, float dynamicRange, float density)
{
   vec3 col = vec3(0.);

    // Compute pixel footprint in texture space, step size w.r.t. anisotropy of the footprint
    mat2 uvToPx = inverse2(mat2(duvdx, duvdy));
    vec2 uvPP = 1. / vec2(maxNrm(uvToPx[0]), maxNrm(uvToPx[1]));

    // material
    vec2 mesoRoughness = sqrt(max(roughness * roughness - microRoughness * microRoughness, vec2(1.e-12))); // optimizer fail, max 0 removed

    // Anisotropic compression of the grid
    vec2 texAnisotropy = vec2( min(mesoRoughness.x / mesoRoughness.y, 1.)
                             , min(mesoRoughness.y / mesoRoughness.x, 1.) );

    // Compute half vector (w.r.t. dir light)
    vec3 hvW = normalize(lig + view);
    vec3 hv = normalize(hvW * ctf);
    vec2 h = hv.xy / hv.z;
    vec2 h2 = 0.75 * hv.xy / (hv.z + 1.);
    // Anisotropic compression of the slope-domain grid
    h2 *= texAnisotropy;

    // Compute the Gaussian probability of encountering a glint within a given finite cone
    vec2 hppRScaled = h / roughness;
    float pmf = (microRoughness.x * microRoughness.y) / (roughness.x * roughness.y)
        * exp(-dot(hppRScaled, hppRScaled)); // planeplane h
    pmf /= hv.z * hv.z * hv.z * hv.z; // projected h
//  pmf /= dot(lig, nor) * dot(view, nor); // projected area, cancelled out by parts of G, ...
    float pmfToBRDF = 1. / (3.14159 * microRoughness.x * microRoughness.y);
    pmfToBRDF /= 4.; // solid angle o
    pmfToBRDF *= geometryFactor(dot(lig, nor), dot(view, nor), roughness); // ... see "geometryFactor"
    // phenomenological: larger cones flatten distribution
    float searchAreaProj = searchConeAngle * searchConeAngle / (4. * dot(lig, hvW) * hv.z); // * PI
    pmf = mix(pmf, 1., clamp(searchAreaProj, 0.0, 1.0)); // searchAreaProj / PI
    pmf = min(pmf, 1.);
    
    // noise coordinate (decorrelate interleaved grid)
    texCO += vec2(100.);
    // apply anisotropy _after_ footprint estimation
    texCO *= texAnisotropy;

    // Compute AABB of pixel in texture space
    vec2 uvAACB = max(abs(duvdx), abs(duvdy)) * texAnisotropy; // border center box
    vec2 uvb = texCO - 0.5 * uvAACB;
    vec2 uve = texCO + 0.5 * uvAACB;

    vec2 uvLongAxis = uvAACB.x > uvAACB.y ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 uvShortAxis = 1.0 - uvLongAxis;

    // Compute skew correction to snap axis-aligned line sampling back to longer anisotropic pixel axis in texture space
    vec2 skewCorr2 = -(uvToPx * uvLongAxis) / (uvToPx * uvShortAxis);
    float skewCorr = abs((uvToPx * uvShortAxis).x) > abs((uvToPx * uvShortAxis).y) ? skewCorr2.x : skewCorr2.y;
    skewCorr *= dot(texAnisotropy, uvShortAxis) / dot(texAnisotropy, uvLongAxis);

    float isoUVPP = dot(uvPP, uvShortAxis);
    // limit anisotropy
    isoUVPP = max(isoUVPP, dot(uvAACB, uvLongAxis) / 16.0);

     // Two virtual grid mips: current and next
    float fracMip = log2(isoUVPP);
    float lowerMip = floor(fracMip);
    float uvPerLowerC = exp2(lowerMip);

    // Current mip level and cell size
    float uvPC = uvPerLowerC;
    float mip = lowerMip;

    int iter = 0;
    int iterThreshold = 150;

    for (int i = 0; i < 2; ++i)
    {
        float mipWeight = 1.0 - abs(mip - fracMip);

        vec2 uvbg = min(uvb + 0.5 * uvPC, texCO);
        vec2 uveg = max(uve - 0.5 * uvPC, texCO);

        // Snapped uvs of the cell centers
        vec2 uvbi = floor(uvbg / uvPC);
        vec2 uvbs = uvbi * uvPC;
        vec2 uveo = uveg + uvPC - uvbs;

        // Resulting compositing values for a current layer
        float weight = 0.0;
        vec3 reflection = vec3(0.0);

        // March along the long axis
        vec2 uvo = vec2(0.0), uv = uvbs, uvio = vec2(0.0), uvi = uvbi;
        for (int iter1 = 0; iter1 < 18; ++iter1) // horrible WebGL-compatible static for loop
        {
            // for cond:
            if (dot(uvo, uvLongAxis) < dot(uveo, uvLongAxis) && iter < iterThreshold);
            else break;

            // Snap samples to long anisotropic pixel axis
            float uvShortCenter = dot(texCO, uvShortAxis) + skewCorr * dot(uv - texCO, uvLongAxis);

            // Snapped uvs of the cell center
            uvi += (floor(uvShortCenter / uvPC) - dot(uvi, uvShortAxis)) * uvShortAxis;
            uv = uvi * uvPC;
            float uvShortEnd = uvShortCenter + uvPC;

            vec2 uvb2 = uvbg * uvLongAxis + uvShortCenter * uvShortAxis;
            vec2 uve2 = uveg * uvLongAxis + uvShortCenter * uvShortAxis;

            // March along the shorter axis
            for (int iter2 = 0; iter2 < 4; ++iter2) // horrible WebGL-compatible static for loop
            {
                // for cond:
                if (dot(uv, uvShortAxis) < uvShortEnd && iter < iterThreshold);
                else break;

                // Compute interleaved cell index
                ivec2 cellIdx = ivec2(uvi + vec2(.5));
                cellIdx = multilevelGridIdx(cellIdx);

                // Randomize a glint based on a texture-space id of current grid cell
                vec2 u2 = hash2(float( (cellIdx.x + 1549 * cellIdx.y) ));
                // Compute index of the cone
                vec2 hg = h2 / (microRoughness + searchConeAngle);
                vec2 hs = floor(hg + u2) + u2 * 533.;    // discrete cone index in paraboloid hv grid
                ivec2 coneIdx = ivec2(hs);

                // Randomize glint sizes within this layer
                float var_u = hash(float( (cellIdx.x + cellIdx.y * 763 + coneIdx.x + coneIdx.y * 577) ));
                float mls = 1. + variation * erfinv(mix(-.999, .999, var_u));
                if (mls <= 0.0) mls = fract(mls) / (1. - mls);
                mls = max(mls, 1.e-12);

                // Bilinear interpolation using coverage made by areas of two rects
                vec2 mino = max(1.0 - max((uvb2 - uv) / uvPC, 0.0), 0.0);
                vec2 maxo = max(1.0 - max((uv - uve2) / uvPC, 0.0), 0.0);
                vec2 multo = mino * maxo;
                float coverageWeight = multo.x * multo.y;

                float cellArea = uvPC * uvPC;
                // Expected number of glints 
                float eN = density * cellArea;
                float sN = max(eN * mls, min(1.0, eN));
                eN = eN * mls;

                // Sample actually found number of glints
                float u = hash(float(coneIdx.x + coneIdx.y * 697));
                float lN = approx_binomial(u, sN, pmf);
#if 0
                // Colored glints
                if (false) {
                    vec3 glintColor = hue_colormap(fract(u + u2.y));
                    glintColor = mix(vec3(1.0f), glintColor, 1. / sqrt(max(lN, 1.0)));
                }
#endif
                // Ratio of glinting vs. expected number of microfacets
                float ratio = lN / eN;
                
                // limit dynamic range (snow more or less unlimited)
                ratio = min(ratio, dynamicRange * pmf);
                
                // convert to reflectance
                ratio *= pmfToBRDF;
#if 0
                // Grid
                reflection += vec3(u);
                weight += coverageWeight;
#else
                // Accumulate results
                reflection += coverageWeight * ratio;
                weight += coverageWeight;
#endif

                // for incr:
                uv += uvPC * uvShortAxis, uvi += uvShortAxis, ++iter;
            }

            // for incr:
              uvo += uvPC * uvLongAxis, uv = uvbs + uvo
            , uvio += uvLongAxis, uvi = uvbi + uvio;
        }

#ifdef DEBUG
        // Normalization
        if (weight < 1.e-15) {
            col = vec3(0.0, 1.0, 1.0);
            break;
        }
#endif

        reflection = reflection / weight;

        // Compositing of two layers
        col += mipWeight * reflection;

        // for incr:
        uvPC *= 2., mip += 1.;
    }

    return col;
}

vec3 CalcSpotLightGlint(vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(Light.Position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), Mat.shininess);
    // attenuation
    //float distance = length(Light.Position.xyz - fragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    //float theta = dot(lightDir, normalize(-light.direction));
    //float epsilon = light.cutOff - light.outerCutOff;
    //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = vec3(0.6) * vec3(texture(texture_diffuse1, TexCoord));
    vec3 diffuse = vec3(0.6) * diff * vec3(texture(texture_diffuse1, TexCoord));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    //vec3 specular = light.specular * (spec * material.specular);
    //ambient *= 0.8 * intensity;
    //diffuse *= 5 * intensity;
    //specular *= attenuation * intensity;


    return (ambient + diffuse);
}

void main()
{
    //vec2 texCO, vec2 duvdx, vec2 duvdy, mat3 ctf
    //  , vec3 lig, vec3 nor, vec3 view
    //  , vec2 roughness, vec2 microRoughness, float searchConeAngle,
    //float variation, float dynamicRange, float density)
    vec2 dst0 = dFdx(TexCoord);
    vec2 dst1 = dFdy(TexCoord);
    //VertexPos je view?
    vec3 view = normalize(CameraPosition - VertexPos);
    // Matrix for transformation to tangent space

    vec3 binormal = cross(VertexNorm, VertexTang);
    mat3 toLocal = mat3(
    VertexTang.x, binormal.x, VertexNorm.x,
    VertexTang.y, binormal.y, VertexNorm.y,
    VertexTang.z, binormal.z, VertexNorm.z);
    mat3 ctf = mat3(VertexTang,binormal,VertexNorm);

    vec3 FragColor1 = glints(TexCoord,dst0,dst1,ctf,Light.Position.xyz,VertexNorm,view,
    Material.roughness, Material.microRoughness, Material.searchConeAngle,
    Material.variation,Material.dynamicRange,Material.density);



    //vyuzijeme poznatok od chermaina cez ctf ziskame texturu
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    //return vec3(0.8, 0., 0.) * m_i_pi * wi.z; //just red


    float distanceSquared = distance(VertexPos, Light.Position.xyz);
    distanceSquared *= distanceSquared;
    vec3 Li = Light.L / distanceSquared;
    vec3 wi = toLocal * normalize(Light.Position.xyz - VertexPos );
    wi = normalize(wi);
    FragColor1 += (texColor.xyz * 0.318309 * wi.z)*Li;

    FragColor = vec4(FragColor1.xyz,1.0);
    FragColor = vec4(FragColor1.xyz,1.0);
}

