/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others

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
// sbar.c -- status bar code

#include "quakedef.h"

#include "KatGL.h"
#include "engine_video.h"

#define STAT_MINUS		10	// num frame for '-' stats digit
qpic_t		*sb_nums[2][11];
qpic_t		*sb_colon, *sb_slash;
qpic_t		*sb_ibar;
qpic_t		*sb_scorebar;

qpic_t		*sb_armor[3];
qpic_t		*sb_items[32];

qpic_t	*sb_faces[7][2];		// 0 is gibbed, 1 is dead, 2-6 are alive
							// 0 is static, 1 is temporary animation
qpic_t	*sb_face_quad;
qpic_t	*sb_face_invuln;
qpic_t	*sb_face_invis_invuln;

bool	sb_showscores;

int			sb_lines;			// scan lines to draw

void Sbar_MiniDeathmatchOverlay (void);
void Sbar_DeathmatchOverlay (void);
void M_DrawPic (int x, int y, qpic_t *pic);

// drawing routines are relative to the status bar location

void Sbar_DrawPic (int x, int y, qpic_t *pic)
{
	Draw_Pic (x, y + 24, pic);
}

void Sbar_DrawPicAlpha (int x, int y, qpic_t *pic, float alpha)
{
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glColor4f(1,1,1,alpha);
	Draw_Pic(x, y + 24, pic);
	glColor3f(1,1,1);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
}

void Sbar_DrawCharacter (int x, int y, int num)
{
	Draw_Character (x, y + 24, num);
}

int Sbar_itoa (int num, char *buf)
{
	char	*str;
	int		pow10;
	int		dig;

	str = buf;

	if (num < 0)
	{
		*str++ = '-';
		num = -num;
	}

	for (pow10 = 10 ; num >= pow10 ; pow10 *= 10)
	;

	do
	{
		pow10 /= 10;
		dig = num/pow10;
		*str++ = '0'+dig;
		num -= dig*pow10;
	} while (pow10 != 1);

	*str = 0;

	return str-buf;
}

void Sbar_DrawNum (int x, int y, int num, int digits, int color)
{
#if 0
	char			str[12];
	char			*ptr;
	int				l, frame;

	num = min(999,num); //johnfitz -- cap high values rather than truncating number

	l = Sbar_itoa (num, str);
	ptr = str;
	if (l > digits)
		ptr += (l-digits);
	if (l < digits)
		x += (digits-l)*24;

	while (*ptr)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		Sbar_DrawPic(x,y,sb_nums[color][frame]); //johnfitz -- DrawTransPic is obsolete
		x += 24;
		ptr++;
	}
#else
	Con_Printf("OBSOLETE: Sbar_DrawNum\n");
#endif
}

int		fragsort[MAX_SCOREBOARD];

char	scoreboardtext[MAX_SCOREBOARD][20];
int		scoreboardtop[MAX_SCOREBOARD];
int		scoreboardbottom[MAX_SCOREBOARD];
int		scoreboardcount[MAX_SCOREBOARD];
int		scoreboardlines;

void Sbar_SortFrags (void)
{
	int		i, j, k;

// sort by frags
	scoreboardlines = 0;
	for (i=0 ; i<cl.maxclients ; i++)
	{
		if (cl.scores[i].name[0])
		{
			fragsort[scoreboardlines] = i;
			scoreboardlines++;
		}
	}

	for (i=0 ; i<scoreboardlines ; i++)
		for (j=0 ; j<scoreboardlines-1-i ; j++)
			if (cl.scores[fragsort[j]].frags < cl.scores[fragsort[j+1]].frags)
			{
				k = fragsort[j];
				fragsort[j] = fragsort[j+1];
				fragsort[j+1] = k;
			}
}

int	Sbar_ColorForMap (int m)
{
	return m < 128 ? m + 8 : m + 8;
}

void Sbar_DrawScoreboard (void)
{
	if (cl.gametype == GAME_DEATHMATCH)
		Sbar_DeathmatchOverlay ();
}

void Sbar_DrawFrags (void)
{
	int				numscores, i, x, color;
	char			num[12];
	scoreboard_t	*s;

	Sbar_SortFrags ();

// draw the text
	numscores = Math_Min(scoreboardlines,4);

	for (i=0, x=184; i<numscores; i++, x+=32)
	{
		s = &cl.scores[fragsort[i]];
		if (!s->name[0])
			continue;

	// top color
		color = s->colors & 0xf0;
		color = Sbar_ColorForMap (color);

	// bottom color
		color = (s->colors & 15)<<4;
		color = Sbar_ColorForMap (color);

	// number
		sprintf (num, "%3i", s->frags);
		Sbar_DrawCharacter (x + 12, -24, num[0]);
		Sbar_DrawCharacter (x + 20, -24, num[1]);
		Sbar_DrawCharacter (x + 28, -24, num[2]);

	// brackets
		if (fragsort[i] == cl.viewentity - 1)
		{
			Sbar_DrawCharacter (x + 6, -24, 16);
			Sbar_DrawCharacter (x + 32, -24, 17);
		}
	}
}

void Sbar_IntermissionNumber (int x, int y, int num, int digits, int color)
{
#if 0
	char			str[12];
	char			*ptr;
	int				l, frame;

	l = Sbar_itoa (num, str);
	ptr = str;
	if (l > digits)
		ptr += (l-digits);
	if (l < digits)
		x += (digits-l)*24;

	while (*ptr)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		Draw_Pic (x,y,sb_nums[color][frame]); //johnfitz -- stretched menus
		x += 24;
		ptr++;
	}
#else
	Con_Printf("OBSOLETE: Sbar_IntermissionNumber\n");
#endif
}

void Sbar_DeathmatchOverlay (void)
{
	qpic_t			*pic;
	int				i, k, l;
	int				top, bottom;
	int				x, y, f;
	char			num[12];
	scoreboard_t	*s;

	GL_SetCanvas (CANVAS_MENU); //johnfitz

	pic = Draw_CachePic ("gfx/ranking.lmp");
	M_DrawPic ((320-pic->width)/2, 8, pic);

// scores
	Sbar_SortFrags ();

// draw the text
	l = scoreboardlines;

	x = 80; //johnfitz -- simplified becuase some positioning is handled elsewhere
	y = 40;
	for (i=0 ; i<l ; i++)
	{
		k = fragsort[i];
		s = &cl.scores[k];
		if (!s->name[0])
			continue;

	// draw background
		top = s->colors & 0xf0;
		bottom = (s->colors & 15)<<4;
		top = Sbar_ColorForMap (top);
		bottom = Sbar_ColorForMap (bottom);

	// draw number
		f = s->frags;
		sprintf (num, "%3i",f);

		Draw_Character ( x+8 , y, num[0]); //johnfitz -- stretched overlays
		Draw_Character ( x+16 , y, num[1]); //johnfitz -- stretched overlays
		Draw_Character ( x+24 , y, num[2]); //johnfitz -- stretched overlays

		if (k == cl.viewentity - 1)
			Draw_Character ( x - 8, y, 12); //johnfitz -- stretched overlays

#if 0
{
	int				total;
	int				n, minutes, tens, units;

	// draw time
		total = cl.completed_time - s->entertime;
		minutes = (int)total/60;
		n = total - minutes*60;
		tens = n/10;
		units = n%10;

		sprintf (num, "%3i:%i%i", minutes, tens, units);

		M_Print ( x+48 , y, num); //johnfitz -- was Draw_String, changed for stretched overlays
}

	// draw name
		M_Print (x+64, y, s->name); //johnfitz -- was Draw_String, changed for stretched overlays

		y += 10;
#endif
	}

	GL_SetCanvas (CANVAS_SBAR); //johnfitz
}

void Sbar_MiniDeathmatchOverlay (void)
{
	int				i, k,top, bottom, x, y, f, numlines;
	char			num[12];
	float			scale; //johnfitz
	scoreboard_t	*s;

	scale = CLAMP (1.0, scr_sbarscale.value, (float)glwidth / 320.0); //johnfitz

	//MAX_SCOREBOARDNAME = 32, so total width for this overlay plus sbar is 632, but we can cut off some i guess
	if (glwidth/scale < 512 || scr_viewsize.value >= 120) //johnfitz -- test should consider scr_sbarscale
		return;

// scores
	Sbar_SortFrags ();

// draw the text
	numlines = (scr_viewsize.value >= 110) ? 3 : 6; //johnfitz

	//find us
	for (i = 0; i < scoreboardlines; i++)
		if (fragsort[i] == cl.viewentity - 1)
			break;
	if (i == scoreboardlines) // we're not there
			i = 0;
	else // figure out start
			i = i - numlines/2;
	if (i > scoreboardlines - numlines)
			i = scoreboardlines - numlines;
	if (i < 0)
			i = 0;

	x = 324;
	y = (scr_viewsize.value >= 110) ? 24 : 0; //johnfitz -- start at the right place
	for ( ; i < scoreboardlines && y <= 48; i++, y+=8) //johnfitz -- change y init, test, inc
	{
		k = fragsort[i];
		s = &cl.scores[k];
		if (!s->name[0])
			continue;

	// colors
		top = s->colors & 0xf0;
		bottom = (s->colors & 15)<<4;
		top = Sbar_ColorForMap (top);
		bottom = Sbar_ColorForMap (bottom);

//		Draw_Fill ( x, y+1, 40, 4, top, 1);
//		Draw_Fill ( x, y+5, 40, 3, bottom, 1);

	// number
		f = s->frags;
		sprintf (num, "%3i",f);
		Draw_Character ( x+8 , y, num[0]);
		Draw_Character ( x+16 , y, num[1]);
		Draw_Character ( x+24 , y, num[2]);

	// brackets
		if (k == cl.viewentity - 1)
		{
			Draw_Character ( x, y, 16);
			Draw_Character ( x+32, y, 17);
		}

	// name
		R_DrawString(x+48,y,s->name);
	}
}

void Sbar_IntermissionOverlay (void)
{
#if 0
	qpic_t	*pic;
	int		dig;
	int		num;

	if (cl.gametype == GAME_DEATHMATCH)
	{
		Sbar_DeathmatchOverlay ();
		return;
	}

	GL_SetCanvas (CANVAS_MENU); //johnfitz

	pic = Draw_CachePic ("gfx/complete.lmp");
	Draw_Pic (64, 24, pic);

	pic = Draw_CachePic ("gfx/inter.lmp");
	Draw_Pic (0, 56, pic);

	dig = cl.completed_time/60;
	Sbar_IntermissionNumber (152, 64, dig, 3, 0); //johnfitz -- was 160
	num = cl.completed_time - dig*60;
	Draw_Pic (224,64,sb_colon); //johnfitz -- was 234
	Draw_Pic (240,64,sb_nums[0][num/10]); //johnfitz -- was 246
	Draw_Pic (264,64,sb_nums[0][num%10]); //johnfitz -- was 266

	Sbar_IntermissionNumber (152, 104, cl.stats[STAT_SECRETS], 3, 0); //johnfitz -- was 160
	Draw_Pic (224,104,sb_slash); //johnfitz -- was 232
	Sbar_IntermissionNumber (240, 104, cl.stats[STAT_TOTALSECRETS], 3, 0); //johnfitz -- was 248

	Sbar_IntermissionNumber (152, 144, cl.stats[STAT_MONSTERS], 3, 0); //johnfitz -- was 160
	Draw_Pic (224,144,sb_slash); //johnfitz -- was 232
	Sbar_IntermissionNumber (240, 144, cl.stats[STAT_TOTALMONSTERS], 3, 0); //johnfitz -- was 248
#else
	Con_Printf("OBSOLTE: Sbar_IntermissionOverlay\n");
#endif
}