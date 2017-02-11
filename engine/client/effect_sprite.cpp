/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "../engine_base.h"

#include "../video.h"		// TODO: make this a base include
#include "video_camera.h"
#include "effect_sprite.h"

using namespace core;

ConsoleVariable_t cv_sprite_debugsize = { "sprite_debugsize", "0", false, false, "If enabled, shows the area that the sprite covers." };

SpriteManager *g_spritemanager = nullptr;

// Sprite Manager

#define	SPRITE_DEFAULT_MAX	1024

SpriteManager::SpriteManager()
{
	Con_Printf("Initializing Sprite Manager...\n");

	Cvar_RegisterVariable(&cv_sprite_debugsize, NULL);

	sprites.reserve(SPRITE_DEFAULT_MAX);
}

SpriteManager::~SpriteManager()
{
	Con_Printf("Shutting down Sprite Manager...\n");

	Clear();
}

/*	Add a new sprite to the manager.
*/
Sprite *SpriteManager::Add()
{
	Sprite *sprite = new Sprite();
	sprites.push_back(sprite);
	return sprite;
}

/*	Removes a single sprite.
*/
void SpriteManager::Remove(Sprite *sprite)
{
	for (auto iterator = sprites.begin(); iterator != sprites.end(); ++iterator)
		if (sprite == *iterator)
		{
			sprites.erase(iterator);
			delete (*iterator);
			break;
		}
}

/*	Clears out all sprites.
*/
void SpriteManager::Clear()
{
	for (auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite)
		delete (*sprite);

	sprites.clear();	// Clear the vector.
}

/*	Run through and simulate each sprite individually.
*/
void SpriteManager::Simulate()
{
	for (unsigned int i = 0; i < sprites.size(); i++)
		sprites[i]->Simulate();
}

void SpriteManager::Draw()
{
	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		// Skip sprites that aren't currently visible.
		if (!sprites[i]->IsVisible())
			continue;

		sprites[i]->Draw();
	}
}

/*
	Sprite
*/

Sprite::Sprite() :
	ident(0),
	isactive(false),
	islit(false),
	isvisible(false),
	scale(5.0f),
	material(g_mMissingMaterial)
{
	plVectorClear(position);
	plVectorClear(mins);
	plVectorClear(maxs);

	plVector4Setf(1.0f, colour);

	draw = vlCreateDraw(VL_PRIMITIVE_TRIANGLES, 2, 4);
}

Sprite::~Sprite()
{
	vlDeleteDraw(draw);
}

void Sprite::SetColour(float r, float g, float b, float a)
{
	colour[0] = r;
	colour[1] = g;
	colour[2] = b;
	colour[3] = a;
}

void Sprite::SetPosition(plVector3f_t nposition)
{
	// Just use the other func.
	SetPosition(nposition[0], nposition[1], nposition[2]);
}

void Sprite::SetPosition(float x, float y, float z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void Sprite::SetType(SpriteType_t stype)
{
	// Don't bother if we're already that type.
	if (type == stype)
		return;

	// Otherwise update, and set defaults.
	type = stype;
	switch (type)
	{
	default:
	case SPRITE_TYPE_DEFAULT:
		// Otherwise by default, clipped by actual scale.
		Math_VectorSet(-16.0f, mins);
		Math_VectorSet(16.0f, maxs);
		break;
	case SPRITE_TYPE_FLARE:
	case SPRITE_TYPE_SCALE:
		// Flares get clipped by origin position, so no additional size.
		plVectorClear(mins);
		plVectorClear(maxs);
		break;
	}
}

void Sprite::SetScale(float nscale)
{
	// Little bit silly, sometimes we have an invalid scale.
	if (nscale <= 0)
		nscale = 1.0f;

	scale = nscale;
}

void Sprite::SetMaterial(Material_t *nmaterial)
{
	if (!nmaterial)
		return;

	material = nmaterial;
}

void Sprite::Simulate()
{
	// TODO: last bit is a hack, since having the console open doesn't count as being paused...
	if (!isactive || cl.paused || ((key_dest == key_console) && (svs.maxclients == 1)))
		return;

	Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera)
		return;

	isvisible = true;

	if (colour[3] <= 0)
		isvisible = false;

	// Ensure it's on screen.
	PLVector3f mvmins, mvmaxs;
	plVectorAdd3fv(position, mins, mvmins);
	plVectorAdd3fv(position, maxs, mvmaxs);
	if (((type == SPRITE_TYPE_FLARE) && camera->IsPointOutsideFrustum(position)) ||
		camera->IsBoxOutsideFrustum(mvmins, mvmaxs))
		isvisible = false;

	// Not visible.
	if (!isvisible)	
		return;

	switch (type)
	{
	default:
	case SPRITE_TYPE_DEFAULT:
		if (islit)
			Math_MVToVector(Light_GetSample(position), colour);
		break;
	case SPRITE_TYPE_FLARE:
	case SPRITE_TYPE_SCALE:
		// Scale the sprite, dependant on view position.
		scale *=
			(position[0] - camera->GetPosition()[0]) * camera->GetForward()[0] +
			(position[1] - camera->GetPosition()[1]) * camera->GetForward()[1] +
			(position[2] - camera->GetPosition()[2]) * camera->GetForward()[2];
		break;
	}
}

void Sprite::Draw()
{
	// Not visible.
	if (!isvisible)	return;

	Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera)
		return;

	if (colour[3] < 1.0f)
		plEnableGraphicsStates(VL_CAPABILITY_BLEND);

	vlBeginDraw(draw);
	vlDrawVertex3f(-scale, scale, 0);
	vlDrawVertex3f(scale, scale, 0);
	vlDrawVertex3f(scale, -scale, 0);
	vlDrawVertex3f(-scale, -scale, 0);
	vlEndDraw(draw);

	plDraw(draw);

	if (colour[3] < 1.0f)
		plDisableGraphicsStates(VL_CAPABILITY_BLEND);

	if (cv_sprite_debugsize.boolean_value)
	{
		// We need the size relative to the current position.
		PLVector3f NewMins = { 0 }, NewMaxs = { 0 };
		plVectorAdd3fv(mins, position, NewMins);
		plVectorAdd3fv(maxs, position, NewMaxs);
		
		// Draw a point representing the current position.
		draw::CoordinateAxes(position);
		
		// Draw the bounding box.
		draw::WireBox(NewMins, NewMaxs, 0, 1.0f, 0);
	}
}

/*	Sprite Manager C Interface	*/

extern "C" void SpriteManager_Simulate(void)
{
	g_spritemanager->Simulate();
}

extern "C" void SpriteManager_Draw(void)
{
	g_spritemanager->Draw();
}

extern "C" ISprite *SpriteManager_Add(void)
{
	return g_spritemanager->Add();
}

extern "C" void SpriteManager_Remove(ISprite *sprite)
{
	g_spritemanager->Remove(dynamic_cast<Sprite*>(sprite));
}

extern "C" void SpriteManager_Clear(void)
{
	g_spritemanager->Clear();
}

/*	Sprite C Interface	*/

extern "C" void Sprite_SetPosition(ISprite *sprite, plVector3f_t position)
{
	sprite->SetPosition(position);
}

extern "C" void Sprite_SetColour(ISprite *sprite, float r, float g, float b, float a)
{
	sprite->SetColour(r, g, b, a);
}

extern "C" void Sprite_SetType(ISprite *sprite, SpriteType_t type)
{
	sprite->SetType(type);
}

extern "C" void Sprite_SetScale(ISprite *sprite, float scale)
{
	sprite->SetScale(scale);
}

extern "C" void Sprite_SetMaterial(ISprite *sprite, Material_t *material)
{
	sprite->SetMaterial(material);
}

/**/
