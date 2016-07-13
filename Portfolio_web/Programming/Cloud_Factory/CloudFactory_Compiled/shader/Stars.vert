#version 140

uniform mat4 PVMmatrix;     // Projection * View * Model --> model to clip coordinates
uniform float time;         // used for simulation of moving lights (such as sun)

in vec3 position;           // vertex position in world space
in vec2 texCoord;           // incoming texture coordinates

smooth out vec2 texCoord_v; // outgoing texture coordinates
//out float disappearing;

float speed = 10f;
int width = 8;
ivec2 pattern = ivec2(8, 1);

void main() {

  // vertex position after the projection (gl_Position is predefined output variable)
  gl_Position = PVMmatrix * vec4(position, 1.0f);   // outgoing vertex in clip coordinates

  float offsetParameter = time * speed;
  // localTime = 0;

  // outputs entering the fragment shader

  
  vec2 offset = vec2(1.0f) / vec2(pattern);

  vec2 texCoordBase = texCoord / vec2(pattern);
  texCoord_v = texCoordBase + vec2(int(mod(offsetParameter, pattern.x)), 0.0f) * offset;

  //disappearing = clamp(exp2(-gl_Position.z),0.0,1.0);
  //texCoord_v = texCoord;
}
