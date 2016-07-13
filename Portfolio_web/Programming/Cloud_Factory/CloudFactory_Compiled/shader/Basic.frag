#version 140

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

struct Fog {
	vec4 color;
	float density;
};

uniform sampler2D texSampler;  // sampler for the texture access

uniform Material material;     // current material
uniform Fog fog;

in float fogValue;
smooth in vec4 color_v;        // incoming fragment color (includes lighting)
smooth in vec2 texCoord_v;     // fragment texture coordinates
out vec4       color_f;        // outgoing fragment color

void main() {
	vec4 col;

	col = color_v;

  // if material has a texture -> apply it

	if(material.useTexture){
		col =  color_v * texture(texSampler, texCoord_v);
	}

	color_f = mix(fog.color,col,fogValue);
	color_f.w = col.w;
	//color_f = col;
}