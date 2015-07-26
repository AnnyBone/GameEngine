varying vec4 diffuseColour;
varying vec4 normalColour;

uniform sampler2D diffuseTexture;
uniform sampler2D fullbrightTexture;
uniform sampler2D sphereTexture;

void main()
{
	vec4 diffuse = texture2D(diffuseTexture, gl_TexCoord[0].st);

	//gl_FragColor = diffuse;
	gl_FragColor = vec4(diffuseColour.xyz, 1.0) * diffuse;
	//gl_FragColor = normalize(normalColour);
}