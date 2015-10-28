/*	Copyright (C) 2011-2015 OldTimes Software

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

varying vec4 v_diffusecolour;
varying vec4 v_normalcolour;

uniform sampler2D diffuseTexture;
uniform sampler2D fullbrightTexture;
uniform sampler2D sphereTexture;

uniform	float	u_alphaclamp;
uniform	bool	u_alphatest;

void main()
{
	vec4 diffuse = texture2D(diffuseTexture, gl_TexCoord[0].st);

	// Alpha-testing.
	if(u_alphatest == true)
		if(diffuse.a > u_alphaclamp)
			discard;

	//gl_FragColor = diffuse;
	gl_FragColor = vec4(v_diffusecolour.xyz, 1.0) * diffuse;
	//gl_FragColor = normalize(v_normalcolour);
}