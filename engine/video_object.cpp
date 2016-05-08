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

#include "video.h"
#include "video_shader.h"

/*	Calls up buffer and draws it.
*/
void Draw_StaticEntity(ClientEntity_t *entity)
{
	model_t *model = entity->model;
	if (!model)
		return;

	// TODO: TEMPORARY DEBUGGING STUFF!!!!
	vlPushMatrix();

	R_RotateForEntity(entity->origin, entity->angles);

	Material_Draw(model->materials, 0, VL_PRIMITIVE_IGNORE, 0, false);
	vlDraw(model->objects[entity->frame]);
	Material_Draw(model->materials, 0, VL_PRIMITIVE_IGNORE, 0, true);

	vlPopMatrix();
	// TODO: TEMPORARY DEBUGGING STUFF!!!!
}

void Draw_VertexEntity(ClientEntity_t *entity)
{
	model_t *model = entity->model;
	if (!model)
		return;

	vlPushMatrix();

	R_RotateForEntity(entity->origin, entity->angles);

	vlDraw(model->objects[entity->frame]);

	vlPopMatrix();
}
