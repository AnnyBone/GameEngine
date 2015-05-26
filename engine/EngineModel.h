/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef __ENGINEMODEL__
#define __ENGINEMODEL__

//
// Whole model
//

#define	MODEL_MAX_TEXTURES	32

#include "SharedModel.h"

//============================================================================

void	Model_Initialize(void);
void	Model_ClearAll(void);
model_t *Mod_ForName(char *cName);
void	*Mod_Extradata (model_t *mod);	// handles caching
void	Model_Touch(char *cName);

mleaf_t *Mod_PointInLeaf (float *p, model_t *model);
byte	*Mod_LeafPVS (mleaf_t *leaf, model_t *model);

// OBJ Support
void Model_DrawOBJ(entity_t *eEntity);

#endif	// __MODEL__
