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

#include "engine_base.h"

using namespace std;

bool ModelU3D_Load(model_t *model, void *buf)
{
	U3DDataHeader_t			dataheader;
	U3DAnimationHeader_t	animheader;
	U3DTriangle_t			*poly;
	FILE					*dataf, *animf;

	COM_FOpenFile(model->name, &dataf);
	if (!dataf)
	{
		Con_Warning("Failed to load data file!\n");
		return false;
	}

	// Try to figure out the data string we used
	// then erase it.
	string newpath(model->name);
	size_t strpos = newpath.find("_d.3d");
	if (strpos != -1)
		newpath.erase(strpos);
	else
	{
		// Some legacy models use _Data...
		strpos = newpath.find("_Data.3d");
		if (strpos != -1)
			newpath.erase(strpos);
		else
		{
			fclose(dataf);

			Con_Warning("Invalid file name! (%s)\n", newpath.c_str());
			return false;
		}
	}
	newpath.append("_a.3d");

	// Attempt to load the animation file.
	COM_FOpenFile(newpath.c_str(), &animf);
	if (!animf)
	{
		// Some legacy models use _Anim...
		newpath.erase(newpath.length() - 2);
		newpath.append("_Anim.3d");

		COM_FOpenFile(newpath.c_str(), &animf);
		if (!animf)
		{
			fclose(animf);
			fclose(dataf);

			// Welp, I give up *drops everything and walks out*
			Con_Warning("Failed to load animation file!\n");
			return false;
		}
	}

	fclose(animf);
	fclose(dataf);

	return true;
}