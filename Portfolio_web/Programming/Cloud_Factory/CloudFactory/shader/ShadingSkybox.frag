  #version 140
  
  uniform samplerCube skyboxSampler;
  in vec3 texCoord_v;
  out vec4 color_f;
  
  void main() {
    vec4 color_v = texture(skyboxSampler, texCoord_v);
	int border = 10;

	float sum = 3/(color_v.r+color_v.g+color_v.b);
	float sum2 = (floor((color_v.r+color_v.g+color_v.b)/3*border)+3)/border;

	
	//sum2 = (floor(sum2*border))/border;
	color_f = vec4(color_v.r*sum2, color_v.g*sum2, color_v.b*sum2, color_v.a);

	int test = 2*int(mod((gl_FragCoord.x+gl_FragCoord.y)/8,2)) + int(mod((gl_FragCoord.x-gl_FragCoord.y)/8,2));

	if(test <= 0){
		color_f = vec4(color_v.r*color_v.r, color_v.g*color_v.g ,color_v.b*color_v.b ,color_v.a);
	}else if(test <= 1){
		color_f = color_v;
	}else if(test <= 2){
		color_f = color_v;
	}else{
		color_f = vec4(sqrt(color_v.r), sqrt(color_v.g) ,sqrt(color_v.b) ,color_v.a);
	}
  }