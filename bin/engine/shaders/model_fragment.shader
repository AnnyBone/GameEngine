varying vec4 diffuseColour;
varying vec3 normalColour;

uniform sampler2D diffuseTexture;
uniform sampler2D fullbrightTexture;
uniform sampler2D detailTexture;
uniform sampler2D sphereTexture;

void main()
{
	vec4 diffuse = texture2D(diffuseTexture, gl_TexCoord[0].st) * vec4(diffuseColour.xyz, 1.0);
	vec4 fullbright = texture2D(fullbrightTexture, gl_TexCoord[0].st);
	
	gl_FragColor = vec4(normalColour, 1.0);
	//gl_FragColor = diffuse;
	//gl_FragColor = vec4(diffuseColour.xyz, 1.0);
}