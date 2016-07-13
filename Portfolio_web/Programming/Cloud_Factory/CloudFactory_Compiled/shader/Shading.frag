#version 140

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

uniform sampler2D texSampler;  // sampler for the texture access

uniform Material material;     // current material

smooth in vec4 color_v;        // incoming fragment color (includes lighting)
smooth in vec2 texCoord_v;     // fragment texture coordinates
out vec4       color_f;        // outgoing fragment color

void main() {

	vec4 col = color_v;

  // if material has a texture -> apply it

	if(material.useTexture){
		col =  color_v * texture(texSampler, texCoord_v);
	}

	
	int border = 8;
	float sum = (1+floor((col.x + col.y + col.z) / 3 * border))/border;
	float sum2 = 3/(col.r + col.g + col.b);

	int i = 0;

	if( mod((gl_FragCoord.x-gl_FragCoord.y)/2,7) < 2){
		i += 2;
	}

	if( mod((gl_FragCoord.x+gl_FragCoord.y)/2,7) <= 2){
		i += 1;
	}
	
	float tr1 = 0.75;
	float tr2 = 0.50;
	float tr3 = 0.25;
	float tr4 = 0.1;
	float decrease = 0.25;
	float top = 0.8;

	if(i == 0){
		col = col * sum2 * top;
	}else if(i == 1){
		if(sum < tr4){
			col = vec4(0.0f);
		}else if(sum < tr2){
			col = col * sum2 * decrease;
		}else{
			col = col * sum2 * top;
		}
	}else if(i == 2){
		if(sum < tr3){
			col = vec4(0.0f);
		}else if(sum < tr1){
			col = col * sum2 * decrease;
		}else{
			col = col * sum2 * top;
		}
	}else{
		if(sum < tr3){
			col = vec4(0.0f);
		}else if(sum < tr1){
			col = col * sum2 * decrease;
		}else{
			col = col * sum2 * top;
		}
	}
	//col = col * sum * sum2;

	color_f = col;
	
}