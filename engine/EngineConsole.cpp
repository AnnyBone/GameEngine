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

#include <algorithm>
#include <ctype.h>
#include <fcntl.h>
#include <list>
#include <set>
#include <time.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "engine_base.h"

#include "EngineInput.h"
#include "video.h"

static const unsigned int CHAR_WIDTH     = 8;
static const unsigned int CHAR_HEIGHT    = 8;
static const unsigned int CONS_BACKLOG   = 200;
static const unsigned int CONS_MAXNOTIFY = 10;
static const unsigned int CONS_HISTORY   = 32;
static const char        *CONS_PS1       = "]";

float con_cursorspeed = 4;

bool con_forcedup;		// because no entities to refresh

cvar_t		con_notifytime = {"con_notifytime","3"};		//seconds
cvar_t		con_logcenterprint = {"con_logcenterprint", "1"}; //johnfitz

char		con_lastcenterstring[1024]; //johnfitz

bool g_consoleinitialized;

/* Singleton console instance. */
Core::Console *con_instance = NULL;

Core::Console::Console():
	cursor_x(0),
	cursor_y(0),
	backscroll(0),
	input_hiter(input_history.end()),
	input_cursor_pos(0),
	input_draw_off(0)
{
	lines.emplace_back("");
}

void Core::Console::Clear()
{
	cursor_x = 0;
	cursor_y = 0;

	lines.clear();
	lines.emplace_back("");
}

void Core::Console::ClearNotify()
{
	for(auto l = lines.begin(); l != lines.end(); ++l)
	{
		l->time = 0;
	}
}

void Core::Console::Print(const char *text)
{
	/* A string starting with 0x02 is "coloured" and the characters are
	 * ORd with 128 so the rendering code highlights them.
	*/
	unsigned char cbit = 0;
	if(*text == 0x02)
	{
		cbit = 128;
		++text;
	}

	while(*text)
	{
		if(*text == '\n')
		{
			linefeed();
			cursor_x = 0;
		}
		else if(*text == '\r')
		{
			cursor_x = 0;
		}
		else{
			if(lines[cursor_y].text.size() > cursor_x)
			{
				lines[cursor_y].text[cursor_x] = (*text | cbit);
			}
			else{
				lines[cursor_y].text += (*text | cbit);
			}

			lines[cursor_y].time = time(NULL);

			++cursor_x;
		}

		++text;
	}
}

void Core::Console::ScrollUp()
{
	++backscroll;
}

void Core::Console::ScrollDown()
{
	if(backscroll > 0)
		--backscroll;
}

void Core::Console::ScrollHome()
{
	backscroll = CONS_BACKLOG;
}

void Core::Console::ScrollEnd()
{
	backscroll = 0;
}

void Core::Console::InputKey(int key, bool ctrl, bool shift)
{
	if(key == INPUT_KEY_TAB)
	{
		TabComplete(shift);
		return;
	}
	else{
		/* Reset the tab completion state machine */
		tab_suggestions.clear();
	}

	// TODO: Move clipboard stuff into platform lib
#ifdef _WIN32
	if(tolower(key) == 'v' && ctrl)
	{
		/* Paste... */

		if(OpenClipboard(NULL))
		{
			HANDLE th = GetClipboardData(CF_TEXT);
			if(th)
			{
				const char *clipText = (const char*)GlobalLock(th);
				if(clipText)
				{
					/* Insert text from the clipboard up to
					 * the first newline, carridge return or
					 * backspace character.
					*/

					size_t len = strcspn(clipText, "\n\r\b");

					input_line.insert(input_cursor_pos, clipText, len);
					input_cursor_pos += len;
				}

				GlobalUnlock(th);
			}

			CloseClipboard();
		}
	}
#endif

	switch(key)
	{
		case INPUT_KEY_ENTER:
		case KP_ENTER:
		{
			std::string line = input_line;
			InputClear();

			input_history.push_back(line);
			if(input_history.size() > CONS_HISTORY)
			{
				input_history.pop_front();
			}

			Cbuf_AddText(line.c_str());
			Cbuf_AddText("\n");

			break;
		}

		case K_BACKSPACE:
		{
			if(input_cursor_pos > 0)
				input_line.erase(--input_cursor_pos, 1);

			break;
		}

		case K_DEL:
		{
			if(input_line.length() > input_cursor_pos)
				input_line.erase(input_cursor_pos, 1);

			break;
		}

		case K_HOME:
		{
			if(ctrl)
				ScrollHome();
			else
				input_cursor_pos = 0;

			break;
		}

		case K_END:
		{
			if(ctrl)
				ScrollEnd();
			else
				input_cursor_pos = input_line.length();

			break;
		}

		case K_PGUP:
		{
			ScrollUp();
			break;
		}

		case K_PGDN:
		{
			ScrollDown();
			break;
		}

		case K_LEFTARROW:
		{
			if(input_cursor_pos > 0)
				--input_cursor_pos;

			break;
		}

		case K_RIGHTARROW:
		{
			if(input_cursor_pos < input_line.length())
				++input_cursor_pos;

			break;
		}

		case K_UPARROW:
		{
			if(input_hiter != input_history.begin())
			{
				--input_hiter;
				input_line       = *input_hiter;
				input_cursor_pos = input_line.length();
			}

			break;
		}

		case K_DOWNARROW:
		{
			if(input_hiter == input_history.end()
				|| ++input_hiter == input_history.end())
			{
				/* Pressed down on or beyond the last element
				 * in the history, just clear the input.
				*/

				input_line.clear();
				input_cursor_pos = 0;
			}
			else{
				input_line       = *input_hiter;
				input_cursor_pos = input_line.length();
			}

			break;
		}

		default:
		{
			if(isprint(key) && !ctrl)
			{
				input_line.insert(input_cursor_pos, 1, key);
				++input_cursor_pos;
			}

			break;
		}
	}
}

void Core::Console::InputClear()
{
	input_line.clear();
	input_cursor_pos  = 0;
	input_draw_off = 0;

	input_hiter = input_history.end();
}

void Core::Console::linefeed()
{
	lines.emplace_back("");

	if(lines.size() > CONS_BACKLOG)
		lines.pop_front();
	else
		++cursor_y;
}

std::list<std::string> Core::Console::prepare_text(unsigned int cols, unsigned int rows)
{
	std::list<std::string> wrapped_lines;

	/* Start working back from the end of the deque of lines in the buffer
	 * until we have enough to fill the visible rows + scrollback
	*/
	for(auto l = lines.rbegin();
		l != lines.rend() && (backscroll == (size_t)(-1) || wrapped_lines.size() < rows + backscroll);
		++l)
	{
		auto wrapped_line = wrap_line(l->text, cols);
		wrapped_lines.insert(wrapped_lines.begin(), wrapped_line.begin(), wrapped_line.end());
	}

	/* Clamp the backscroll value to the maximum value for the current
	 * console size and contents. Doing it here is a little ick, but I can't
	 * think of a better way to do it without introducing more bits of state
	 * right now.
	*/
	if(lines.size() < rows)
		backscroll = 0;
	else
		backscroll = std::min(backscroll, (lines.size() - rows));

	/* Chop off the lines currently scrolled back past. */
	for(unsigned int i = 0; i < backscroll && wrapped_lines.size() > rows; ++i)
	{
		wrapped_lines.pop_back();
	}

	while(wrapped_lines.size() > rows)
	{
		wrapped_lines.pop_front();
	}

	return wrapped_lines;
}

std::list<std::string> Core::Console::wrap_line(std::string line, unsigned int cols)
{
	std::list<std::string> wrapped_line;

	do 
	{
		size_t frag_len;

		if(line.length() <= cols)
			frag_len = line.length();
		else
		{
			size_t break_space = line.find_last_of(' ', cols);

			if(break_space == std::string::npos || break_space == 0)
				frag_len = cols;
			else
				frag_len = break_space;
		}

		wrapped_line.emplace_back(line, 0, frag_len);
		line.erase(0, frag_len);
	} while(!line.empty());

	return wrapped_line;
}

void Core::Console::Draw(bool draw_input)
{
	const unsigned int con_cols   = vid.conwidth / CHAR_WIDTH;
	const unsigned int line_width = con_cols - 3;

	// Draw the background
	Draw_ConsoleBackground();

	GL_SetCanvas(CANVAS_CONSOLE);

	// Starting from the bottom...
	int y = vid.conheight - CHAR_HEIGHT;

	// ...draw version number in bottom right...
	{
		char ver[64];
		snprintf(ver, sizeof(ver), "Katana (%i)", (int)(ENGINE_VERSION_BUILD));

		size_t vlen = strlen(ver);
		size_t x    = con_cols - vlen;

		for(size_t i = 0; i < vlen; ++i, ++x)
		{
			Draw_Character(x * CHAR_WIDTH, y, ver[i]);
		}

		y -= CHAR_HEIGHT;
	}

	// ...draw input line...
	if(draw_input && !(key_dest != key_console && !con_forcedup))
	{
		const char *text = input_line.c_str();

		// Clamp input_draw_off
		
		input_draw_off = std::min(input_draw_off, input_cursor_pos - !!input_cursor_pos);
		
		if(input_cursor_pos >= (line_width - strlen(CONS_PS1)))
		{
			input_draw_off = std::max(input_draw_off, input_cursor_pos - (line_width - strlen(CONS_PS1)));
		}

		int x = CHAR_WIDTH;

		// Draw prompt
		for(const char *p = CONS_PS1; *p != '\0'; ++p)
		{
			Draw_Character(x, y, *p);
			x += CHAR_WIDTH;
		}

		// Draw the blinky cursor
		if(!((int)((realtime-key_blinktime)*con_cursorspeed) & 1))
			Draw_Character(x + ((input_cursor_pos - input_draw_off) * CHAR_WIDTH), y + 2, '_');

		// Draw input string
		for(size_t i = input_draw_off; i < input_line.length(); ++i)
		{
			Draw_Character(x, y, input_line[i]);
			x += CHAR_WIDTH;
		}

		y -= CHAR_HEIGHT;
	}

	// ...draw visible console output in remaining space...
	{
		const unsigned int rows = (y / CHAR_HEIGHT) + 1;

		std::list<std::string> wrapped_lines = prepare_text(line_width, rows);

		for(auto line = wrapped_lines.rbegin(); line != wrapped_lines.rend() && y >= 0;)
		{
			for(size_t i = 0; i < line->length(); ++i)
			{
				Draw_Character((i+1) * CHAR_WIDTH, y, line->at(i));
			}

			y -= CHAR_HEIGHT;
			++line;
		}
	}
}

void Core::Console::DrawNotify()
{
	const unsigned int cols = (vid.conwidth / CHAR_WIDTH) - 2;

	GL_SetCanvas(CANVAS_CONSOLE);

	time_t now = time(NULL);

	std::list<std::string> wrapped_lines;

	for(auto l = lines.rbegin(); l != lines.rend() && wrapped_lines.size() < CONS_MAXNOTIFY; ++l)
	{
		if(l->time != 0 && (l->time + con_notifytime.iValue) < now)
		{
			/* Message has expired, implies all the ones above it
			 * are also expired, so break here to save on a load of
			 * pointless work.
			*/
			break;
		}
		else if(l->time == 0)
		{
			/* Message time uninitialised or cleared, skip */
			continue;
		}

		auto wrapped_line = wrap_line(l->text, cols);
		wrapped_lines.insert(wrapped_lines.begin(), wrapped_line.begin(), wrapped_line.end());
	}

	unsigned int y = vid.conheight;

	for(auto l = wrapped_lines.begin(); l != wrapped_lines.end(); ++l)
	{
		for(size_t i = 0; i < l->size(); ++i)
		{
			Draw_Character(((i + 1) * CHAR_WIDTH), y, (*l)[i]);
		}

		y += CHAR_HEIGHT;

		scr_tileclear_updates = 0; // ???
	}

	if (key_dest == key_message)
	{
		const char *say_prompt = team_message ? "Say (team):" : "Say (all):";
		size_t plen      = strlen(say_prompt);

		//johnfitz -- distinguish say and say_team
		if (team_message)
			say_prompt = "Say (team):";
		else
			say_prompt = "Say (all):";
		//johnfitz

		Draw_String(CHAR_WIDTH, y, say_prompt); //johnfitz

		size_t x = 0;
		while(chat_buffer[x])
		{
			Draw_Character((x + plen + 2) * CHAR_WIDTH, y, chat_buffer[x]);
			++x;
		}

		Draw_Character((x + plen + 2) * CHAR_WIDTH, y, 10 + ((int)(realtime*con_cursorspeed)&1));
		y += CHAR_HEIGHT;

		scr_tileclear_updates = 0; //johnfitz
	}
}

extern "C" void M_Menu_Main_f (void);

/* Returns a bar of the desired length, but never wider than the console. */
char *Con_Quakebar (unsigned int len)
{
	static char bar[42];

	const unsigned int con_linewidth = (vid.conwidth / CHAR_WIDTH) - 3;

	len = Math_Min(len, sizeof(bar) - 2);
	len = Math_Min(len, con_linewidth);

	bar[0] = '\35';
	memset(bar + 1, '\36', len - 2);
	bar[len-1] = '\37';

	if (len < con_linewidth)
	{
		bar[len] = '\n';
		bar[len+1] = 0;
	}
	else
		bar[len] = 0;

	return bar;
}

extern "C" int history_line; //johnfitz

void Con_ToggleConsole_f (void)
{
	if (key_dest == key_console)
	{
		if(cls.state == ca_connected)
		{
			key_dest = key_game;
			con_instance->InputClear(); // clear any typing

			Input_ActivateMouse();
		}
		else
			M_Menu_Main_f ();
	}
	else
	{
		key_dest = key_console;

		Input_DeactivateMouse();
	}

	SCR_EndLoadingPlaque();
}

void Con_Clear_f (void)
{
	if(con_instance)
	{
		con_instance->Clear();
	}
}

void Con_ClearNotify (void)
{
	if(con_instance)
	{
		con_instance->ClearNotify();
	}
}

void Con_MessageMode_f (void)
{
	key_dest = key_message;
	team_message = false;
}

void Con_MessageMode2_f (void)
{
	key_dest = key_message;
	team_message = true;
}

void Console_Initialize(void)
{
	if (g_consoleinitialized)
		return;

	plClearLog(ENGINE_LOG);

	con_instance = new Core::Console();

	// register our commands
	Cvar_RegisterVariable (&con_notifytime, NULL);
	Cvar_RegisterVariable (&con_logcenterprint, NULL);

	Cmd_AddCommand("toggleconsole",Con_ToggleConsole_f);
	Cmd_AddCommand("messagemode",Con_MessageMode_f);
	Cmd_AddCommand("messagemode2",Con_MessageMode2_f);
	Cmd_AddCommand("clear",Con_Clear_f);

	g_consoleinitialized = true;
}

/*	Handles cursor positioning, line wrapping, etc
	All console printing must go through this in order to be logged to disk
	If no console is visible, the notify window will pop up.
*/
void Con_Print (char *txt)
{
	if(con_instance)
	{
		con_instance->Print(txt);
	}
}

#define	MAXPRINTMSG	4096

/*	Handles cursor positioning, line wrapping, etc
*/

void Con_Printf (const char *fmt, ...)
{
	va_list			argptr;
	char			msg[MAXPRINTMSG];

	va_start(argptr,fmt);
	vsprintf(msg,fmt,argptr);
	va_end(argptr);

	// Also echo to debugging console.
	Sys_Printf ("%s", msg);

	plWriteLog(ENGINE_LOG, "%s", msg);

	if (g_state.embedded)
		g_launcher.PrintMessage(msg);
	else
	{
		Con_Print(msg);
	}
}

void Con_Warning (const char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	vsprintf (msg,fmt,argptr);
	va_end (argptr);

	if (g_state.embedded)
	{
		plWriteLog(ENGINE_LOG, "%s", msg);
		g_launcher.PrintWarning(msg);
	}
	else
	{
		Con_SafePrintf("\x02Warning: ");
		Con_Printf("%s", msg);
	}
}

void Con_Error(char *fmt,...)
{
	static	bool	bInError = false;
	va_list			argptr;
	char 			msg[MAXPRINTMSG];

	if(bInError)
		Sys_Error("Con_Error: Recursively entered\nCheck log for details.\n");
	bInError = true;

	va_start(argptr,fmt);
	vsprintf(msg,fmt,argptr);
	va_end(argptr);

	if (g_state.embedded)
	{
		plWriteLog(ENGINE_LOG, "%s", msg);
		g_launcher.PrintError(msg);
	}
	else
		Con_Printf("\nError: %s\n",msg);

	CL_Disconnect_f();

	bInError = false;

	longjmp(host_abortserver,1);
	/*Todo:
	 - Clear everything out*/
}

void Con_DPrintf(const char *fmt,...)
{
	va_list	argptr;
	char	msg[MAXPRINTMSG];

	if (!developer.bValue)
		return;

	va_start(argptr,fmt);
	vsprintf(msg,fmt,argptr);
	va_end(argptr);

	if (g_state.embedded)
	{
		plWriteLog(ENGINE_LOG, "%s", msg);
		g_launcher.PrintMessage(msg);
	}
	else
	{
		plWriteLog(ENGINE_LOG, "%s", msg);
		Con_SafePrintf("%s", msg); //johnfitz -- was Con_Printf
	}
}

/*	Okay to call even when the screen can't be updated
*/
void Con_SafePrintf (const char *fmt, ...)
{
	va_list		argptr;
	char		msg[1024];
	int			temp;

	va_start(argptr,fmt);
	vsprintf(msg,fmt,argptr);
	va_end(argptr);

	temp = scr_disabled_for_loading;
	scr_disabled_for_loading = true;
	Con_Printf ("%s", msg);
	scr_disabled_for_loading = temp;
}

void Con_CenterPrintf (unsigned int linewidth, char *fmt, ...)
{
	va_list			argptr;
	char			msg[MAXPRINTMSG]; //the original message
	char			line[MAXPRINTMSG]; //one line from the message
	char			spaces[21]; //buffer for spaces
	char			*src, *dst;
	unsigned int	len, s;

	va_start (argptr,fmt);
	vsprintf (msg,fmt,argptr);
	va_end (argptr);

	linewidth = std::min(linewidth, (vid.conwidth / CHAR_WIDTH) - 3);
	for (src = msg; *src; )
	{
		dst = line;
		while (*src && *src != '\n')
			*dst++ = *src++;
		*dst = 0;
		if (*src == '\n')
			src++;

		len = strlen(line);
		if (len < linewidth)
		{
			s = (linewidth-len)/2;
			memset (spaces, ' ', s);
			spaces[s] = 0;
			Con_Printf ("%s%s\n", spaces, line);
		}
		else
			Con_Printf ("%s\n", line);
	}
}

void Con_LogCenterPrint (char *str)
{
	if(!strcmp(str,con_lastcenterstring))
		return; // Ignore duplicates

	if(cl.gametype == GAME_DEATHMATCH && con_logcenterprint.value != 2)
		return; // Don't log in deathmatch

	strcpy(con_lastcenterstring, str);

	if(con_logcenterprint.value)
	{
		Con_Printf(Con_Quakebar(40));
		Con_CenterPrintf(40,"%s\n",str);
		Con_Printf(Con_Quakebar(40));
		Con_ClearNotify();
	}
}

void Console_ErrorMessage(bool bCrash, const char *ccFile, const char *reason)
{
	if (bCrash)
		Sys_Error("Failed to load %s\nReason: %s", ccFile, reason);
	else
		Con_Error("Failed to load %s\nReason: %s", ccFile, reason);
}

/*
==============================================================================

	TAB COMPLETION

==============================================================================
*/

struct tab_suggestion
{
	std::string name;
	std::string type;

	tab_suggestion(const std::string &n, const std::string &t):
		name(n), type(t) {}

	bool operator<(const tab_suggestion &rhs) const
	{
		return name < rhs.name;
	}
};

std::set<tab_suggestion> BuildTabList(const char *partial)
{
	std::set<tab_suggestion> suggestions;

	size_t len = strlen(partial);

	for(ConsoleVariable_t *cvar = cConsoleVariables; cvar; cvar = cvar->next)
		if (!strncmp(partial, cvar->name, len))
			suggestions.insert(tab_suggestion(cvar->name, "cvar"));

	for(cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next)
		if (!strncmp(partial,cmd->name, len))
			suggestions.insert(tab_suggestion(cmd->name, "command"));

	for(cmdalias_t *alias = cmd_alias; alias; alias = alias->next)
		if (!strncmp(partial, alias->name, len))
			suggestions.insert(tab_suggestion(alias->name, "alias"));

	return suggestions;
}

void Core::Console::TabComplete(bool reverse)
{
	// if editline is empty, return
	if(input_line.empty())
		return;

	if(tab_suggestions.empty())
	{
		/* Find the start of the current word */
		size_t partial_start = input_line.find_last_of(" \";", input_cursor_pos);
		if(partial_start == std::string::npos)
			partial_start = 0;
		else if(partial_start < input_cursor_pos)
			++partial_start;

		std::string partial = input_line.substr(partial_start, input_cursor_pos - partial_start);

		//if partial is empty, return
		if(partial.empty())
			return;

		std::set<tab_suggestion> suggestions = BuildTabList(partial.c_str());

		if(suggestions.empty())
			return;

		//print list
		for(auto s = suggestions.begin(); s != suggestions.end(); ++s)
		{
			Con_SafePrintf("   %s (%s)\n", s->name.c_str(), s->type.c_str());
			tab_suggestions.push_back(s->name.substr(partial.length()));
		}
		
		tab_siter = tab_suggestions.begin();
	}
	else{
		input_cursor_pos -= tab_siter->length();
		input_line.erase(input_cursor_pos, tab_siter->length());

		if(reverse)
		{
			if(tab_siter == tab_suggestions.begin())
			{
				tab_siter = tab_suggestions.end();
			}

			--tab_siter;
		}
		else{
			if(++tab_siter == tab_suggestions.end())
			{
				tab_siter = tab_suggestions.begin();
			}
		}
	}

	input_line.insert(input_cursor_pos, *tab_siter);
	input_cursor_pos += tab_siter->length();
}

/*
==============================================================================

DRAWING

==============================================================================
*/

/* Draws the last few lines of output transparently over the game top. */
void Con_DrawNotify(void)
{
	if(con_instance)
	{
		con_instance->DrawNotify();
	}
}

/*	Draws the console with the solid background
	The typing input line at the bottom should only be drawn if typing is allowed
*/
void Con_DrawConsole(bool draw_input)
{
	if(con_instance)
	{
		con_instance->Draw(draw_input);
	}
}

/* TODO: Get rid of these functions, bring more of the engine into C++ land
 * and let them use the methods of con_instance directly.
*/

void Con_InputKey(int key, bool ctrl, bool shift)
{
	if(con_instance)
	{
		con_instance->InputKey(key, ctrl, shift);
	}
}
