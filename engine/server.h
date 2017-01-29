
typedef enum
{
	SERVER_STATE_LOADING,
	SERVER_STATE_ACTIVE
} ServerState_t;

typedef struct
{
	bool	active;				// FALSE if only a net client
	bool	paused;
	bool	loadgame;			// handle connections specially

	double		time;

	int			lastcheck;			// used by PF_checkclient
	double		lastchecktime;

	char			name[64];			// map name
	char			modelname[64];		// maps/<name>.bsp, for model_precache[0]
	struct model_s 	*worldmodel;
	char			*model_precache[MAX_MODELS];	// NULL terminated
	struct model_s	*models[MAX_MODELS];
	char			*sound_precache[MAX_SOUNDS];	// NULL terminated
	char			*material_precache[SERVER_MAX_TEXTURES];
	char			*lightstyles[MAX_LIGHTSTYLES];
	unsigned int	num_edicts;
	unsigned int	max_edicts;
	ServerEntity_t	*edicts;		// can NOT be array indexed, because
									// ServerEntity_t is variable sized, but can
									// be used to reference the world ent
	ServerState_t	state;			// some actions are only valid during load

	sizebuf_t	datagram;
	uint8_t		datagram_buf[MAX_DATAGRAM];

	sizebuf_t	reliable_datagram;	// copied to all clients at end of frame
	uint8_t		reliable_datagram_buf[MAX_DATAGRAM];

	sizebuf_t	signon;
	uint8_t		signon_buf[MAX_MSGLEN - 2]; //johnfitz -- was 8192, now uses MAX_MSGLEN

	unsigned	protocol; //johnfitz
} Server_t;

PL_EXTERN_C

extern	ConsoleVariable_t	teamplay;
extern	ConsoleVariable_t	skill;
extern	ConsoleVariable_t	deathmatch;
extern	ConsoleVariable_t	coop;

extern	ServerStatic_t	svs;				// persistant server info
extern	Server_t		sv;					// local server

extern	ServerClient_t	*host_client;

extern	jmp_buf 	host_abortserver;

extern	ServerEntity_t		*sv_player;

PL_EXTERN_C_END

//===========================================================

void SV_Init (void);
void SV_StartSound (ServerEntity_t *entity, int channel, char *sample, int volume,float attenuation);
void SV_DropClient(bool crash);
void SV_SendClientMessages (void);
void SV_ClearDatagram (void);

int SV_ModelIndex (char *name);

void SV_SetIdealPitch (void);
void SV_AddUpdates (void);
void SV_ClientThink (void);
void SV_AddClientToServer (struct qsocket_s	*ret);
void SV_ClientPrintf (const char *fmt, ...);
void SV_BroadcastPrintf (char *fmt, ...);

void SV_WriteClientdataToMessage (ServerEntity_t *ent, sizebuf_t *msg);
void SV_CheckForNewClients (void);
void SV_RunClients (void);
void SV_SaveSpawnparms ();
void SV_SpawnServer (char *server);
