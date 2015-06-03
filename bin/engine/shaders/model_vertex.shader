varying vec4 diffuseColour;
varying vec3 normalColour;

uniform vec3 lightPosition;
uniform vec3 lightColour;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
	
	vec3 viewVertex = normalize(gl_ModelViewMatrix * gl_Vertex);
	vec3 viewNormal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 viewLightPosition = normalize(gl_NormalMatrix * lightPosition);
	
	float dist = distance(viewNormal, viewLightPosition);
	
	diffuseColour = vec4((lightColour * 2.0) * dist,1.0) * max(dot(viewNormal, viewLightPosition), 0.0);
	
	normalColour = vec3(gl_Normal);
}