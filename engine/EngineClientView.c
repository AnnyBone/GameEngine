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

#include "engine_base.h"

#include "video.h"
#include "EngineGame.h"

/*	The view is allowed to move slightly from it's true position for bobbing,
	but if it exceeds 8 pixels linear distance (spherical, not box), the list of
	entities sent from the server may not include everything in the pvs, especially
	when crossing a water boundary.
*/

cvar_t	scr_ofsx = { "scr_ofsx", "0", false };
cvar_t	scr_ofsy = { "scr_ofsy", "0", false };
cvar_t	scr_ofsz = { "scr_ofsz", "0", false };
cvar_t	cl_rollspeed = { "cl_rollspeed", "200" };
cvar_t	cl_rollangle = { "cl_rollangle", "2.0" };
cvar_t	cl_bob = { "cl_bob", "0.002", false };
cvar_t	cl_bobcycle = { "cl_bobcycle", "0.43", false };
cvar_t	cl_bobup = { "cl_bobup", "0.45", false };
cvar_t	cSideBobUp = { "view_sideup", "0.5", false };
cvar_t	cSideBobCycle = { "view_sidecycle", "0.86", false };
cvar_t	cSideBob = { "view_sidebob", "0.007", false };
cvar_t	cViewModelLag = { "view_modellag", "0.2" };
cvar_t	v_kicktime = { "v_kicktime", "0.5", false };
cvar_t	v_kickroll = { "v_kickroll", "0.6", false };
cvar_t	v_kickpitch = { "v_kickpitch", "0.6", false };
cvar_t	v_gunkick = { "v_gunkick", "1" }; //johnfitz
cvar_t	v_iyaw_cycle = { "v_iyaw_cycle", "2", false };
cvar_t	v_iroll_cycle = { "v_iroll_cycle", "0.5", false };
cvar_t	v_ipitch_cycle = { "v_ipitch_cycle", "1", false };
cvar_t	v_iyaw_level = { "v_iyaw_level", "0.3", false };
cvar_t	v_iroll_level = { "v_iroll_level", "0.1", false };
cvar_t	v_ipitch_level = { "v_ipitch_level", "0.3", false };
cvar_t	v_idlescale = { "v_idlescale", "0", false },
cViewModelPosition = { "view_modelposition", "2", true, false, "0 = Center, 1 = Left and 2 = Right." };
cvar_t	gl_cshiftpercent = { "gl_cshiftpercent", "100", false };

float	v_dmg_time, v_dmg_roll, v_dmg_pitch;

extern	int	in_forward, in_forward2, in_back;

plVector3f_t	v_punchangles[2]; 
plVector3f_t	forward, right, up;

/*	Used by view and sv_user
*/
float V_CalcRoll(plVector3f_t angles, plVector3f_t velocity)
{
	float	sign,side,value;

	plAngleVectors(angles, forward, right, up);
	side = Math_DotProduct (velocity, right);
	sign = side < 0 ? -1 : 1;
	side = fabs(side);

	value = cl_rollangle.value;

	if (side < cl_rollspeed.value)
		side = side * value / cl_rollspeed.value;
	else
		side = value;

	return side*sign;

}

/*
==============================================================================

	VIEW BLENDING

==============================================================================
*/

cshift_t	cshift_empty	= {	{	0,		0,	0	},	0	};
cshift_t	cshift_water	= { {	0,		0,	100	},	100	};
cshift_t	cshift_slime	= { {	0,		25,	5	},	150 };
cshift_t	cshift_lava		= { {	255,	80,	0	},	150 };

plColour_t vViewBlend = { 0, 0, 0, 255.0f };	// rgba 0.0 - 1.0

void V_ParseDamage (void)
{
	int		armor, blood;
	MathVector3f_t from;
	int		i;
	MathVector3f_t mvForward, mvRight, mvUp;
	ClientEntity_t *ent;
	float	side;
	float	count;

	armor = MSG_ReadByte ();
	blood = MSG_ReadByte ();
	for (i=0 ; i<3 ; i++)
		from[i] = MSG_ReadCoord ();

	count = blood*0.5f+armor*0.5f;
	if(count < 10)
		count = 10;

	cl.cshifts[CSHIFT_DAMAGE].percent += 3*count;
	if (cl.cshifts[CSHIFT_DAMAGE].percent < 0)
		cl.cshifts[CSHIFT_DAMAGE].percent = 0;
	if (cl.cshifts[CSHIFT_DAMAGE].percent > 150)
		cl.cshifts[CSHIFT_DAMAGE].percent = 150;

	if (armor > blood)
	{
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 200;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 100;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 100;
	}
	else if (armor)
	{
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 220;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 50;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 50;
	}
	else
	{
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 255;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 0;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 0;
	}

	// Calculate view angle kicks
	ent = &cl_entities[cl.viewentity];

	Math_VectorSubtract(from,ent->origin,from);
	plVectorNormalize(from);

	plAngleVectors(ent->angles, mvForward, mvRight, mvUp);

	side = Math_DotProduct (from, mvRight);
	v_dmg_roll = count*side*v_kickroll.value;

	side = Math_DotProduct (from, mvForward);
	v_dmg_pitch = count*side*v_kickpitch.value;

	v_dmg_time = v_kicktime.value;
}

void V_cshift_f (void)
{
	cshift_empty.destcolor[0]	= atoi(Cmd_Argv(1));
	cshift_empty.destcolor[1]	= atoi(Cmd_Argv(2));
	cshift_empty.destcolor[2]	= atoi(Cmd_Argv(3));
	cshift_empty.percent		= atoi(Cmd_Argv(4));
}

/*	When you run over an item, the server sends this command
*/
void V_BonusFlash_f(void)
{
	cl.cshifts[CSHIFT_BONUS].destcolor[0]	= 215;
	cl.cshifts[CSHIFT_BONUS].destcolor[1]	= 186;
	cl.cshifts[CSHIFT_BONUS].destcolor[2]	= 69;
	cl.cshifts[CSHIFT_BONUS].percent		= 50;
}

/*	Underwater, lava, etc each has a color shift
*/
void V_SetContentsColor(int contents)
{
	switch(contents)
	{
	case BSP_CONTENTS_LAVA:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_lava;
		break;
	case BSP_CONTENTS_SLIME:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_slime;
		break;
	case BSP_CONTENTS_WATER:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_water;
		break;
	default:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_empty;
	}
}

void V_CalcPowerupCshift (void)
{
#if 0
	if (cl.items & IT_QUAD)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 255;
		cl.cshifts[CSHIFT_POWERUP].percent = 30;
	}
	else if (cl.items & IT_SUIT)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 0;
		cl.cshifts[CSHIFT_POWERUP].percent = 20;
	}
	else if (cl.items & IT_INVISIBILITY)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 100;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 100;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 100;
		cl.cshifts[CSHIFT_POWERUP].percent = 100;
	}
	else if (cl.items & IT_INVULNERABILITY)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 0;
		cl.cshifts[CSHIFT_POWERUP].percent = 30;
	}
	else
		cl.cshifts[CSHIFT_POWERUP].percent = 0;
#else
	cl.cshifts[CSHIFT_POWERUP].percent = 0;
#endif
}

// [26/3/2013] Revised ~hogsy
void View_CalculateBlend(void)
{
	plColour_t	vColour;
	float		fAlpha;
	int			j;

	plVectorClear(vColour);

	for(j = 0; j < NUM_CSHIFTS; j++)
	{
		if(!gl_cshiftpercent.value)
			continue;

		//johnfitz -- only apply leaf contents color shifts during intermission
		if(cl.intermission && j != CSHIFT_CONTENTS)
			continue;
		//johnfitz

		fAlpha = ((cl.cshifts[j].percent*gl_cshiftpercent.value)/100.0)/255.0f;
		if(!fAlpha)
			continue;

		vColour[ALPHA] += fAlpha*(1 - vColour[ALPHA]);
		fAlpha /= vColour[ALPHA];

		vColour[RED]	= cl.cshifts[j].destcolor[RED] * fAlpha;
		vColour[GREEN]	= cl.cshifts[j].destcolor[GREEN] * fAlpha;
		vColour[BLUE]	= cl.cshifts[j].destcolor[BLUE] * fAlpha;
	}

	Math_VectorDivide(vColour, 255.0f, vViewBlend);

	vViewBlend[ALPHA] = vColour[ALPHA];

	if(vViewBlend[3] > 1.0f)
		vViewBlend[3] = 1.0f;
	else if(vViewBlend[3] < 0)
		vViewBlend[3] = 0;
}

void V_UpdateBlend(void)
{
	int		i,j;
	bool	bBlendChanged = false;

	V_CalcPowerupCshift();

	for(i = 0; i < NUM_CSHIFTS; i++)
	{
		if (cl.cshifts[i].percent != cl.prev_cshifts[i].percent)
		{
			bBlendChanged = true;
			cl.prev_cshifts[i].percent = cl.cshifts[i].percent;
		}

		for (j=0 ; j<3 ; j++)
			if (cl.cshifts[i].destcolor[j] != cl.prev_cshifts[i].destcolor[j])
			{
				bBlendChanged = true;
				cl.prev_cshifts[i].destcolor[j] = cl.cshifts[i].destcolor[j];
			}
	}

	// Drop the damage value
	cl.cshifts[CSHIFT_DAMAGE].percent -= host_frametime*150;
	if(cl.cshifts[CSHIFT_DAMAGE].percent <= 0)
		cl.cshifts[CSHIFT_DAMAGE].percent = 0;

	// Drop the bonus value
	cl.cshifts[CSHIFT_BONUS].percent -= host_frametime*100;
	if(cl.cshifts[CSHIFT_BONUS].percent <= 0)
		cl.cshifts[CSHIFT_BONUS].percent = 0;

	if(bBlendChanged)
		View_CalculateBlend();
}

void View_PolyBlend(void)
{
#if 0
	if (!gl_polyblend.value || !cl.cshifts[CSHIFT_CONTENTS].percent)
		return;

	vlDisable(VL_CAPABILITY_DEPTH_TEST);
	Draw_Rectangle(0, 0, Video.iWidth, Video.iHeight, vViewBlend);
	vlEnable(VL_CAPABILITY_DEPTH_TEST);
#endif
}

/*
==============================================================================

	VIEW RENDERING

==============================================================================
*/

/*	Adds a delay to the view model (such as a weapon) in the players view.
*/
void View_ModelDrift(plVector3f_t vOrigin, plVector3f_t vAngles, plVector3f_t vOldAngles)
{
	int						i;
	float					fScale,fSpeed,fDifference;
	static	plVector3f_t	svLastFacing;
	plVector3f_t			vForward, vRight, vUp,
							vDifference;

	plAngleVectors(vAngles, vForward, vRight, vUp);

	if(host_frametime != 0.0f)
	{
		plVectorSubtract3fv(vForward, svLastFacing, vDifference);

		fSpeed = 3.0f;

		fDifference = plLengthf(vDifference);
		if((fDifference > cViewModelLag.value) && (cViewModelLag.value > 0.0f ))
			fSpeed *= fScale = fDifference/cViewModelLag.value;

		for(i = 0; i < 3; i++)
			svLastFacing[i] += vDifference[i]*(fSpeed*host_frametime);

		plVectorNormalize(svLastFacing);

		for(i = 0; i < 3; i++)
		{
			vOrigin[i] += (vDifference[i]*-1.0f)*5.0f;
		//	r_refdef.viewangles[ROLL] += vDifference[YAW];
			vAngles[ROLL] += vDifference[YAW];
		}
	}
}

void V_CalcRefdef (void)
{
#if 0
	int				i;
	float			fBob[2],fCycle[2],
					delta;
	static	float	oldz = 0;
	ClientEntity_t	*ent,*view;
	MathVector3f_t	forward_vector,right_vector,mvUp,
					angles,vOldAngles;
	static MathVector3f_t punch = {0,0,0}; //johnfitz -- v_gunkick

	V_DriftPitch();

	// ent is the player model (visible when out of body)
	ent = &cl_entities[cl.viewentity];
	// view is the weapon model (only visible from inside body)
	view = &cl.viewent;

	// transform the view offset by the model's matrix to get the offset from
	// model origin for the view
	ent->angles[YAW]	= cl.viewangles[YAW];	// the model should face the view dir
	ent->angles[PITCH]	= -cl.viewangles[PITCH];	// the model should face the view dir
	ent->angles[ROLL]	= -cl.viewangles[ROLL];

	// [10/5/2013] Forward cycle ~hogsy
	fCycle[0] = (cl.time-(int)(cl.time/cl_bobcycle.value)*cl_bobcycle.value)/cl_bobcycle.value;
	if(fCycle[0] < cl_bobup.value)
		fCycle[0] = PL_PI * fCycle[0] / cl_bobup.value;
	else
		fCycle[0] = PL_PI + PL_PI * (fCycle[0] - cl_bobup.value) / (1.0f - cl_bobup.value);

	// [10/5/2013] Side cycle ~hogsy
	fCycle[1] = (cl.time-(int)(cl.time/cSideBobCycle.value)*cSideBobCycle.value)/cSideBobCycle.value;
	if(fCycle[1] < cSideBobUp.value)
		fCycle[1] = PL_PI * fCycle[1] / cSideBobUp.value;
	else
		fCycle[1] = PL_PI + PL_PI*(fCycle[1] - cSideBobUp.value) / (1.0f - cSideBobUp.value);

	/*	Bob is proportional to velocity in the xy plane
		(don't count Z, or jumping messes it up)		*/
	fBob[0] = sqrt(
		cl.velocity[0]*cl.velocity[0]+
		cl.velocity[1]*cl.velocity[1])
		*cl_bob.value;
	fBob[1] = sqrt(
		cl.velocity[0]*cl.velocity[0]+
		cl.velocity[1]*cl.velocity[1])
		*cSideBob.value;

	// [10/5/2013] Bleh... Better/cleaner method? ~hogsy
	for(i = 0; i < 2; i++)
	{
		fBob[i] *= fBob[i]*0.7f*sin(fCycle[i]);
		if(fBob[i] > 4.0f)
			fBob[i] = 4.0f;
		else if(fBob[i] < -7.0f)
			fBob[i] = -7.0f;
	}

	// [10/5/2013] Save our old angles for later... ~hogsy
	Math_VectorCopy(view->angles,vOldAngles);

	// Refresh position
	Math_VectorCopy(ent->origin,r_refdef.vieworg);
	r_refdef.vieworg[2] += cl.viewheight+fBob[0];

	/*	Never let it sit exactly on a node line, because a water plane can
		dissapear when viewed with the eye exactly on it.
		The server protocol only specifies to 1/16 pixel, so add 1/32 in each axis.
	*/
	Math_VectorAddValue(r_refdef.vieworg,1.0f/32.0f,r_refdef.vieworg);

	Math_VectorCopy(cl.viewangles,r_refdef.viewangles);

	V_CalcViewRoll();
	V_AddIdle();

	// offsets
	angles[PITCH]	= -ent->angles[PITCH]; // because entity pitches are actually backward
	angles[YAW]		= ent->angles[YAW];
	angles[ROLL]	= ent->angles[ROLL];

	plAngleVectors(angles, forward_vector, right_vector, mvUp);

	if(cl.maxclients <= 1) //johnfitz -- moved cheat-protection here from V_RenderView
		for(i=0; i<3; i++)
			r_refdef.vieworg[i] += scr_ofsx.value*forward_vector[i]+scr_ofsy.value*right_vector[i]+scr_ofsz.value*mvUp[i];

	V_BoundOffsets ();

	// Set up gun position
	Math_VectorCopy(cl.viewangles,view->angles);

	Math_VectorCopy(ent->origin,view->origin);
	view->origin[2] += cl.viewheight;

	for(i = 0; i < 3; i++)
		view->origin[i] +=	(mvUp[i]*fBob[0]*0.2f)+
							(right_vector[i]*fBob[1]*0.3f);

	view->origin[2] += fBob[0];

	//johnfitz -- removed all gun position fudging code (was used to keep gun from getting covered by sbar)

	view->model	= cl.model_precache[cl.stats[STAT_WEAPON]];
	view->frame	= cl.stats[STAT_WEAPONFRAME];

	View_ModelAngle();

//johnfitz -- v_gunkick
	if(v_gunkick.value) //lerped kick
	{
		for(i = 0; i < 3; i++)
			if (pl_origin3f[i] != v_punchangles[0][i])
			{
				// Speed determined by how far we need to lerp in 1/10th of a second
				delta = (v_punchangles[0][i]-v_punchangles[1][i])*host_frametime*10.0f;
				if(delta > 0)
					punch[i] = Math_Min(punch[i]+delta,v_punchangles[0][i]);
				else if(delta < 0)
					punch[i] = Math_Max(punch[i]+delta,v_punchangles[0][i]);
			}

		Math_VectorAdd(r_refdef.viewangles,punch,r_refdef.viewangles);
	}
//johnfitz

// smooth out stair step ups
	if (!noclip_anglehack && cl.bIsOnGround && ent->origin[2] - oldz > 0) //johnfitz -- added exception for noclip
	//FIXME: noclip_anglehack is set on the server, so in a nonlocal game this won't work.
	{
		float fStepTime;

		fStepTime = (float)(cl.time-cl.oldtime);
		if(fStepTime < 0)
			//FIXME	I_Error ("steptime < 0");
			fStepTime = 0;

		oldz += fStepTime*80.0f;
		if(oldz > ent->origin[2])
			oldz = ent->origin[2];

		if(ent->origin[2]-oldz > 12.0f)
			oldz = ent->origin[2]-12.0f;

		r_refdef.vieworg[2] += oldz-ent->origin[2];
		view->origin[2]		+= oldz-ent->origin[2];
	}
	else
		oldz = ent->origin[2];
#endif
}
