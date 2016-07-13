  #version 140

struct Fog {
	vec4 color;
	float density;
};

  uniform Fog fog;  
  uniform samplerCube skyboxSampler;
  in vec3 texCoord_v;
  in float fogValue;
  out vec4 color_f;
  
  void main() {
	  if(fog.density <= 0){
		  color_f = texture(skyboxSampler, texCoord_v);
	  }else{
		  color_f = fog.color;
	  }
  }