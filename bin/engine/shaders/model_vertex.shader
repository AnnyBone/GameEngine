varying vec4 diffuseColour;
varying vec4 normalColour;

uniform vec3 lightPosition;
uniform vec3 lightColour;

uniform float vertexScale;

void main()
{
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = ftransform();
	
/*	vec3 viewNormal = gl_Normal;
	vec3 viewLightPosition = vec3(0.5, 0.5, 0.5) - gl_Vertex.xyz;
	
	float dist = length(viewLightPosition);
	float attenuation = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist);
	float intensity = max(dot(viewNormal, normalize(viewLightPosition)), 0.1);
	
	diffuseColour = vec4(lightColour, 1.0) * intensity * attenuation * 4.0;
	normalColour = vec4(gl_Normal.xyz, 1.0);*/
}