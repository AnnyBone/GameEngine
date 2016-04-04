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

typedef struct
{
	int		length;
	char	cMap[64];
	char	average; //johnfitz
	char	peak; //johnfitz
} lightstyle_t;

typedef struct
{
	char	name[32];
	float	entertime;
	int		frags;
	int		colors;			// two 4 bit fields
} scoreboard_t;

typedef struct
{
	int		destcolor[3];
	int		percent;		// 0-256
} cshift_t;

#define	CSHIFT_CONTENTS	0
#define	CSHIFT_DAMAGE	1
#define	CSHIFT_BONUS	2
#define	CSHIFT_POWERUP	3
#define	NUM_CSHIFTS		4

#define	NAME_LENGTH	64


//
// client_state_t should hold all pieces of the client state
//

#define	SIGNONS		4			// signon messages to receive before connected

#define	MAX_DLIGHTS	1024

#define	MAX_BEAMS	32 //johnfitz -- was 24
typedef struct
{
	unsigned int	entity;
	struct model_s	*model;
	float			endtime;
	plVector3f_t	start, end;
} beam_t;

#define	MAX_EFRAGS		2048 //johnfitz -- was 640
#define	MAX_MAPSTRING	2048
#define	MAX_DEMOS		8
#define	MAX_DEMONAME	16

typedef enum {
	ca_dedicated, 			// a dedicated server with no ability to start a client
	ca_disconnected, 		// full screen console with no connection
	CLIENT_STATE_EDITOR,
	ca_connected			// valid netcon, talking to a server
} cactive_t;

//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
	cactive_t	state;

// personalization data sent to server
	char		mapstring[MAX_QPATH];
	char		spawnparms[MAX_MAPSTRING];	// to restart a level

// demo loop control
	int			demonum;		// -1 = don't play demos
	char		demos[MAX_DEMOS][MAX_DEMONAME];		// when not playing

// demo recording info must be here, because record is started before
// entering a map (and clearing client_state_t)
	bool	demorecording;
	bool	demoplayback;
	bool	timedemo;
	int		forcetrack;			// -1 = use normal cd track
	FILE	*demofile;
	int		td_lastframe;		// to meter out one message a frame
	int		td_startframe;		// host_framecount at start
	float	td_starttime;		// realtime at second frame of timedemo

// connection information
	int			signon;			// 0 to SIGNONS
	struct qsocket_s	*netcon;
	sizebuf_t	message;		// writing buffer to send to server

} client_static_t;

#ifdef __cplusplus
extern "C" {
#endif
	extern client_static_t	cls;
#ifdef __cplusplus
}
#endif

/*	The client_state_t structure is wiped completely at every
	server signon.
*/
typedef struct
{
	int			movemessages;	// since connecting to this server
								// throw out the first couple, so the player
								// doesn't accidentally do something the
								// first frame
	ClientCommand_t	cmd;		// last command sent to the server

	// information for local display
	int				stats[STAT_NONE];	// health, etc
	int				items;					// inventory bit flags
	float			item_gettime[32];		// cl.time of aquiring item, for blinking

	cshift_t	cshifts[NUM_CSHIFTS];	// color shifts for damage, powerups
	cshift_t	prev_cshifts[NUM_CSHIFTS];	// and content types

// the client maintains its own idea of view angles, which are
// sent to the server each frame.  The server sets punchangle when
// the view is temporarliy offset, and an angle reset commands at the start
// of each level and after teleporting.
	plVector3f_t		mviewangles[2];	// during demo playback viewangles is lerped
								// between these
	plVector3f_t		viewangles;
	plVector3f_t		mvelocity[2];	// update by server, used for lean+bob
								// (0 is newest)
	plVector3f_t		velocity;		// lerped between mvelocity[0] and [1]
	plVector3f_t		punchangle;		// temporary offset

// pitch drifting vars
	float		idealpitch;
	float		pitchvel;
	bool		bNoDrift;
	float		driftmove;
	double		laststop;

	float		viewheight;

	bool		bIsPaused,			// send over by server
				bIsOnGround,
				bIsInWater;

	int			intermission;	// don't change view angle, full screen, etc
	int			completed_time;	// latched at intermission start

	double		mtime[2];		// the timestamp of last two messages
	double		time;			// clients view of time, should be between
								// servertime and oldservertime to generate
								// a lerp point for other data
	double		oldtime;		// previous cl.time, time-oldtime is used
								// to decay light values and smooth step ups

	float		last_received_message;	// (realtime) for net trouble icon

	// Information that is static for the entire time connected to a server
	struct model_s		*model_precache[MAX_MODELS];
	char				sound_precache[MAX_SOUNDS][MAX_QPATH];

	char			levelname[128];	// for display on solo scoreboard //johnfitz -- was 40.
	unsigned int	viewentity;		// cl_entitites[cl.viewentity] = player
	int				maxclients;
	int				gametype;

// refresh related state
	struct model_s	*worldmodel;	// cl_entitites[0].model
	struct efrag_s	*free_efrags;
	unsigned int	num_entities;	// held in cl_entities array
	int				num_statics;	// held in cl_staticentities array
	ClientEntity_t	viewent;		// the gun model

	int			cdtrack, looptrack;	// cd audio

// frag scoreboard
	scoreboard_t	*scores;		// [cl.maxclients]

	unsigned	protocol; //johnfitz
} client_state_t;


//
// cvars
//
extern	cvar_t	cl_name;
extern	cvar_t	cl_color;
extern	cvar_t	cl_upspeed;
extern	cvar_t	cl_forwardspeed;
extern	cvar_t	cl_backspeed;
extern	cvar_t	cl_sidespeed;
extern	cvar_t	cl_movespeedkey;
extern	cvar_t	cl_yawspeed;
extern	cvar_t	cl_pitchspeed;
extern	cvar_t	cl_anglespeedkey;
extern	cvar_t	cl_autofire;
extern	cvar_t	cl_shownet;
extern	cvar_t	cl_nolerp;
extern	cvar_t	cl_pitchdriftspeed;
extern	cvar_t	lookspring;
extern	cvar_t	lookstrafe;
extern	cvar_t	sensitivity;
extern	cvar_t	m_pitch;
extern	cvar_t	m_yaw;
extern	cvar_t	m_forward;
extern	cvar_t	m_side;

#define	MAX_TEMP_ENTITIES	256		//johnfitz -- was 64
#define	MAX_STATIC_ENTITIES	512		//johnfitz -- was 128
#define	MAX_VISEDICTS		2048

plEXTERN_C_START

extern	client_state_t	cl;

// FIXME, allocate dynamically
extern	efrag_t			cl_efrags[MAX_EFRAGS];
extern	ClientEntity_t	cl_static_entities[MAX_STATIC_ENTITIES];
extern	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
extern	DynamicLight_t	*cl_dlights;
extern	ClientEntity_t	cl_temp_entities[MAX_TEMP_ENTITIES];
extern	beam_t			cl_beams[MAX_BEAMS];
extern	ClientEntity_t	*cl_visedicts[MAX_VISEDICTS];
extern	unsigned int	cl_numvisedicts;

extern	ClientEntity_t	*cl_entities; //johnfitz -- was a static array, now on hunk
extern	unsigned int	cl_max_edicts; //johnfitz -- only changes when new map loads

//=============================================================================

//
// cl_main
//
DynamicLight_t	*Client_AllocDlight(int key);
void			CL_DecayLights (void);

void CL_Init (void);
void CL_EstablishConnection (char *host);
void CL_Disconnect (void);
void CL_Disconnect_f (void);
void CL_NextDemo (void);

//
// cl_input
//
typedef struct
{
	int		down[2];		// key nums holding it down
	int		state;			// low bit is down state
} kbutton_t;

extern	kbutton_t	in_klook;
extern 	kbutton_t 	in_strafe;
extern 	kbutton_t 	in_speed;

void CL_InitInput (void);
void CL_SendCmd (void);
void CL_SendMove(ClientCommand_t *cmd);
void CL_UpdateTEnts (void);
void CL_ClearState (void);

int  CL_ReadFromServer (void);
void CL_BaseMove(ClientCommand_t *cmd);


float CL_KeyState (kbutton_t *key);
char *Key_KeynumToString (int keynum);

//
// cl_demo.c
//
void CL_StopPlayback (void);
int CL_GetMessage (void);
void CL_Stop_f (void);
void CL_Record_f (void);
void CL_PlayDemo_f (void);
void CL_TimeDemo_f (void);

//
// cl_parse.c
//
void CL_ParseServerMessage (void);

//
// view
//
void V_StartPitchDrift (void);
void V_StopPitchDrift (void);
void V_RenderView (void);
void V_ParseDamage (void);
void V_SetContentsColor (int contents);

//
// cl_tent
//
void CL_SignonReply (void);

plEXTERN_C_END
