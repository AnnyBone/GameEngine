/*	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

varying vec4 v_normalcolour;
varying vec4 v_diffusecolour;

uniform float u_vertexscale;

//attribute vec3 a_coords;

void main(void)
{
	//gl_Position = vec4(a_coords, 1.0);

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
}
