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

#ifndef	__SERVERWEAPON_H__
#define	__SERVERWEAPON_H__

#include "server_main.h"

/*	Ammo types.
*/
typedef enum
{
	AM_BULLET,
	AM_SWITCH,
	AM_MELEE,
	AM_ROCKET,
	AM_PROJECTILE,				// General projectile-based ammo.
#ifdef GAME_OPENKATANA
/*	Episode one					*/
	AM_IONS,					// ION ammunition.
	AM_SHELL,					// SHOTCYCLER ammunition.
	AM_LASER,					// Shockwave ammunition.
	AM_C4BOMB,					// C4 ammunition.
/*	Episode two					*/
/*	Episode three				*/
/*	Episode four				*/
#endif

	AM_NONE
} WeaponAmmo_t;

#ifdef GAME_OPENKATANA
void IonBlaster_Deploy(ServerEntity_t *ent);
void Shockwave_Deploy(ServerEntity_t *ent);
void SideWinder_Deploy(ServerEntity_t *ent);
void Shotcycler_Deploy(ServerEntity_t *ent);
void C4Vizatergo_Deploy(ServerEntity_t *ent);
void Daikatana_Deploy(ServerEntity_t *ent);
void Axe_Deploy(ServerEntity_t *ent);
void Ballista_Deploy(ServerEntity_t *ent);
void Barrier_Deploy(ServerEntity_t *ent);
void Crossbow_Deploy(ServerEntity_t *ent);
void Discus_Deploy(ServerEntity_t *ent);
void Glock_Deploy(ServerEntity_t *ent);
void GreekFire_Deploy(ServerEntity_t *ent);
void Hermes_Deploy(ServerEntity_t *ent);
void Kineticore_Deploy(ServerEntity_t *ent);
void Midas_Deploy(ServerEntity_t *ent);
void PulseRifle_Deploy(ServerEntity_t *ent);
void Zeus_Deploy(ServerEntity_t *ent);
void IonRifle_Deploy(ServerEntity_t *eOwner);

void Axe_PrimaryAttack(ServerEntity_t *ent);
void Ballista_PrimaryAttack(ServerEntity_t *ent);
void Barrier_PrimaryAttack(ServerEntity_t *ent);
void C4Vizatergo_PrimaryAttack(ServerEntity_t *eOwner);
void C4Vizatergo_SecondaryAttack(ServerEntity_t *eOwner);
void Crossbow_PrimaryAttack(ServerEntity_t *ent);
void Daikatana_PrimaryAttack(ServerEntity_t *ent);
void Discus_PrimaryAttack(ServerEntity_t *ent);
void Glock_PrimaryAttack(ServerEntity_t *ent);
void GreekFire_PrimaryAttack(ServerEntity_t *ent);
void Hermes_PrimaryAttack(ServerEntity_t *ent);
void IonBlaster_PrimaryAttack(ServerEntity_t *ent);
void Kineticore_PrimaryAttack(ServerEntity_t *ent);
void Midas_PrimaryAttack(ServerEntity_t *ent);
void PulseRifle_PrimaryAttack(ServerEntity_t *ent);
void PulseRifle_SecondaryAttack(ServerEntity_t *ent);
void Shockwave_PrimaryAttack(ServerEntity_t *ent);
void Shotcycler_PrimaryAttack(ServerEntity_t *ent);
void SideWinder_PrimaryAttack(ServerEntity_t *eOwner);
void Zeus_PrimaryAttack(ServerEntity_t *ent);
void IonRifle_PrimaryAttack(ServerEntity_t *eOwner);
void IonRifle_SecondaryAttack(ServerEntity_t *eOwner);
#elif GAME_ADAMAS
void Blazer_Deploy(ServerEntity_t *eOwner);
void Blazer_PrimaryAttack(ServerEntity_t *eOwner);
#endif

typedef struct
{
	int				iItem;

	char			*model;

	void			(*Deploy)(ServerEntity_t *ent);

	WeaponAmmo_t	iPrimaryType;

	void			(*Primary)(ServerEntity_t *ent);

	WeaponAmmo_t	iSecondaryType;

	void			(*Secondary)(ServerEntity_t *ent);

	// For handing some control over to the weapon system in regards to updating ammo.
	int				iPrimaryAmmoOffset,
					iSecondaryAmmoOffset;
} Weapon_t;

MathVector_t Weapon_Aim(ServerEntity_t *eEntity);

Weapon_t *Weapon_GetCurrentWeapon(ServerEntity_t *eEntity);
Weapon_t *Weapon_GetWeapon(int iWeaponID);

void Weapon_Precache(void);
void Weapon_UpdateCurrentAmmo(Weapon_t *wWeapon, ServerEntity_t *eEntity);
void Weapon_SetActive(Weapon_t *wWeapon, ServerEntity_t *eEntity, bool bDeploy);
void Weapon_BulletProjectile(ServerEntity_t *ent, float spread, int damage, MathVector3f_t vVector);
void Weapon_Projectile(ServerEntity_t *eOwner, ServerEntity_t *eProjectile, float fVelocity);
void Weapon_CheckFrames(ServerEntity_t *eEntity);
void Weapon_Animate(ServerEntity_t *ent, EntityFrame_t *eFrames);
void Weapon_PrimaryAttack(ServerEntity_t *eEntity);
void Weapon_CheckInput(ServerEntity_t *eEntity);
void Weapon_ViewPunch(ServerEntity_t *eEntity, float fIntensity, bool bAddition);

bool Weapon_CheckPrimaryAmmo(Weapon_t *wWeapon, ServerEntity_t *eEntity);
bool Weapon_CheckSecondaryAmmo(Weapon_t *wWeapon, ServerEntity_t *eEntity);
bool Weapon_CheckTrace(ServerEntity_t *eOwner);

#endif
