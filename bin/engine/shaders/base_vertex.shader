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

uniform vec3	u_lightposition;
uniform vec4	u_lightcolour;

uniform float	u_vertexscale;

void main()
{
	v_normalcolour = vec4(gl_Normal, 1.0);
	v_diffusecolour = vec4(1.0, 1.0, 1.0, 1.0);

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();

	// Sphere-mapping
	/*
	vec3 u = normalize(vec3(gl_ModelViewMatrix * gl_Vertex));
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 r = reflect(u, n);
	float m = 2.0 * sqrt(r.x*r.x + r.y*r.y + (r.z + 1.0)*(r.z + 1.0));
	gl_TexCoord[1].s = r.x / m + 0.5;
	gl_TexCoord[1].t = r.y / m + 0.5;
	*/

	/*
	vec3 viewVertex = normalize(gl_ModelViewMatrix * gl_Vertex);
	vec3 viewNormal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 viewLightPosition = normalize(gl_NormalMatrix * u_lightposition);

	float dist = distance(viewNormal, viewLightPosition);

	v_diffusecolour = vec4((lightColour * 2.0) * dist,1.0) * max(dot(viewNormal, viewLightPosition), 0.0);
	*/
}