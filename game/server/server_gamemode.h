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

#ifndef SERVER_GAMEMODE_H
#define SERVER_GAMEMODE_H

typedef struct
{
	const char *ccName;			// Name of the mode.
	const char *ccDescription;	// The description of the gamemode.

	void(*StartFrame)(void);	// Called at the start of the frame (pre).
	void(*EndFrame)(void);		// Called at the end of the frame (post).

	void(*PlayerSpawn)(ServerEntity_t *sePlayer);	// Called just before spawning a player.
	void(*WorldSpawn)(ServerEntity_t *seWorld);		// Called just before spawning the world.
} ServerGameMode_t;

#endif // !SERVER_GAMEMODE_H