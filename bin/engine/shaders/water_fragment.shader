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

uniform sampler2D	u_normalmap;
uniform float		u_time;

void main()
{
	vec3	noisevec;
	vec2	displacement;
	float	scaledtimer;

	displacement = gl_TexCoord[0].st;

	scaledtimer = u_time * 0.1;

	displacement.x += scaledtimer;
	displacement.y -= scaledtimer;

	noisevec = normalize(texture2D(u_normalmap, displacement.xy).xyz);
	noisevec = (noisevec * 2.0 - 1.0) * 0.035;

	gl_FragColor = texture2D(diffuse, gl_TexCoord[0].st + noisevec.xy);
}
