#version 140

uniform sampler2D texSampler;  // sampler for texture access

smooth in vec2 texCoord_v;     // incoming fragment texture coordinates
out vec4 color_f;              // outgoing fragment color
//in float disappearing;

void main() {

  // fragment color is given only by the texture
	vec4 col= texture(texSampler, texCoord_v);
	//color_f = mix(vec4(0.0f),col,gl_FragCoord.z);
	color_f = col;
}