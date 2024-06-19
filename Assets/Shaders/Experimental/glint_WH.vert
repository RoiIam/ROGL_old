#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec3 VertexTangent;

out vec2 TexCoord;
out vec3 VertexPos;
//out vec3 VertexNorm;
out vec3 VertexTang;

out vec3 vNormal;
out vec3 vViewVec;
out vec3 vObjPos;
out vec4 vlarge_dir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 CameraPosition;

void main() {

   // Transform normal and tangent to world space
   vec3 norm = normalize((model * vec4(VertexNormal, 0.)).xyz);
   //VertexNorm = norm;
   vNormal = norm;

   vec3 tang = normalize((model * vec4(VertexTangent, 0.)).xyz);
   VertexTang = tang;

   TexCoord = VertexTexCoord;

   VertexPos = (model * vec4(VertexPosition, 1.)).xyz;

   gl_Position =  projection * view * model *  vec4(VertexPosition, 1.0);

   vViewVec = gl_Position.xyz - CameraPosition; // vo world space
   vObjPos = VertexPos.xyz; //ci VertexPos?
   float dotvn =dot ( vViewVec , -vNormal ) ;
   vlarge_dir = vec4( vViewVec - dotvn * -vNormal , dotvn);
}
