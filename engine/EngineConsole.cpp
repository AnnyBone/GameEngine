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
#include <list>
#include <set>

#include "platform_log.h"

#include "engine_base.h"

#include "input.h"
#include "video.h"

static const unsigned int CHAR_WIDTH = 8;
static const unsigned int CHAR_HEIGHT = 8;
static const unsigned int CONS_BACKLOG = 200;
static const unsigned int CONS_MAXNOTIFY = 10;

float con_cursorspeed = 4;

bool con_forcedup;        // because no entities to refresh

ConsoleVariable_t con_notifytime = {"con_notifytime", "3"};        //seconds
ConsoleVariable_t con_logcenterprint = {"con_logcenterprint", "1"}; //johnfitz

char con_lastcenterstring[1024]; //johnfitz

#define    MAXCMDLINE    256

extern "C" char key_lines[32][MAXCMDLINE];
extern "C" int edit_line;
extern "C" unsigned int key_linepos;

bool g_consoleinitialized;

using namespace core;

/* Singleton console instance. */
Console *g_console = nullptr;

Console::Console() :
        cursor_x(0),
        cursor_y(0),
        backscroll(0) {
    lines.emplace_back("");
}

void Console::Clear() {
    cursor_x = 0;
    cursor_y = 0;

    lines.clear();
    lines.emplace_back("");
}

void Console::ClearNotify() {
    for (auto l = lines.begin(); l != lines.end(); ++l) {
        l->time = 0;
    }
}

void Console::Print(const char *text) {
    /* A string starting with 0x02 is "coloured" and the characters are
     * ORd with 128 so the rendering code highlights them.
    */
    unsigned char cbit = 0;
    if (*text == 0x02) {
        cbit = 128;
        ++text;
    }

    while (*text) {
        if (*text == '\n') {
            linefeed();
            cursor_x = 0;
        } else if (*text == '\r') {
            cursor_x = 0;
        } else {
            if (lines[cursor_y].text.size() > cursor_x)
                lines[cursor_y].text[cursor_x] = (*text | cbit);
            else
                lines[cursor_y].text += (*text | cbit);

            lines[cursor_y].time = time(NULL);

            ++cursor_x;
        }

        ++text;
    }
}

void Console::ScrollUp() {
    ++backscroll;
}

void Console::ScrollDown() {
    if (backscroll > 0)
        --backscroll;
}

void Console::ScrollHome() {
    backscroll = CONS_BACKLOG;
}

void Console::ScrollEnd() {
    backscroll = 0;
}

void Console::linefeed() {
    lines.emplace_back("");

    if (lines.size() > CONS_BACKLOG)
        lines.pop_front();
    else
        ++cursor_y;
}

std::list<std::string> Console::prepare_text(unsigned int cols, unsigned int rows) {
    std::list<std::string> wrapped_lines;

    /* Start working back from the end of the deque of lines in the buffer
     * until we have enough to fill the visible rows + scrollback
    */
    for (auto l = lines.rbegin();
         l != lines.rend() && (backscroll == (size_t) (-1) || wrapped_lines.size() < rows + backscroll);
         ++l) {
        auto wrapped_line = wrap_line(l->text, cols);
        wrapped_lines.insert(wrapped_lines.begin(), wrapped_line.begin(), wrapped_line.end());
    }

    /* Clamp the backscroll value to the maximum value for the current
     * console size and contents. Doing it here is a little ick, but I can't
     * think of a better way to do it without introducing more bits of state
     * right now.
    */
    if (lines.size() < rows)
        backscroll = 0;
    else
        backscroll = std::min(backscroll, (lines.size() - rows));

    /* Chop off the lines currently scrolled back past. */
    for (unsigned int i = 0; i < backscroll && wrapped_lines.size() > rows; ++i) {
        wrapped_lines.pop_back();
    }

    while (wrapped_lines.size() > rows) {
        wrapped_lines.pop_front();
    }

    return wrapped_lines;
}

std::list<std::string> Console::wrap_line(std::string line, unsigned int cols) {
    std::list<std::string> wrapped_line;

    do {
        size_t frag_len;

        if (line.length() <= cols)
            frag_len = line.length();
        else {
            size_t break_space = line.find_last_of(' ', cols);

            if (break_space == std::string::npos || break_space == 0)
                frag_len = cols;
            else
                frag_len = break_space;
        }

        wrapped_line.emplace_back(line, 0, frag_len);
        line.erase(0, frag_len);
    } while (!line.empty());

    return wrapped_line;
}

void Console::SetSize(unsigned int width, unsigned int height) {
    assert((width != 0) || (height != 0));

#if 1    // todo, do we need this?
    vid.conwidth = (scr_conwidth.value > 0) ? (int) scr_conwidth.value : (scr_conscale.value > 0) ? (int) (width /
                                                                                                           scr_conscale.value)
                                                                                                  : width;
    vid.conwidth = plClamp(320, vid.conwidth, width);
    vid.conwidth &= 0xFFFFFFF8;
    vid.conheight = vid.conwidth * height / width;
#endif

    _width = width & 0xFFFFFFF8;
    _height = _width * height / width;

    // todo, merge this into here.
    Screen_SetUpToDrawConsole(width, height);
}

void Console::Draw(bool draw_input) {
    const unsigned int con_cols = _width / CHAR_WIDTH;
    const unsigned int line_width = con_cols - 3;

    GL_SetCanvas(CANVAS_CONSOLE);

    float bgalpha = 0.5f;
    if (cls.state != ca_connected)
        bgalpha = 1;

    // Draw the background.
    PLColour black = {0, 0, 0, 255};
    PLColour lightblack = {0, 0, 0, bgalpha};
    draw::GradientFill(0, 0, vid.conwidth, vid.conheight, black, lightblack);

    // Starting from the bottom...
    int y = vid.conheight - CHAR_HEIGHT;

    glDisable(GL_DEPTH_TEST);

    Material_Draw(g_mGlobalConChars, NULL, VL_PRIMITIVE_IGNORE, 0, false);

    // ...draw version number in bottom right...
    {
        char ver[64];
        snprintf(ver, sizeof(ver), "Xenon (%i)", (int) (ENGINE_VERSION_BUILD));

        size_t vlen = strlen(ver);
        size_t x = con_cols - vlen;

        for (size_t i = 0; i < vlen; ++i, ++x)
            draw::Character((PLint) x * CHAR_WIDTH, y, ver[i]);

        y -= CHAR_HEIGHT;
    }

    // ...draw input line...
    if (draw_input && !(key_dest != key_console && !con_forcedup)) {
        const char *text = key_lines[edit_line];

        // Prestep if horizontally scrolling
        if (key_linepos >= line_width)
            text += 1 + key_linepos - line_width;

        // Draw input string
        for (size_t i = 0;; ++i) {
            if (text[i] != '\0')
                draw::Character((PLint) (i + 1) * CHAR_WIDTH, y, text[i]);
            else {
                // Why is this even necessary?
                draw::Character((PLint) (i + 1) * CHAR_WIDTH, y, ' ');
                break;
            }
        }

        // Draw the blinky cursor
        if (!((int) ((realtime - key_blinktime) * con_cursorspeed) & 1))
            draw::Character((key_linepos + 1) * CHAR_WIDTH, y, '_');

        y -= CHAR_HEIGHT;
    }

    // ...draw visible console output in remaining space...
    {
        const unsigned int rows = (y / CHAR_HEIGHT) + 1;

        std::list<std::string> wrapped_lines = prepare_text(line_width, rows);

        for (auto line = wrapped_lines.rbegin(); line != wrapped_lines.rend() && y >= 0;) {
            for (size_t i = 0; i < line->length(); ++i)
                draw::Character((PLint) (i + 1) * CHAR_WIDTH, y, line->at(i));

            y -= CHAR_HEIGHT;
            ++line;
        }
    }

    Material_Draw(g_mGlobalConChars, NULL, VL_PRIMITIVE_IGNORE, 0, true);

    glEnable(GL_DEPTH_TEST);
}

void Console::DrawNotify() {
    const unsigned int cols = (vid.conwidth / CHAR_WIDTH) - 2;

    GL_SetCanvas(CANVAS_CONSOLE);

    time_t now = time(NULL);

    std::list<std::string> wrapped_lines;

    for (auto l = lines.rbegin(); l != lines.rend() && wrapped_lines.size() < CONS_MAXNOTIFY; ++l) {
        if (l->time != 0 && (l->time + con_notifytime.iValue) < now) {
            /* Message has expired, implies all the ones above it
             * are also expired, so break here to save on a load of
             * pointless work.
            */
            break;
        } else if (l->time == 0) {
            /* Message time uninitialised or cleared, skip */
            continue;
        }

        auto wrapped_line = wrap_line(l->text, cols);
        wrapped_lines.insert(wrapped_lines.begin(), wrapped_line.begin(), wrapped_line.end());
    }

    unsigned int y = vid.conheight;

    Material_Draw(g_mGlobalConChars, NULL, VL_PRIMITIVE_IGNORE, 0, false);

    for (auto l = wrapped_lines.begin(); l != wrapped_lines.end(); ++l) {
        for (size_t i = 0; i < l->size(); ++i)
            draw::Character((PLint) ((i + 1) * CHAR_WIDTH), y, (*l)[i]);

        y += CHAR_HEIGHT;
    }

    if (key_dest == key_message) {
        const char *say_prompt = team_message ? "Say (team):" : "Say (all):";
        size_t plen = strlen(say_prompt);

        //johnfitz -- distinguish say and say_team
        if (team_message)
            say_prompt = "Say (team):";
        else
            say_prompt = "Say (all):";
        //johnfitz

        Draw_String(CHAR_WIDTH, y, say_prompt); //johnfitz

        size_t x = 0;
        while (chat_buffer[x]) {
            draw::Character((PLint) (x + plen + 2) * CHAR_WIDTH, y, chat_buffer[x]);
            ++x;
        }

        draw::Character((PLint) (x + plen + 2) * CHAR_WIDTH, y, 10 + ((int) (realtime * con_cursorspeed) & 1));
        //y += CHAR_HEIGHT;
    }

    Material_Draw(g_mGlobalConChars, NULL, VL_PRIMITIVE_IGNORE, 0, true);
}

extern "C" void M_Menu_Main_f(void);

/* Returns a bar of the desired length, but never wider than the console. */
char *Con_Quakebar(unsigned int len) {
    const unsigned int con_linewidth = (vid.conwidth / CHAR_WIDTH) - 3;

    static char bar[42];
    len = std::min(len, (unsigned int) sizeof(bar) - 2);
    len = std::min(len, con_linewidth);

    bar[0] = '\35';
    std::memset(bar + 1, '\36', len - 2);
    bar[len - 1] = '\37';

    if (len < con_linewidth) {
        bar[len] = '\n';
        bar[len + 1] = 0;
    } else
        bar[len] = 0;

    return bar;
}

extern "C" int history_line; //johnfitz

void Con_ToggleConsole_f(void) {
    if (key_dest == key_console) {
        if (cls.state == ca_connected) {
            key_dest = key_game;
            key_lines[edit_line][1] = 0;    // clear any typing
            key_linepos = 1;
            history_line = edit_line; //johnfitz -- it should also return you to the bottom of the command history

            Input_ActivateMouse();
        } else
            M_Menu_Main_f();
    } else {
        key_dest = key_console;

        Input_DeactivateMouse();
    }

    SCR_EndLoadingPlaque();
}

void Con_Clear_f(void) {
    if (g_console)
        g_console->Clear();
}

void Con_ClearNotify(void) {
    if (g_console)
        g_console->ClearNotify();
}

void Con_MessageMode_f(void) {
    key_dest = key_message;
    team_message = false;
}

void Con_MessageMode2_f(void) {
    key_dest = key_message;
    team_message = true;
}

void Console_Initialize(void) {
    if (g_consoleinitialized)
        return;

    plClearLog(ENGINE_LOG);

    g_console = new Console();

    // register our commands
    Cvar_RegisterVariable(&con_notifytime, NULL);
    Cvar_RegisterVariable(&con_logcenterprint, NULL);

    Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
    Cmd_AddCommand("messagemode", Con_MessageMode_f);
    Cmd_AddCommand("messagemode2", Con_MessageMode2_f);
    Cmd_AddCommand("clear", Con_Clear_f);

    g_consoleinitialized = true;
}

/*	Handles cursor positioning, line wrapping, etc
	All console printing must go through this in order to be logged to disk
	If no console is visible, the notify window will pop up.
*/
void Con_Print(char *txt) {
    if (g_console)
        g_console->Print(txt);
}

#define    MAXPRINTMSG    4096

/*	Handles cursor positioning, line wrapping, etc
*/

void Con_Printf(const char *fmt, ...) {
    va_list argptr;
    char msg[MAXPRINTMSG];

    va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end(argptr);

    // Also echo to debugging console.
    Sys_Printf("%s", msg);

    plWriteLog(ENGINE_LOG, "%s", msg);

    if (g_state.embedded) g_launcher.PrintMessage(msg);

    Con_Print(msg);
}

void Con_Warning(const char *fmt, ...) {
    va_list argptr;
    char msg[MAXPRINTMSG];

    va_start (argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end (argptr);

    if (g_state.embedded) {
        plWriteLog(ENGINE_LOG, "%s", msg);
        g_launcher.PrintWarning(msg);
    } else {
        Con_SafePrintf("\x02Warning: ");
        Con_Printf("%s", msg);
    }
}

void Con_Error(char *fmt, ...) {
    static bool bInError = false;
    va_list argptr;
    char msg[MAXPRINTMSG];

    if (bInError)
        Sys_Error("Con_Error: Recursively entered\nCheck log for details.\n");
    bInError = true;

    va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end(argptr);

    if (g_state.embedded) {
        plWriteLog(ENGINE_LOG, "%s", msg);
        g_launcher.PrintError(msg);
    } else
        Con_Printf("\nError: %s\n", msg);

    CL_Disconnect_f();

    bInError = false;

    longjmp(host_abortserver, 1);
    /*Todo:
     - Clear everything out*/
}

void Con_DPrintf(const char *fmt, ...) {
    va_list argptr;
    char msg[MAXPRINTMSG];

    if (!developer.bValue)
        return;

    va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end(argptr);

    if (g_state.embedded) {
        plWriteLog(ENGINE_LOG, "%s", msg);
        g_launcher.PrintMessage(msg);
    } else {
        plWriteLog(ENGINE_LOG, "%s", msg);
        Con_SafePrintf("%s", msg); //johnfitz -- was Con_Printf
    }
}

/*	Okay to call even when the screen can't be updated
*/
void Con_SafePrintf(const char *fmt, ...) {
    va_list argptr;
    char msg[1024];

    va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end(argptr);

    PLbool temp = scr_disabled_for_loading;
    scr_disabled_for_loading = PL_TRUE;
    Con_Printf("%s", msg);
    scr_disabled_for_loading = temp;
}

void Con_CenterPrintf(unsigned int linewidth, char *fmt, ...) {
    va_list argptr;
    char msg[MAXPRINTMSG]; //the original message
    char line[MAXPRINTMSG]; //one line from the message
    char spaces[21]; //buffer for spaces
    char *src, *dst;

    va_start (argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end (argptr);

    size_t len, s;
    linewidth = std::min(linewidth, (vid.conwidth / CHAR_WIDTH) - 3);
    for (src = msg; *src;) {
        dst = line;
        while (*src && *src != '\n')
            *dst++ = *src++;
        *dst = 0;
        if (*src == '\n')
            src++;

        len = strlen(line);
        if (len < linewidth) {
            s = (linewidth - len) / 2;
            memset(spaces, ' ', s);
            spaces[s] = 0;
            Con_Printf("%s%s\n", spaces, line);
        } else
            Con_Printf("%s\n", line);
    }
}

void Con_LogCenterPrint(char *str) {
    if (!strcmp(str, con_lastcenterstring))
        return; // Ignore duplicates

    if (cl.gametype == GAME_DEATHMATCH && con_logcenterprint.value != 2)
        return; // Don't log in deathmatch

    strcpy(con_lastcenterstring, str);

    if (con_logcenterprint.value) {
        Con_Printf(Con_Quakebar(40));
        Con_CenterPrintf(40, "%s\n", str);
        Con_Printf(Con_Quakebar(40));
        Con_ClearNotify();
    }
}

void Console_ErrorMessage(bool bCrash, const char *ccFile, const char *reason) {
    if (bCrash) Sys_Error("Failed to load %s\nReason: %s", ccFile, reason);
    else Con_Error("Failed to load %s\nReason: %s", ccFile, reason);
}

/*
==============================================================================

	TAB COMPLETION

==============================================================================
*/

extern "C" char key_tabpartial[MAXCMDLINE];

struct tab_suggestion {
    std::string name;
    std::string type;

    tab_suggestion(const std::string &n, const std::string &t) :
            name(n), type(t) {}

    bool operator<(const tab_suggestion &rhs) const {
        return name < rhs.name;
    }
};

std::set<tab_suggestion> BuildTabList(const char *partial) {
    std::set<tab_suggestion> suggestions;

    size_t len = strlen(partial);

    for (ConsoleVariable_t *cvar = cConsoleVariables; cvar; cvar = cvar->next)
        if (!strncmp(partial, cvar->name, len))
            suggestions.insert(tab_suggestion(cvar->name, "cvar"));

    for (cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next)
        if (!strncmp(partial, cmd->name, len))
            suggestions.insert(tab_suggestion(cmd->name, "command"));

    for (cmdalias_t *alias = cmd_alias; alias; alias = alias->next)
        if (!strncmp(partial, alias->name, len))
            suggestions.insert(tab_suggestion(alias->name, "alias"));

    return suggestions;
}

void Con_TabComplete(void) {
    char partial[MAXCMDLINE];
    static char *c;
    int i;

    // if editline is empty, return
    if (key_lines[edit_line][1] == 0)
        return;

    // get partial string (space -> cursor)
    if (!strlen(key_tabpartial)) //first time through, find new insert point. (Otherwise, use previous.)
    {
        //work back from cursor until you find a space, quote, semicolon, or prompt
        c = key_lines[edit_line] + key_linepos - 1; //start one space left of cursor
        while (*c != ' ' && *c != '\"' && *c != ';' && c != key_lines[edit_line])
            c--;
        c++; //start 1 char after the seperator we just found
    }
    for (i = 0; c + i < key_lines[edit_line] + key_linepos; i++)
        partial[i] = c[i];
    partial[i] = 0;

    //if partial is empty, return
    if (partial[0] == 0)
        return;

    //trim trailing space becuase it screws up string comparisons
    if (i > 0 && partial[i - 1] == ' ')
        partial[i - 1] = 0;

    // find a match
    std::string match;

    if (!strlen(key_tabpartial)) //first time through
    {
        strcpy(key_tabpartial, partial);
        std::set<tab_suggestion> suggestions = BuildTabList(key_tabpartial);

        if (suggestions.empty()) {
            return;
        }

        //print list
        for (auto s = suggestions.begin(); s != suggestions.end(); ++s) {
            Con_SafePrintf("   %s (%s)\n", s->name.c_str(), s->type.c_str());
        }

        //get first match
        match = suggestions.begin()->name;
    } else {
        std::set<tab_suggestion> suggestions = BuildTabList(key_tabpartial);

        if (suggestions.empty()) {
            return;
        }

        auto s = suggestions.find(tab_suggestion(partial, ""));
        if (s == suggestions.end()) {
            Con_SafePrintf("BUG: s == suggestions.end()\n");
            return;
        }

        /* Iterate backwards through the suggestions if shift is held
         * down. Emulate a cyclic data structure here.
        */
        if (keydown[K_SHIFT]) {
            if (s == suggestions.begin()) {
                s = suggestions.end();
            }

            --s;
        } else {
            if (++s == suggestions.end()) {
                s = suggestions.begin();
            }
        }

        match = s->name;
    }

    /* Update input text to contain new suggestion and any preexisting
     * trailing text.
     *
     * NOTE: Don't optimise the intermediate buffer away - c points to a
     * region within key_lines and everything goes wrong.
    */
    snprintf(partial, sizeof(partial), "%s%s", match.c_str(), (key_lines[edit_line] + key_linepos));
    strcpy(c, partial);

    key_linepos = c - key_lines[edit_line] + match.length(); //set new cursor position

    // if cursor is at end of string, let's append a space to make life easier
    if (key_lines[edit_line][key_linepos] == 0) {
        key_lines[edit_line][key_linepos] = ' ';
        key_linepos++;
        key_lines[edit_line][key_linepos] = 0;
    }
}

/*
==============================================================================

DRAWING

==============================================================================
*/

/* Draws the last few lines of output transparently over the game top. */
void Con_DrawNotify(void) {
    if (g_console) {
        g_console->DrawNotify();
    }
}

/*	Draws the console with the solid background
	The typing input line at the bottom should only be drawn if typing is allowed
*/
void Con_DrawConsole(bool draw_input) {
    if (g_console) {
        g_console->Draw(draw_input);
    }
}

/* TODO: Get rid of these functions, bring more of the engine into C++ land
 * and let them use the methods of con_instance directly.
*/

void Con_ScrollUp(void) {
    if (g_console) {
        g_console->ScrollUp();
    }
}

void Con_ScrollDown(void) {
    if (g_console) {
        g_console->ScrollDown();
    }
}

void Con_ScrollHome(void) {
    if (g_console) {
        g_console->ScrollHome();
    }
}

void Con_ScrollEnd(void) {
    if (g_console) {
        g_console->ScrollEnd();
    }
}
