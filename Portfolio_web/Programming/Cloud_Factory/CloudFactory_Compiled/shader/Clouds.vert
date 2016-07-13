#version 140

uniform mat4 PVMmatrix;     // Projection * View * Model --> model to clip coordinates
uniform float time;         // used for simulation of moving lights (such as sun)

in vec3 position;           // vertex position in world space
in vec2 texCoord;           // incoming texture coordinates

smooth out vec2 texCoord_v; // outgoing texture coordinates
//out vec3 disappearing;

float speed = 0.1f;

void main() {

  // vertex position after the projection (gl_Position is predefined output variable)
  gl_Position = PVMmatrix * vec4(position, 1.0f);   // outgoing vertex in clip coordinates

  float offsetParameter = time * speed;
  // localTime = 0;

  vec2 offset = vec2(offsetParameter, 0.0f);

  // outputs entering the fragment shader
  texCoord_v = texCoord + offset;
  //disappearing = positio;
  //disappearing = clamp(exp2(-gl_Position.z),0.0,1.0);
  //texCoord_v = texCoord;
}
