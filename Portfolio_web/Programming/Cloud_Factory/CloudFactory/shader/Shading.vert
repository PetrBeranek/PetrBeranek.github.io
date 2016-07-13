#version 140

struct Material {   
  vec3  ambient;   
  vec3  diffuse;   
  vec3  specular;  
  float shininess;  

  bool  useTexture;  
};

struct LightDir {   
  vec3  ambient; 
  vec3  diffuse;  
  vec3  specular;  
  vec3  position; 
};

struct LightPoint {
  vec3  ambient;
  vec3  diffuse;
  vec3  specular;
  vec3  position;
  float  fallOff;
};

struct Fog {
	vec4 color;
	float density;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform Material material;
uniform LightDir lightDir1;
uniform LightDir lightDir2;
uniform Fog fog;

uniform LightPoint lightPoint;

uniform mat4 PVMmatrix;
uniform mat4 Vmatrix;
uniform mat4 Mmatrix;
uniform mat4 normalMatrix;

out float fogValue;
smooth out vec2 texCoord_v;
smooth out vec4 color_v;

vec4 directionalLight(Material material, LightDir light, vec3 vertexPosition, vec3 vertexNormal) {

	vec3 ret = vec3(0.0f);
	vec3 modified = (Vmatrix*vec4(light.position,0.0f)).xyz;
  
   vec3 L = normalize(modified);
   vec3 R = normalize(reflect(-L,vertexNormal));
   vec3 V = normalize(-vertexPosition);

   ret += material.diffuse * light.diffuse * max(dot(L,vertexNormal),0);
   ret += material.ambient * light.ambient;
   ret + material.specular * light.specular * pow(max(dot(R,V),0),material.shininess);

  return vec4(ret, 1.0f);
	//return vec4(material.diffuse,0.0f);

	
	/*
	vec4 myDiff = vec4(material.diffuse,0.0f)*dot(normalize(-vertexPosition),normalize(vertexNormal));
	vec4 myDiff2 = vec4(material.diffuse,0.0f)*dot(normalize(-vertexPosition),normalize(-vertexNormal));

	return vec4(max(myDiff.x,myDiff2.x),max(myDiff.y,myDiff2.y),max(myDiff.z,myDiff2.z),0.0f);
	*/
	
	//vec4 myDiff = vec4(material.diffuse,0.0f)*dot(normalize(-vertexPosition),normalize(vertexNormal));

	//return myDiff;
}

vec4 pointLight(Material material, LightPoint light, vec3 vertexPosition, vec3 vertexNormal) {
	vec3 toCamera = (Vmatrix*vec4(light.position,1.0f)).xyz - vertexPosition;
	float dist = 1/length(toCamera);
	dist = dist*dist;

	vec3 ret = vec3(0.0f);
  
   vec3 L = normalize(toCamera);
   vec3 R = normalize(reflect(-L,vertexNormal));
   vec3 V = normalize(-vertexPosition);

   ret += material.diffuse * light.diffuse * max(dot(L,vertexNormal),0) * dist * light.fallOff;
   ret += material.ambient * light.ambient * dist * light.fallOff;
   ret + material.specular * light.specular * pow(max(dot(R,V),0),material.shininess) * dist * light.fallOff;

  return vec4(ret, 1.0f);
}
/*
float dist(vec3 vec){

}
*/
void main() {
  vec3 vertexPosition = (Vmatrix * Mmatrix * vec4(position, 1.0)).xyz;
  vec3 vertexNormal   = normalize( (normalMatrix * vec4(normal, 0.0) ).xyz);


  vec3 globalAmbientLight = vec3(0.5f);
  vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0f);

  outputColor += directionalLight(material, lightDir1, vertexPosition, vertexNormal);
  outputColor += directionalLight(material, lightDir2, vertexPosition, vertexNormal);
  outputColor += pointLight(material, lightPoint, vertexPosition, vertexNormal);

  gl_Position = PVMmatrix * vec4(position, 1);

  texCoord_v = texCoord;

  if(fog.density >= 0){
	fogValue = clamp(exp2(-gl_Position.z*fog.density),0.0,1.0);
  }else{
	fogValue = 1-clamp(exp(length((Mmatrix * vec4(position, 1.0)).xyz)*fog.density),0.0,1.0);
  }
	/*gl_Position = vec4(1.0f);
	color_v = vec4(1.0f);
	texCoord_v = vec2(1.0f);*/
  /*
  int border = 10;
  float sum = (floor((outputColor.x + outputColor.y + outputColor.z) / 2 * border))/border;
  float sum2 = 3/(outputColor.r+outputColor.g+outputColor.b);
  outputColor = outputColor * sum * sum2;
  */
  
  color_v = outputColor;
}


