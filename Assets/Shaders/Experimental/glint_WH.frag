#version 410


//Sparkly but not too sparkly!
//Huw Bowles Beibei Wang
uniform sampler3D Noise;//the 3D noise texture
uniform float glitterStrength;//the brighness of the sparkle
uniform vec4 lightDir; //light direction
uniform vec4 color; //color of the surface

in vec3 vNormal;
in vec3 vViewVec;
in vec3 vObjPos;
in vec4 vlarge_dir; //potrebne pre aniso

uniform bool with_anisotropy;//with the anisotropy or not
uniform float i_sparkle_size; //sparkle size
uniform float i_sparkle_density;//sparkle density
uniform float i_noise_density;//the noise density

uniform float i_noise_amount;
uniform float i_view_amount;
uniform float i_time_factor;
uniform float time_0_X;

layout(location = 0) out vec4 FragColor;

void main(void)
{

   // flip the z for OpenGL.
   vec3 ldir = normalize(-vec3(lightDir.x, lightDir.y, lightDir.z));   //CO AK TOTO ROBI PROBLEMY PRI DELIOTOVI? V OPENGL?
   vec3 n_view_dir = normalize(vViewVec);
   
   // Basic lighting
   //float diffuse = saturate(dot(ldir, -vNormal)) + 0.8;

   //Ak clampneme -1 tak nevidno trbiletky kde svetlo nesvieti
   float diffuse = clamp(dot(ldir, -vNormal),-1.0,1.0) + 0.8;

   //float specBase = saturate(dot(reflect(- n_view_dir, vNormal), ldir));
   float specBase = clamp(dot(reflect(- n_view_dir, -vNormal),ldir),-1.0,1.0);

   float specular = pow(specBase, 24.0);
   float diffuse_noise = texture(Noise, vObjPos * 0.04).x;
   //float diffuse_noise = 0.7f;//texture(Noise, vObjPos * 0.04).x; //no need to use texture3D()
   vec4 base = (0.5 * diffuse_noise + 0.5) * color;

   float noise_dense = i_noise_density;
   float grid_sparkle_dense = i_sparkle_density * 15.0;
   float adjust_sparkle_size = 1.0 - 0.2 * i_sparkle_size * i_sparkle_density * i_sparkle_density; 
          
   //Expanding the distance range, Logarithm Distribution 
   float zBuf = length(vViewVec);    
   float z_exp = log2(0.3 * zBuf + 3.0) / 0.37851162325;   
   float floorlog = floor(z_exp);
   float level_zBuf = 0.1 * pow(1.3,floorlog) - 0.2; 
   float level = 0.12 / level_zBuf;              
   grid_sparkle_dense *= level;
   noise_dense *= level;

   //warpping the view vector
   vec3 w_view_dir = n_view_dir * level_zBuf;
   w_view_dir = sign(w_view_dir) * fract(abs(w_view_dir));
          
   //consider the view direction and the time
   float time = 0.2 * abs(fract(time_0_X * i_time_factor) - 0.5);
   vec3 pos_with_view = grid_sparkle_dense * vObjPos + time + i_view_amount * normalize(w_view_dir);
     
   //generate the grid
   vec3 grid_index = floor(pos_with_view);
   vec3 grid_offset = pos_with_view - grid_index;
   float grid_length = 1.0 / grid_sparkle_dense;
   vec3 grid_center = grid_index * grid_length;         
         
   //jitter the grid center      
   float jitter_noisy = 0;//i_noise_amount * texture(Noise, noise_dense * grid_center).x;           // same texture3D
   vec3 jitter_grid = vec3(jitter_noisy);
   jitter_grid = 0.5 * fract(jitter_grid + 0.5) - vec3(0.75,0.75,0.75);
   vec3 new_offset = grid_offset + jitter_grid;  

   //Anisotropy    
   //float dotvn = vlarge_dir.w; // float dotvn = dot ( v , n ) ;

   // ma je  large_dir
   //vec3 large_dir = v - dotvn *n ; // v je view/z; n je normal
   float dotvn = dot ( vViewVec , -vNormal ) ;
   vec3 large_dir = n_view_dir-(dotvn *-vNormal);
   //vec3 ma = v - dotvn *n ;  // Ellipse major axis
   //vec3 P_x_proj = dot ( P_x , ma)*ma ;
   // P_x += ( abs ( dotvn ) -1.0)* P_x_proj / dot (ma , ma ) ;

   //vec3 large_dir = vlarge_dir.xyz; not yet used
   if(with_anisotropy)   
      new_offset += (abs(dotvn) - 1.0) * dot(new_offset,large_dir) * large_dir / dot(large_dir,large_dir);

   //compute the brightness of the sparkle according to the offset
   float l2 = dot(new_offset, new_offset);
   float m_ss = 1.0 - adjust_sparkle_size;
   float glittering = 0.0;
   if(m_ss > l2)
   {
      float lend = ((1.0 - l2) - adjust_sparkle_size) / (1.0 - adjust_sparkle_size);
      glittering =  20.0 * lend;
   }
   
   FragColor = base * diffuse + 0.5 * specular + glitterStrength * glittering;
   
}
