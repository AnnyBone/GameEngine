/*	Copyright (C) 2013-2015 OldTimes Software
*/
#include "server_weapon.h"

/*
	The secret Ion Rifle. 
	Inspired by Talon Brave's weapon in Prey '98!
*/

#define	IONRIFLE_MAX_RANGE	2000.0f

#define	IONRIFLE_IDLEA_FRAME	0
#define	IONRIFLE_IDLEB_FRAME	30

// [17/11/2013] Animation for the primary attack ~hogsy
ServerEntityFrame_t	efIonRiflePulseFireA[] =
{
	{	NULL,	10,	0.02f			},
	{	NULL,	11,	0.02f			},
	{	NULL,	12,	0.02f,	true	}
};

ServerEntityFrame_t	efIonRiflePulseFireB[] =
{
	{	NULL,	13,	0.02f			},
	{	NULL,	14,	0.02f			},
	{	NULL,	15,	0.02f,	true	}
};

ServerEntityFrame_t	efIonRiflePulseFireC[] =
{
	{	NULL,	16,	0.02f			},
	{	NULL,	17,	0.02f			},
	{	NULL,	18,	0.02f,	true	}
};

ServerEntityFrame_t	efIonRiflePulseFireD[] =
{
	{	NULL,	19,	0.02f			},
	{	NULL,	20,	0.02f			},
	{	NULL,	21,	0.02f,	true	}
};

ServerEntityFrame_t	efIonRiflePulseFireE[] =
{
	{	NULL,	22,	0.02f			},
	{	NULL,	23,	0.02f			},
	{	NULL,	24,	0.02f,	true	}
};

// [17/11/2013] Animation for switching to pulse mode (arms fold in) ~hogsy
ServerEntityFrame_t	efIonRiflePulseMode[] =
{
	{	NULL,	29,	0.03f			},
	{	NULL,	28,	0.03f			},
	{	NULL,	27,	0.03f			},
	{	NULL,	26,	0.03f			},
	{	NULL,	25,	0.03f			},
	{	NULL,	24,	0.03f,	true	}
};

ServerEntityFrame_t	efIonRifleBlastFire[] =
{
	{	NULL,	32,	0.03f			},
	{	NULL,	33,	0.03f			},
	{	NULL,	34,	0.03f			},
	{	NULL,	35,	0.03f			},
	{	NULL,	36,	0.03f,	true	}
};

// [17/11/2013] Should be just a reverse of the above (arms fold out) ~hogsy
ServerEntityFrame_t	efIonRifleBlastMode[] =
{
	{	NULL,	24,	0.03f			},
	{	NULL,	25,	0.03f			},
	{	NULL,	26,	0.03f			},
	{	NULL,	27,	0.03f			},
	{	NULL,	28,	0.03f			},
	{	NULL,	29,	0.03f,	true	}
};

ServerEntityFrame_t	efIonRifleDeploy[] =
{
	{	NULL,	1,	0.02f			},
	{	NULL,	2,	0.02f			},
	{	NULL,	3,	0.02f			},
	{	NULL,	4,	0.02f			},
	{	NULL,	5,	0.02f			},
	{	NULL,	6,	0.02f			},
	{	NULL,	7,	0.02f			},
	{	NULL,	8,	0.02f			},
	{	NULL,	9,	0.02f,	true	}
};

void IonRifle_IonBallTouch(ServerEntity_t *eIonBall,ServerEntity_t *eOther)
{
	if(eOther == eIonBall->local.eOwner)
		return;	// [29/1/2014] Should never even happen, but it does. Shit. ~hogsy
	else if (Engine.Server_PointContents(eIonBall->v.origin) == BSP_CONTENTS_SKY)
	{
		Entity_Remove(eIonBall);
		return;
	}

	if(eOther->v.bTakeDamage)
	{
		if(!eIonBall->local.style)
			Entity_Damage(eOther, eIonBall->local.eOwner, 25, 0);
		else
			Entity_Damage(eOther, eIonBall->local.eOwner, 50, 0);
	}

	Entity_Remove(eIonBall);
}

/*	Do the deploy animation.
*/
void IonRifle_Deploy(ServerEntity_t *eOwner)
{
	// [29/1/2014] Remember to reset everything we're going to use ~hogsy
	eOwner->local.iBarrelCount = 0;

	Weapon_Animate(eOwner,efIonRifleDeploy);
}

/*	Uses both primary burst and mega burst!
*/
void IonRifle_PrimaryAttack(ServerEntity_t *eOwner)
{
	ServerEntity_t	*eIonBall;

	switch(eOwner->local.iFireMode)
	{
	case 1:
		Weapon_Animate(eOwner,efIonRifleBlastFire);

		eOwner->v.punchangle[0] -= (float)(((rand()%10)+5));

		eOwner->local.ionblaster_ammo -= 5;

		{
			eIonBall = Entity_Spawn();
			if(eIonBall)
			{
				plVector3f_t vOrigin;

				eIonBall->v.cClassname = "ionball";
				eIonBall->v.movetype = MOVETYPE_FLY;
				eIonBall->v.effects = EF_LIGHT_GREEN;
				eIonBall->v.TouchFunction = IonRifle_IonBallTouch;

				eIonBall->Model.scale = 2.0f;

				eIonBall->Physics.solid	= SOLID_BBOX;
				eIonBall->Physics.ignore	= eOwner;

				eIonBall->local.eOwner	= eOwner;
				eIonBall->local.style	= 1;		// [29/1/2014] Preserve our firemode ~hogsy

				Math_VectorCopy(eOwner->v.origin,vOrigin);

				vOrigin[2] += 25.0f;

				Entity_SetModel(eIonBall,"models/ionball.md2");
				Entity_SetSizeVector(eIonBall, pl_origin3f, pl_origin3f);
				Entity_SetOrigin(eIonBall,vOrigin);

				Weapon_Projectile(eOwner, eIonBall, IONRIFLE_MAX_RANGE);

				Entity_Link(eIonBall,false);
			}
		}

		if(eOwner->local.attackb_finished > Server.dTime)
			eOwner->local.dAttackFinished = Server.dTime+0.10;
		else
			eOwner->local.dAttackFinished = Server.dTime+0.5;
		break;
	default:	// Simple bursts
		Sound(eOwner,CHAN_AUTO,"weapons/laser.wav",255,ATTN_NORM);

		switch(eOwner->local.iBarrelCount)
		{
		case 0:
			Weapon_Animate(eOwner,efIonRiflePulseFireA);
			break;
		case 1:
			Weapon_Animate(eOwner,efIonRiflePulseFireB);
			break;
		case 2:
			Weapon_Animate(eOwner,efIonRiflePulseFireC);
			break;
		case 3:
			Weapon_Animate(eOwner,efIonRiflePulseFireD);
			break;
		case 4:
			Weapon_Animate(eOwner,efIonRiflePulseFireE);
		}

		// [25/9/2013] Punch the view back ~hogsy
		eOwner->v.punchangle[0] -= (float)(((rand()%5)+1)/10.0f);

		eOwner->local.ionblaster_ammo--;

		// [29/1/2014] Let us cycle through each barrel on an individual basis ~hogsy
		eOwner->local.iBarrelCount++;
		if(eOwner->local.iBarrelCount >= 4)
			eOwner->local.iBarrelCount = 0;

		{
			eIonBall = Entity_Spawn();
			if(eIonBall)
			{
				plVector3f_t vOrigin;

				eIonBall->v.cClassname		= "ionball";
				eIonBall->v.movetype		= MOVETYPE_FLY;
				eIonBall->v.effects			= EF_LIGHT_GREEN;
				eIonBall->v.TouchFunction	= IonRifle_IonBallTouch;

				eIonBall->Model.scale = 0.3f;

				eIonBall->Physics.solid	= SOLID_BBOX;
				eIonBall->Physics.ignore	= eOwner;

				eIonBall->local.eOwner	= eOwner;
				eIonBall->local.style	= 0;		// [29/1/2014] Preserve our firemode ~hogsy

				plVectorCopy(eOwner->v.origin, vOrigin);

				vOrigin[2] += 25.0f;

				Entity_SetModel(eIonBall,"models/ionball.md2");
				Entity_SetSizeVector(eIonBall, pl_origin3f, pl_origin3f);
				Entity_SetOrigin(eIonBall,vOrigin);

				Weapon_Projectile(eOwner, eIonBall, IONRIFLE_MAX_RANGE);

				Entity_Link(eIonBall,false);
			}
		}

		if(eOwner->local.attackb_finished > Server.dTime)
			eOwner->local.dAttackFinished = Server.dTime+0.5;
		else
			eOwner->local.dAttackFinished = Server.dTime+0.3;
	}

	// [17/11/2013] Update ammo counts... ~hogsy
	eOwner->v.iPrimaryAmmo = eOwner->local.ionblaster_ammo;
}

/*	Switch fire modes.
*/
void IonRifle_SecondaryAttack(ServerEntity_t *eOwner)
{
	if(eOwner->local.iFireMode)
	{
		Weapon_Animate(eOwner,efIonRiflePulseMode);

		eOwner->local.iFireMode			=
		eOwner->local.iWeaponIdleFrame	= IONRIFLE_IDLEA_FRAME;
	}
	else
	{
		Weapon_Animate(eOwner,efIonRifleBlastMode);

		eOwner->local.iFireMode			= 1;
		eOwner->local.iWeaponIdleFrame	= IONRIFLE_IDLEB_FRAME;
	}

	eOwner->local.dAttackFinished = Server.dTime+1.2;
}
