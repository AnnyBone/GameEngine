uniform sampler2D SampleTexture;

void main()
{
	vec4 Texture = texture2D(SampleTexture, gl_TexCoord[0].st);
	gl_FragColor = Texture; //vec4(1.0, 0, 0, 1.0);
}
