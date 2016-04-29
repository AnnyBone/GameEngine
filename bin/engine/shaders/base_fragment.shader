/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

varying vec4 v_diffusecolour;
varying vec4 v_normalcolour;

uniform sampler2D	u_diffusemap;
uniform sampler2D	u_detailmap;
uniform sampler2D	u_fullbrightmap;
uniform sampler2D	u_normalmap;
uniform sampler2D	u_spheremap;

uniform vec4	u_lightcolour;
uniform vec3	u_lightposition;

uniform	float	u_alphaclamp;
uniform	bool	u_alphatest;

void main()
{
	vec4 diffuse = texture2D(u_diffusemap, gl_TexCoord[0].st);

	// Alpha-testing.
	if (u_alphatest == true)
		if (diffuse.a < u_alphaclamp)
			discard;

	//gl_FragColor = diffuse;
	gl_FragColor = vec4(v_diffusecolour.xyz, 1.0) * diffuse;
	//gl_FragColor = normalize(v_normalcolour);
}