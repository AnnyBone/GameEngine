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

#ifndef SHARED_CLIENT_H
#define SHARED_CLIENT_H

/*	Client->Server type
	defines. Currently these
	are all still engine-side
	so it's wise not to touch!
*/
enum
{
	CLC_BAD,								// A bad message which won't be read by the server
	CLC_NOP,
	CLC_DISCONNECT,							// Disconnects from the current server
	CLC_MOVE,								// Lets the server know we're moving
	CLC_STRINGCMD,
/*	Anything beyond this point
	is local.					*/

	CLC_NONE
};

/*	Temporary Entity Types
*/
enum
{
	CTE_EXPLOSION,
	CTE_GUNSHOT,
	CTE_TELEPORT,
	CTE_MUZZLEFLASH,

	// Blood
	CTE_BLOODPUFF,
	CTE_BLOODCLOUD,

	CTE_PARTICLE_FIELD
};

/*	Client stats, shared from server to client.
*/
typedef enum
{
	STAT_HEALTH,
	STAT_FRAGS,
	STAT_WEAPON,
	STAT_AMMO,
	STAT_ARMOR,
	STAT_WEAPONFRAME,
	STAT_ACTIVEWEAPON,
	STAT_TOTALSECRETS,
	STAT_TOTALMONSTERS,
	STAT_SECRETS,		// Bumped on client side by svc_foundsecret.
	STAT_MONSTERS,		// Bumped by svc_killedmonster.

	STAT_NONE
} ClientStat_t;

typedef struct
{
	MathVector3f_t	viewangles;

	// intended velocities
	float	forwardmove;
	float	sidemove;
	float	upmove;
} ClientCommand_t;

typedef struct DynamicLight_s
{
	MathVector3f_t	origin;
	float	radius;
	double	die;			// Stop lighting after this time
	float	decay,			// Drop this each second
			minlight;		// Don't add when contributing less
	int		key;
	MathVector3f_t	color;			//johnfitz -- lit support via lordhavoc
	MathVector3f_t	transformed;

	bool	lightmap;		// Defines whether this light effects the lightmap.
} DynamicLight_t;

typedef void (*xcommand_t)(void);

typedef struct ConsoleVariable_s
{
	const char	*name;				// Name of the variable
	char		*string;			// Default value

	bool bArchive;	// Variable will be archived
	bool	    server;
	const char *cDescription;
	float		value;
	struct		ConsoleVariable_s *next;
	const char	*default_string;
	void		(*callback)(void);

	// Different variable types.
	bool bValue;
	int	iValue;
} ConsoleVariable_t;

// Future replacement for cvar, to follow every other naming convention.
#define cvar_t ConsoleVariable_t

#define XENON_CONFIG_EXTENSION	".config"

typedef struct
{
	MathVector3f_t origin,
					angles;
	unsigned short 	modelindex,
					frame;
	unsigned char 	colormap,
					skin,
					alpha;
	int				effects;

	float fScale;
} EntityState_t;

typedef struct ClientEntity_s
{
	bool			bForceLink;		// Model changed

	int				update_type;
	EntityState_t	baseline;		// to fill in defaults in updates
	double			msgtime;		// time of last update

	plVector3f_t msg_origins[2];	// last two updates (0 is newest)
	plVector3f_t origin;
	plVector3f_t msg_angles[2];		// last two updates (0 is newest)
	plVector3f_t angles;

	plVector3f_t velocity;		// Velocity of the entity (roughly).

	int				draw_lastpose,
					draw_pose;
	float			draw_lerpstart;
	struct model_s	*draw_lastmodel;
	struct model_s	*model;			// NULL = no model
	struct efrag_s	*efrag;			// linked list of efrags
	int				frame;
	uint8_t			*colormap;          // Obsolete
	int				effects;		// light, particles, etc
	int				skinnum;		// for Alias models
	int				visframe;		// last frame this entity was
											//  found in an active leaf
	int				dlightframe;	// dynamic lighting
	int				dlightbits;

	struct mnode_s	*topnode;		// for bmodels, first world node
									//  that splits bmodel, or NULL if
									//  not split

	uint8_t			alpha;			//johnfitz -- alpha
	uint8_t			lerpflags;		//johnfitz -- lerping
	float			lerpstart;		//johnfitz -- animation lerping
	float			lerpfinish;		//johnfitz -- lerping -- server sent us a more accurate interval, use it instead of 0.1
	short			previouspose;	//johnfitz -- animation lerping
	short			currentpose;	//johnfitz -- animation lerping
	float			movelerpstart;	//johnfitz -- transform lerping

	// LOD
	float distance_alpha;

	plVector3f_t previousorigin;	//johnfitz -- transform lerping
	plVector3f_t currentorigin;	//johnfitz -- transform lerping
	plVector3f_t previousangles;	//johnfitz -- transform lerping
	plVector3f_t currentangles;	//johnfitz -- transform lerping

	float	scale;	// Sets the model scale.
} ClientEntity_t;

#define entity_t ClientEntity_t

//////////////////////////////////////////////////////////////////////////
// PARTICLES															//
//////////////////////////////////////////////////////////////////////////
typedef enum
{
	PARTICLE_BEHAVIOUR_STATIC,			// Sprite
	PARTICLE_BEHAVIOUR_GRAVITY,			// Gravity
	PARTICLE_BEHAVIOUR_SLOWGRAVITY,		// Slow Gravity
	PARTICLE_BEHAVIOUR_FIRE,			// Fire
	PARTICLE_BEHAVIOUR_EXPLODE,			// Explosion
	PARTICLE_BLOB,
	PARTICLE_BLOB2,
	PARTICLE_BEHAVIOUR_SMOKE			// Smoke
} ParticleBehaviour_t;

typedef enum
{
	PARTICLE_TYPE_DEFAULT,
	PARTICLE_TYPE_LAVASPLASH,
	PARTICLE_TYPE_TELEPORTSPLASH
} ParticleType_t;

#include "shared_material.h"

// TODO: Update these to new coding standard; e.g. lowercase names
typedef struct particle_s
{
	MathVector3f_t		vOrigin,		// The origin of the particle.
						vVelocity;		// Velocity of the particle.

	PLColourf		vColour;		// RGBA colour of the particle.

	int	iFrame;							// Frame of animation.

	Material_t *material;
	int	skin;

	ParticleBehaviour_t		pBehaviour;	// Type of behaviour.

	struct		particle_s	*next;

	double	lifetime;

	float					fRamp,
							fScale;		// Size/scale of the particle.
} Particle_t;
//////////////////////////////////////////////////////////////////////////

#endif // !SHARED_CLIENT_H
