/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2016 OldTimes Software

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

#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H

#include "shared_model.h"

//============================================================================

void	Model_Initialize(void);
void	Model_ClearAll(void);
model_t *Mod_ForName(const char *cName);
void	*Mod_Extradata (model_t *mod);	// handles caching
void	Model_Touch(char *cName);

mleaf_t *Mod_PointInLeaf (float *p, model_t *model);
uint8_t	*Mod_LeafPVS (mleaf_t *leaf, model_t *model);

MathVector_t Model_GenerateNormal(MathVector3f_t a, MathVector3f_t b, MathVector3f_t c);
MathVector_t Model_GenerateNormal3f(float aX, float aY, float aZ,float bX, float bY, float bZ,float cX, float cY, float cZ);

// OBJ Support (TODO: We can be smarter, why have specific drawing routines? Abstract things out.)
void Model_DrawOBJ(ClientEntity_t *eEntity);

#endif // !ENGINE_MODEL_H
