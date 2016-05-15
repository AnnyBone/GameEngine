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

#pragma once

class GameMode
{
	GameMode(const char *_description);
	~GameMode();

	static void CreateBaseMode()
	{
		if (g_gamemode)
			return;

		g_gamemode = new GameMode("base");
		if (!g_gamemode)
			g_engine->Sys_Error("Failed to create mode for game!\n");
	}

	// Server

	virtual void ServerInitialize();
	virtual void ServerShutdown();

	virtual void ServerEntityFrame();
	
	virtual void ServerSpawnEntity();
	virtual void ServerSpawnPlayer();

	// Client

	virtual void ClientInitialize();
	virtual void ClientShutdown();

private:
	const char *description;
};

GameMode *g_gamemode;