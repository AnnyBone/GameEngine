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
#include "EngineMenu.h"
#include "EngineEditor.h"

/*

key up events are sent even if in console mode

*/


#define		MAXCMDLINE	256
char	key_lines[32][MAXCMDLINE];
unsigned int		key_linepos;
int		shift_down=false;
int		key_lastpress;
int		key_insert;	//johnfitz -- insert key toggle (for editing)
double	key_blinktime; //johnfitz -- fudge cursor blinking to make it easier to spot in certain cases

int		edit_line=0;
int		history_line=0;

keydest_t	key_dest;

int		key_count;			// incremented every key event

char	*keybindings[256];
int		keyshift[256];		// key to map to if shift held down in console
int		key_repeats[256];	// if > 1, it is autorepeating
bool	consolekeys[256],	// if true, can't be rebound while in console
		menubound[256],		// if true, can't be rebound while in menu
		keydown[256],
		repeatkeys[256];	//johnfitz -- if true, autorepeat is enabled for this key

typedef struct
{
	char	*name;
	int		keynum;
} keyname_t;

keyname_t keynames[] =
{
	{"TAB", INPUT_KEY_TAB},
	{"ENTER", INPUT_KEY_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},
	{"BACKSPACE", K_BACKSPACE},
	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},

	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},

	//johnfitz -- keypad
	{"KP_NUMLOCK",		KP_NUMLOCK},
	{"KP_SLASH",		KP_SLASH },
	{"KP_STAR",			KP_STAR },
	{"KP_MINUS",		KP_MINUS },
	{"KP_HOME",			KP_HOME },
	{"KP_UPARROW",		KP_UPARROW },
	{"KP_PGUP",			KP_PGUP },
	{"KP_PLUS",			KP_PLUS },
	{"KP_LEFTARROW",	KP_LEFTARROW },
	{"KP_5",			KP_5 },
	{"KP_RIGHTARROW",	KP_RIGHTARROW },
	{"KP_END",			KP_END },
	{"KP_DOWNARROW",	KP_DOWNARROW },
	{"KP_PGDN",			KP_PGDN },
	{"KP_ENTER",		KP_ENTER },
	{"KP_INS",			KP_INS },
	{"KP_DEL",			KP_DEL },
	//johnfitz

	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},

	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGDN", K_PGDN},
	{"PGUP", K_PGUP},
	{"HOME", K_HOME},
	{"END", K_END},

	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},

	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},

	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"AUX16", K_AUX16},
	{"AUX17", K_AUX17},
	{"AUX18", K_AUX18},
	{"AUX19", K_AUX19},
	{"AUX20", K_AUX20},
	{"AUX21", K_AUX21},
	{"AUX22", K_AUX22},
	{"AUX23", K_AUX23},
	{"AUX24", K_AUX24},
	{"AUX25", K_AUX25},
	{"AUX26", K_AUX26},
	{"AUX27", K_AUX27},
	{"AUX28", K_AUX28},
	{"AUX29", K_AUX29},
	{"AUX30", K_AUX30},
	{"AUX31", K_AUX31},
	{"AUX32", K_AUX32},

	{"PAUSE", K_PAUSE},

	{"MWHEELUP", K_MWHEELUP},
	{"MWHEELDOWN", K_MWHEELDOWN},

	{"SEMICOLON", ';'},	// because a raw semicolon seperates commands

	{NULL,0}
};

/*
==============================================================================

			LINE TYPING INTO THE CONSOLE

==============================================================================
*/

/*  Interactive line editing and console scrollback
	johnfitz -- heavy revision
*/
void Key_Console (int key)
{
	extern	char key_tabpartial[MAXCMDLINE];

	switch (key)
	{
	case INPUT_KEY_ENTER:
	case KP_ENTER:
		key_tabpartial[0] = 0;
		Cbuf_AddText (key_lines[edit_line]+1);	// skip the prompt
		Cbuf_AddText ("\n");
		Con_Printf ("%s\n",key_lines[edit_line]);
		edit_line = (edit_line + 1) & 31;
		history_line = edit_line;
		key_lines[edit_line][0] = ']';
		key_lines[edit_line][1] = 0; //johnfitz -- otherwise old history items show up in the new edit line
		key_linepos = 1;
		if (cls.state == ca_disconnected)
			SCR_UpdateScreen (); // force an update, because the command may take some time
		return;
	case INPUT_KEY_TAB:
		Con_TabComplete ();
		return;
	case K_BACKSPACE:
		key_tabpartial[0] = 0;
		if (key_linepos > 1)
		{
			strcpy(key_lines[edit_line] + key_linepos - 1, key_lines[edit_line] + key_linepos);
			key_linepos--;
		}
		return;
	case K_INS:
		key_insert ^= 1;
		return;
	case K_DEL:
		key_tabpartial[0] = 0;
		if ((unsigned)key_linepos < strlen(key_lines[edit_line]))
			strcpy(key_lines[edit_line] + key_linepos, key_lines[edit_line] + key_linepos + 1);
		return;
	case K_HOME:
		if (keydown[K_CTRL])
		{
			Con_ScrollHome();
		}
		else
			key_linepos = 1;
		return;
	case K_END:
		if (keydown[K_CTRL])
			Con_ScrollEnd();
		else
			key_linepos = strlen(key_lines[edit_line]);
		return;
	case K_PGUP:
		Con_ScrollUp();
		return;
	case K_PGDN:
		Con_ScrollDown();
		return;
	case K_LEFTARROW:
		if (key_linepos > 1)
		{
			key_linepos--;
			key_blinktime = realtime;
		}
		return;
	case K_RIGHTARROW:
		if (strlen(key_lines[edit_line]) == key_linepos)
		{
			if (strlen(key_lines[(edit_line + 31) & 31]) <= (unsigned)key_linepos)
				return; // no character to get

			key_lines[edit_line][key_linepos] = key_lines[(edit_line + 31) & 31][key_linepos];
			key_linepos++;
			key_lines[edit_line][key_linepos] = 0;
		}
		else
		{
			key_linepos++;
			key_blinktime = realtime;
		}
		return;
	case K_UPARROW:
		key_tabpartial[0] = 0;
		do
		{
			history_line = (history_line - 1) & 31;
		} while (history_line != edit_line
				&& !key_lines[history_line][1]);
		if (history_line == edit_line)
			history_line = (edit_line+1)&31;
		strcpy(key_lines[edit_line], key_lines[history_line]);
		key_linepos = strlen(key_lines[edit_line]);
		return;
	case K_DOWNARROW:
		key_tabpartial[0] = 0;

		if (history_line == edit_line)
		{
			//clear editline
			key_lines[edit_line][1] = 0;
			key_linepos = 1;
			return;
		}

		do {
			history_line = (history_line + 1) & 31;
		} while (history_line != edit_line
			&& !key_lines[history_line][1]);
		if (history_line == edit_line)
		{
			key_lines[edit_line][0] = ']';
			key_linepos = 1;
		}
		else
		{
			strcpy(key_lines[edit_line], key_lines[history_line]);
			key_linepos = strlen(key_lines[edit_line]);
		}
		return;
	}

//johnfitz -- clipboard pasting, stolen from zquake
//TODO: move win32 specific code to sys_win.c
#ifdef _WIN32
	if((key=='V' || key=='v') && keydown[K_CTRL])
	{
		HANDLE	th;
		char	*clipText;
		int		i;

		if(OpenClipboard(NULL))
		{
			th = GetClipboardData(CF_TEXT);
			if(th)
			{
				clipText = (char*)GlobalLock(th);
				if (clipText)
				{
					for (i=0; clipText[i]; i++)
						if (clipText[i]=='\n' || clipText[i]=='\r' || clipText[i]=='\b')
							break;
					if (i + strlen(key_lines[edit_line]) > MAXCMDLINE-1)
						i = MAXCMDLINE-1 - strlen(key_lines[edit_line]);
					if (i > 0)
					{	// insert the string
						memmove (key_lines[edit_line] + key_linepos + i,
							key_lines[edit_line] + key_linepos, strlen(key_lines[edit_line]) - key_linepos + 1);
						memcpy (key_lines[edit_line] + key_linepos, clipText, i);
						key_linepos += i;
					}
				}

				GlobalUnlock(th);
			}

			CloseClipboard();
		}
		return;
	}
#endif

	if (key < 32 || key > 127)
		return;	// non printable

	//johnfitz -- stolen from darkplaces
	if (key_linepos < MAXCMDLINE-1)
	{
		unsigned int i;

		key_tabpartial[0] = 0; //johnfitz

		if (key_insert)	// check insert mode
		{
			// can't do strcpy to move string to right
			i = strlen(key_lines[edit_line]) - 1;
			if (i == 254)
				i--;

			for (; i >= key_linepos; i--)
				key_lines[edit_line][i + 1] = key_lines[edit_line][i];
		}

		// only null terminate if at the end
		i = key_lines[edit_line][key_linepos];
		key_lines[edit_line][key_linepos] = key;
		key_linepos++;

		if (!i)
			key_lines[edit_line][key_linepos] = 0;
	}
	//johnfitz
}

//============================================================================

char chat_buffer[64];
bool team_message = false;

void Key_Message (int key)
{
	static int chat_bufferlen = 0;

	if (key == INPUT_KEY_ENTER)
	{
		if (team_message)
			Cbuf_AddText ("say_team \"");
		else
			Cbuf_AddText ("say \"");
		Cbuf_AddText(chat_buffer);
		Cbuf_AddText("\"\n");

		key_dest = key_game;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		return;
	}

	if (key == K_ESCAPE)
	{
		key_dest = key_game;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		return;
	}

	if (key < 32 || key > 127)
		return;	// non printable

	if (key == K_BACKSPACE)
	{
		if (chat_bufferlen)
		{
			chat_bufferlen--;
			chat_buffer[chat_bufferlen] = 0;
		}
		return;
	}

	if (chat_bufferlen == 63)
		return; // all full

	chat_buffer[chat_bufferlen++] = key;
	chat_buffer[chat_bufferlen] = 0;
}

//============================================================================


/*
===================
Key_StringToKeynum

Returns a key number to be used to index keybindings[] by looking at
the given string.  Single ascii characters return themselves, while
the K_* names are matched up.
===================
*/
int Key_StringToKeynum (char *str)
{
	keyname_t	*kn;

	if (!str || !str[0])
		return -1;
	if (!str[1])
		return str[0];

	for (kn=keynames ; kn->name ; kn++)
	{
		if (!strcasecmp(str, kn->name))
			return kn->keynum;
	}

	return -1;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, or a K_* name) for the
given keynum.
FIXME: handle quote special (general escape sequence?)
===================
*/
char *Key_KeynumToString (int keynum)
{
	keyname_t	*kn;
	static	char	tinystr[2];

	if (keynum == -1)
		return "<KEY NOT FOUND>";
	if (keynum > 32 && keynum < 127)
	{	// printable ascii
		tinystr[0] = keynum;
		tinystr[1] = 0;
		return tinystr;
	}

	for (kn=keynames ; kn->name ; kn++)
		if (keynum == kn->keynum)
			return kn->name;

	return "<UNKNOWN KEYNUM>";
}

void Key_SetBinding (int keynum, char *binding)
{
	char	*newstring;
	int		l;

	if (keynum == -1)
		return;

	// free old bindings
	free(keybindings[keynum]);
	keybindings[keynum] = NULL;

	// allocate memory for new binding
	l = strlen(binding);
	newstring = (char*)malloc_or_die(l + 1);
	strcpy(newstring, binding);
	newstring[l] = 0;
	keybindings[keynum] = newstring;
}

void Key_Unbind_f (void)
{
	int		b;

	if (Cmd_Argc() != 2)
	{
		Con_Printf ("unbind <key> : remove commands from a key\n");
		return;
	}

	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Con_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding (b, "");
}

void Key_Unbindall_f (void)
{
	int		i;

	for (i=0 ; i<256 ; i++)
		if (keybindings[i])
			Key_SetBinding (i, "");
}

void Key_Bindlist_f (void)
{
	int		i, count;

	count = 0;
	for (i=0 ; i<256 ; i++)
		if (keybindings[i])
			if (*keybindings[i])
			{
				Con_SafePrintf ("   %s \"%s\"\n", Key_KeynumToString(i), keybindings[i]);
				count++;
			}
	Con_SafePrintf ("%i bindings\n", count);
}

void Key_Bind_f (void)
{
	int			i, c, b;
	char		cmd[1024];

	c = Cmd_Argc();

	if (c != 2 && c != 3)
	{
		Con_Printf ("bind <key> [command] : attach a command to a key\n");
		return;
	}
	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Con_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if (c == 2)
	{
		if (keybindings[b])
			Con_Printf ("\"%s\" = \"%s\"\n", Cmd_Argv(1), keybindings[b] );
		else
			Con_Printf ("\"%s\" is not bound\n", Cmd_Argv(1) );
		return;
	}

// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for (i=2 ; i< c ; i++)
	{
		if (i > 2)
			strcat (cmd, " ");
		strcat (cmd, Cmd_Argv(i));
	}

	Key_SetBinding (b, cmd);
}

/*	Writes lines containing "bind key value"
*/
void Key_WriteBindings (FILE *f)
{
	int		i;

	for (i=0 ; i<256 ; i++)
		if (keybindings[i])
			if (*keybindings[i])
				fprintf (f, "bind \"%s\" \"%s\"\n", Key_KeynumToString(i), keybindings[i]);
}

void Key_Init (void)
{
	int		i;

	for (i=0 ; i<32 ; i++)
	{
		key_lines[i][0] = ']';
		key_lines[i][1] = 0;
	}
	key_linepos = 1;

	key_blinktime = realtime; //johnfitz

	// Init ascii characters in console mode
	for(i = 32; i < 128; i++)
		consolekeys[i] = true;
	consolekeys[INPUT_KEY_ENTER]	= true;
	consolekeys[KP_ENTER]		    = true; //johnfitz
	consolekeys[INPUT_KEY_TAB]	    = true;
	consolekeys[K_LEFTARROW]	    = true;
	consolekeys[K_RIGHTARROW]	    = true;
	consolekeys[K_UPARROW]		    = true;
	consolekeys[K_DOWNARROW]	    = true;
	consolekeys[K_BACKSPACE]	    = true;
	consolekeys[K_DEL]			    = true; //johnfitz
	consolekeys[K_INS]			    = true; //johnfitz
	consolekeys[K_HOME]			    = true; //johnfitz
	consolekeys[K_END]			    = true; //johnfitz
	consolekeys[K_PGUP]			    = true;
	consolekeys[K_PGDN]			    = true;
	consolekeys[K_SHIFT]		    = true;
	consolekeys[K_MWHEELUP]		    = true;
	consolekeys[K_MWHEELDOWN]	    = true;
	consolekeys['`']			    = false;
	consolekeys['~']			    = false;

	//johnfitz -- repeating keys
	for(i = 0; i < 256; i++)
		repeatkeys[i] = false;

	repeatkeys[K_BACKSPACE]		= true;
	repeatkeys[K_DEL]			= true;
	repeatkeys[K_PAUSE]			= true;
	repeatkeys[K_PGUP]			= true;
	repeatkeys[K_PGDN]			= true;
	repeatkeys[K_LEFTARROW]		= true;
	repeatkeys[K_RIGHTARROW]	= true;
	//johnfitz

	for(i = 0; i < 256; i++)
		keyshift[i] = i;

	for(i = 'a'; i <= 'z'; i++)
		keyshift[i] = i-'a'+'A';

	keyshift['1']	= '!';
	keyshift['2']	= '@';
	keyshift['3']	= '#';
	keyshift['4']	= '$';
	keyshift['5']	= '%';
	keyshift['6']	= '^';
	keyshift['7']	= '&';
	keyshift['8']	= '*';
	keyshift['9']	= '(';
	keyshift['0']	= ')';
	keyshift['-']	= '_';
	keyshift['=']	= '+';
	keyshift[',']	= '<';
	keyshift['.']	= '>';
	keyshift['/']	= '?';
	keyshift[';']	= ':';
	keyshift['\'']	= '"';
	keyshift['[']	= '{';
	keyshift[']']	= '}';
	keyshift['`']	= '~';
	keyshift['\\']	= '|';

	menubound[K_ESCAPE] = true;
	for (i=0 ; i<12 ; i++)
		menubound[K_F1+i] = true;

	// register our functions
	Cmd_AddCommand ("bindlist",Key_Bindlist_f); //johnfitz
	Cmd_AddCommand ("bind",Key_Bind_f);
	Cmd_AddCommand ("unbind",Key_Unbind_f);
	Cmd_AddCommand ("unbindall",Key_Unbindall_f);
}

/*	Called by the system between frames for both key up and key down events
	Should NOT be called during an interrupt!
*/
void Key_Event(int key,bool down)
{
	char	*kb,
			cmd[1024];

	keydown[key] = down;
	if (!down)
		key_repeats[key] = 0;

	key_lastpress = key;
	key_count++;
	if (key_count <= 0)
		return;		// just catching keys for Con_NotifyBox

// update auto-repeat status
	if(down)
	{
		key_repeats[key]++;
		if (key_repeats[key] > 1 && !repeatkeys[key]) //johnfitz -- use repeatkeys[]
			return;	// ignore most autorepeats

		if (key >= 200 && !keybindings[key])
			Con_Printf ("%s is unbound, hit F4 to set.\n", Key_KeynumToString (key) );
	}

	if (key == K_SHIFT)
		shift_down = down;

	// Handle escape specialy, so the user can never accidently unbind it.
	if(key == K_ESCAPE)
	{
		if(!down)
			return;

		switch (key_dest)
		{
		case KEY_EDITOR:
			Editor_Input(key);
			break;
		case key_message:
			Key_Message (key);
			break;
		case key_menu:
			g_menu->Input(key);
			break;
		case key_game:
		case key_console:
			Menu_Toggle();
			break;
		default:
			Sys_Error ("Bad key_dest");
		}
		return;
	}

	// Allow users to use ALT and ENTER to toggle fullscreen.
	if((key == INPUT_KEY_ENTER) && keydown[K_ALT])
	{
		if (cv_video_fullscreen.value)
			Cvar_SetValue("video_fullscreen",0);
		else
			Cvar_SetValue("video_fullscreen",1);

		Video_UpdateWindow();
		return;
	}

//
// key up events only generate commands if the game key binding is
// a button command (leading + sign).  These will occur even in console mode,
// to keep the character from continuing an action started before a console
// switch.  Button commands include the kenum as a parameter, so multiple
// downs can be matched with ups
//
	if(!down)
	{
		kb = keybindings[key];
		if (kb && kb[0] == '+')
		{
			sprintf (cmd, "-%s %i\n", kb+1, key);
			Cbuf_AddText (cmd);
		}

		if (keyshift[key] != key)
		{
			kb = keybindings[keyshift[key]];
			if (kb && kb[0] == '+')
			{
				sprintf (cmd, "-%s %i\n", kb+1, key);
				Cbuf_AddText (cmd);
			}
		}
		return;
	}

	// During demo playback, most keys bring up the main menu
	if (cls.demoplayback && down && consolekeys[key] && key_dest == key_game)
	{
		M_ToggleMenu_f ();
		return;
	}

//
// if not a consolekey, send to the interpreter no matter what mode is
//
	if ( (key_dest == key_menu && menubound[key])
	|| (key_dest == key_console && !consolekeys[key])
	|| (key_dest == key_game && ( !con_forcedup || !consolekeys[key] ) ) )
	{
		kb = keybindings[key];
		if (kb)
		{
			if (kb[0] == '+')
			{	// button commands add keynum as a parm
				sprintf (cmd, "%s %i\n", kb, key);
				Cbuf_AddText (cmd);
			}
			else
			{
				Cbuf_AddText (kb);
				Cbuf_AddText ("\n");
			}
		}

		return;
	}

	if(!down)
		return;		// other systems only care about key down events

	if (shift_down)
		key = keyshift[key];

	switch(key_dest)
	{
	case KEY_EDITOR:
		Editor_Input(key);
		break;
	case KEY_EDITOR_MATERIAL:
		break;
	case key_message:
		Key_Message(key);
		break;
	case key_menu:
		g_menu->Input(key);
		break;
	case key_game:
	case key_console:
		Key_Console(key);
		break;
	default:
		Con_Warning("Bad key destination! (%i)",key);
	}
}

void Key_ClearStates (void)
{
	int i;

	for(i = 0; i < 256; i++)
		// If the key is down, trigger the up action if, say, +showscores or another +bind is activated
		if(keydown[i])
			Key_Event(i,false);
}

