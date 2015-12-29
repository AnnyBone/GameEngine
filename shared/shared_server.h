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

#ifndef SHARED_SERVER_H
#define SHARED_SERVER_H

#define	SERVER_GRAVITY	600.0f

enum ServerMessage
{
	/*	TODO: Reduce all this, and then shift stuff over
		into game module.
	*/
	SVC_NOP = 1,							// Used by the engine to check if a client is still connected(?)
	SVC_DISCONNECT,							// Disconnects the current server
	SVC_UPDATESTAT,							// Updates the specified client statistic
	SVC_VERSION,							// Sends the server version to the engine for checking
	SVC_SETVIEW,							// TODO: Sets the entity to view from?
	SVC_SOUND,
	SVC_TIME,								// TODO: Sets the time? Client time? What!?
	SVC_PRINT,								// Prints a message to the console
	SVC_STUFFTEXT,							// Adds text to the console buffer
	SVC_SETANGLE,							// Sets the view angle
	SVC_SERVERINFO,							// TODO: Used to print/get information about the current server?
	SVC_LIGHTSTYLE,							// Used for lightstyles
	SVC_UPDATENAME,							// TODO: Updates the clients name in the scoreboard?
	SVC_UPDATEFRAGS,						// Clearly updates the clients frag count
	SVC_CLIENTDATA,
	SVC_STOPSOUND,							// Stops the specified sound
	SVC_UPDATECOLORS,						// Updates with player colours I assume
	SVC_PARTICLE,							// Used to create particles
	SVC_DAMAGE,								// TODO: Tells the server we've been damaged?
	SVC_SPAWNSTATIC,
	SVC_SPAWNBASELINE = 22,
	SVC_TEMPENTITY,							// Spawns a temporary entity
	SVC_SETPAUSE,							// Pauses the game
	SVC_SIGNONNUM,
	SVC_CENTERPRINT,						// Allows us to send a centered message to either everyone or just a specific client
	SVC_KILLEDMONSTER,						// Updates the clients statistic count for kills
	SVC_FOUNDSECRET,						// Updates the clients statistic count for secrets
	SVC_SPAWNSTATICSOUND,
	SVC_INTERMISSION,
	SVC_FINALE,
	SVC_CDTRACK,
	SVC_CUTSCENE		= 34,				// Prepares the client for an in-game cutscene
	SVC_SKYBOX			= 37,				// Sets the skybox for the current level
	SVC_BF				= 40,				// Executes the bf command
	SVC_FOG,								// Sets properties for the fog in the current level
	SVC_SPAWNBASELINE2,
	SVC_SPAWNSTATIC2,
	SVC_SPAWNSTATICSOUND2,
	SVC_UPDATEMENU,
	//SVC_SPRITE,
/*	Anything beyond this point
	is local.					*/
	MESSAGE_SERVER_DEBUG,

	SVC_NONE
};

#define	SERVER_PROTOCOL	(1+(SVC_NONE))

#endif // !SHARED_SERVER_H
