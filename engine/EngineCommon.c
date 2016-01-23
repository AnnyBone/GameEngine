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

#include "EngineScript.h"

#include "platform_filesystem.h"

/*
	Misc functions used in client and server.
*/

#define NUM_SAFE_ARGVS  7

static char *largv[MAX_NUM_ARGVS + NUM_SAFE_ARGVS + 1], *argvdummy = " ";
static char *safeargvs[NUM_SAFE_ARGVS] =
	{"-stdvid", "-nolan", "-nosound", "-nocdaudio", "-nojoy", "-nomouse", "-dibonly"};

bool msg_suppress_1 = 0;

// if a packfile directory differs from this, it is assumed to be hacked
#define PAK0_COUNT              339
#define PAK0_CRC                32981

char	com_token[1024];
int		com_argc;
char	**com_argv;
char	com_cmdline[CMDLINE_LENGTH];

/*


All of Quake's data access is through a hierchal file system, but the contents
of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the quake.exe and all
game directories.  The sys_* files pass this to host_init in quakeparms_t->basedir.
This can be overridden with the "-basedir" command line parm to allow code
debugging in a different directory.  The base directory is only used during
filesystem initialization.

The "game directory" is the first tree on the search path and directory that all
generated files (savegames, screenshots, demos, config files) will be saved to.
This can be overridden with the "-game" command line parameter.  The game
directory can never be changed while quake is executing.  This is a precacution
against having a malicious server instruct clients to write files over areas they
shouldn't.

The "cache directory" is only used during development to save network bandwidth,
especially over ISDN / T1 lines.  If there is a cache directory specified, when
a file is found by the normal search path, it will be mirrored into the cache
directory, then opened there.

FIXME:
The file "parms.txt" will be read out of the game directory and appended to the
current command line arguments to allow different games to initialize startup
parms differently.  This could be used to add a "-sspeed 22050" for the high
quality sound edition.  Because they are added at the end, they will not
override an explicit setting on the original command line.

*/

/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

int Q_atoi (char *str)
{
	int             val;
	int             sign;
	int             c;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

//
// check for hex
//
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X') )
	{
		str += 2;
		for(;;)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val<<4) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val<<4) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val<<4) + c - 'A' + 10;
			else
				return val*sign;
		}
	}

	// Check for character
	if (str[0] == '\'')
		return sign * str[1];

//
// assume decimal
//
	for(;;)
	{
		c = *str++;
		if (c <'0' || c > '9')
			return val*sign;
		val = val*10 + c - '0';
	}
}

float Q_atof (char *str)
{
	double			val;
	int             sign;
	int             c;
	int             decimal, total;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

	// Check for hex
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X') )
	{
		str += 2;
		for(;;)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val*16) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val*16) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val*16) + c - 'A' + 10;
			else
				return val*sign;
		}
	}

	// Check for character
	if(str[0] == '\'')
		return sign*str[1];

	// Assume decimal
	decimal = -1;
	total = 0;
	for(;;)
	{
		c = *str++;
		if (c == '.')
		{
			decimal = total;
			continue;
		}
		if (c <'0' || c > '9')
			break;
		val = val*10 + c - '0';
		total++;
	}

	if (decimal == -1)
		return val*sign;
	while (total > decimal)
	{
		val /= 10;
		total--;
	}

	return val*sign;
}

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

bool    bigendien;

short   (*BigShort)(short l);
int     (*BigLong)(int l);
int     (*LittleLong)(int l);
float   (*LittleFloat)(float l);
float   (*BigFloat)(float l);
short   (*LittleShort)(short l);

short   ShortSwap (short l)
{
	uint8_t b1, b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

short   ShortNoSwap (short l)
{
	return l;
}

int    LongSwap (int l)
{
	uint8_t b1, b2, b3, b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

int LongNoSwap(int l)
{
	return l;
}

float FloatSwap (float f)
{
	union
	{
		float   f;
		uint8_t b[4];
	} dat1, dat2;

	dat1.f      = f;
	dat2.b[0]   = dat1.b[3];
	dat2.b[1]   = dat1.b[2];
	dat2.b[2]   = dat1.b[1];
	dat2.b[3]   = dat1.b[0];

	return dat2.f;
}

float FloatNoSwap (float f)
{
	return f;
}

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

//
// writing functions
//

void MSG_WriteChar (sizebuf_t *sb, int c)
{
	uint8_t    *buf;

#ifdef PARANOID
	if (c < -128 || c > 127)
		Sys_Error ("MSG_WriteChar: range error");
#endif

	buf = (uint8_t*)SZ_GetSpace(sb, 1);
	buf[0] = c;
}

void MSG_WriteByte (sizebuf_t *sb, int c)
{
	uint8_t    *buf;

#ifdef PARANOID
	if (c < 0 || c > 255)
		Sys_Error ("MSG_WriteByte: range error");
#endif

	buf = (uint8_t*)SZ_GetSpace(sb, 1);
	buf[0] = c;
}

void MSG_WriteShort (sizebuf_t *sb, int c)
{
	uint8_t    *buf;

#ifdef PARANOID
	if (c < ((short)0x8000) || c > (short)0x7fff)
		Sys_Error ("MSG_WriteShort: range error");
#endif

	buf = (uint8_t*)SZ_GetSpace(sb, 2);
	buf[0] = c&0xff;
	buf[1] = c>>8;
}

void MSG_WriteLong (sizebuf_t *sb, int c)
{
	uint8_t    *buf;

	buf = (uint8_t*)SZ_GetSpace(sb, 4);
	buf[0] = c&0xff;
	buf[1] = (c>>8)&0xff;
	buf[2] = (c>>16)&0xff;
	buf[3] = c>>24;
}

void MSG_WriteFloat (sizebuf_t *sb, float f)
{
	union
	{
		float   f;
		int     l;
	} dat;

	dat.f = f;
	dat.l = LittleLong (dat.l);

	SZ_Write (sb, &dat.l, 4);
}

void MSG_WriteString (sizebuf_t *sb, const char *s)
{
	if (!s)
		SZ_Write (sb, "", 1);
	else
		SZ_Write (sb, s, strlen(s) + 1);
}

//johnfitz -- original behavior, 13.3 fixed point coords, max range +-4096
void MSG_WriteCoord16 (sizebuf_t *sb, float f)
{
	MSG_WriteShort (sb, pMath_RINT(f*8));
}

//johnfitz -- 16.8 fixed point coords, max range +-32768
void MSG_WriteCoord24 (sizebuf_t *sb, float f)
{
	MSG_WriteShort (sb, f);
	MSG_WriteByte (sb, (int)(f*255)%255);
}

//johnfitz -- 32-bit float coords
void MSG_WriteCoord32f (sizebuf_t *sb, float f)
{
	MSG_WriteFloat (sb, f);
}

void MSG_WriteCoord (sizebuf_t *sb, float f)
{
#if 0
	MSG_WriteCoord16 (sb, f);
#else
	MSG_WriteCoord32f(sb, f);
#endif
}

void MSG_WriteAngle (sizebuf_t *sb, float f)
{
	MSG_WriteByte (sb, pMath_RINT(f * 256.0 / 360.0) & 255); //johnfitz -- use pMath_RINT instead of (int)
}

//johnfitz -- for PROTOCOL_FITZQUAKE
void MSG_WriteAngle16 (sizebuf_t *sb, float f)
{
	MSG_WriteShort (sb, pMath_RINT(f * 65536.0 / 360.0) & 65535);
}
//johnfitz

//
// reading functions
//
int		msg_readcount;
bool	msg_badread;

void MSG_BeginReading (void)
{
	msg_readcount = 0;
	msg_badread = false;
}

// returns -1 and sets msg_badread if no more characters are available
int MSG_ReadChar (void)
{
	int     c;

	if (msg_readcount+1 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (signed char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int MSG_ReadByte (void)
{
	int     c;

	if (msg_readcount+1 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (unsigned char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int MSG_ReadShort (void)
{
	int     c;

	if (msg_readcount+2 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (short)(net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount+1]<<8));

	msg_readcount += 2;

	return c;
}

int MSG_ReadLong (void)
{
	int     c;

	if (msg_readcount+4 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount+1]<<8)
	+ (net_message.data[msg_readcount+2]<<16)
	+ (net_message.data[msg_readcount+3]<<24);

	msg_readcount += 4;

	return c;
}

float MSG_ReadFloat (void)
{
	union
	{
		uint8_t b[4];
		float   f;
		int     l;
	} dat;

	dat.b[0] =      net_message.data[msg_readcount];
	dat.b[1] =      net_message.data[msg_readcount+1];
	dat.b[2] =      net_message.data[msg_readcount+2];
	dat.b[3] =      net_message.data[msg_readcount+3];
	msg_readcount += 4;

	dat.l = LittleLong (dat.l);

	return dat.f;
}

char *MSG_ReadString (void)
{
	static char     string[2048];
	int             l,c;

	l = 0;
	do
	{
		c = MSG_ReadChar ();
		if (c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);

	string[l] = 0;

	return string;
}

//johnfitz -- original behavior, 13.3 fixed point coords, max range +-4096
float MSG_ReadCoord16(void)
{
	return MSG_ReadShort()*(1.0/8);
}

//johnfitz -- 16.8 fixed point coords, max range +-32768
float MSG_ReadCoord24 (void)
{
	return MSG_ReadShort() + MSG_ReadByte() * (1.0/255);
}

//johnfitz -- 32-bit float coords
float MSG_ReadCoord32f (void)
{
	return MSG_ReadFloat();
}

float MSG_ReadCoord (void)
{
#if 0
	return MSG_ReadCoord16();
#else
	return MSG_ReadCoord32f();
#endif
}

float MSG_ReadAngle (void)
{
	return MSG_ReadChar()*(360.0/256);
}

//johnfitz -- for PROTOCOL_FITZQUAKE
float MSG_ReadAngle16 (void)
{
	return MSG_ReadShort()*(360.0/65536);
}
//johnfitz

//===========================================================================

void SZ_Alloc (sizebuf_t *buf, int startsize)
{
	if (startsize < 256)
		startsize = 256;

	buf->data = (uint8_t*)Hunk_AllocName(startsize, "sizebuf");
	buf->maxsize	= startsize;
	buf->cursize	= 0;
}

void SZ_Clear (sizebuf_t *buf)
{
	buf->cursize = 0;
}

void *SZ_GetSpace (sizebuf_t *buf, int length)
{
	void *data;

	if (buf->cursize + length > buf->maxsize)
	{
		if (!buf->allowoverflow)
			Sys_Error ("SZ_GetSpace: overflow without allowoverflow set (%i)\n",buf->cursize+length);

		if (length > buf->maxsize)
			Sys_Error ("SZ_GetSpace: %i is > full buffer size", length);

		buf->overflowed = true;
		Con_Printf ("SZ_GetSpace: overflow");
		SZ_Clear (buf);
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;

	return data;
}

void SZ_Write (sizebuf_t *buf, const void *data, int length)
{
	memcpy(SZ_GetSpace(buf,length),data,length);
}

void SZ_Print (sizebuf_t *buf, char *data)
{
	int             len;

	len = strlen(data)+1;

	// byte * cast to keep VC++ happy
	if (buf->data[buf->cursize-1])
		memcpy((uint8_t*)SZ_GetSpace(buf, len), data, len); // no trailing 0
	else
		memcpy((uint8_t*)SZ_GetSpace(buf, len - 1) - 1, data, len); // write over trailing 0
}

char *FileSystem_SkipPath(char *pathname)
{
	char    *last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname=='/')
			last = pathname+1;
		pathname++;
	}
	return last;
}

void COM_StripExtension (char *in, char *out)
{
	while (*in && *in != '.')
		*out++ = *in++;
	*out = 0;
}

char *COM_FileExtension(char *in)
{
	static char exten[8];
	int			i;

	while (*in && *in != '.')
		in++;
	if (!*in)
		return "";
	in++;
	for (i=0 ; i<7 && *in ; i++,in++)
		exten[i] = *in;
	exten[i] = 0;
	return exten;
}

void COM_FileBase(const char *in,char *out)
{
	char *s,*s2, cBuf[PLATFORM_MAX_PATH];

	strcpy(cBuf, in);

	s = cBuf + strlen(cBuf) - 1;

	while (s != in && *s != '.')
		s--;

	for (s2 = s ; *s2 && *s2 != '/' ; s2--)
	;

	if (s-s2 < 2)
		strcpy(out, "?model?");
	else
	{
		s--;
		strncpy(out, s2 + 1, s - s2);
		out[s-s2] = 0;
	}
}

void COM_DefaultExtension (char *path, char *extension)
{
	char    *src;
//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path+strlen(path)-1;

	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;                 // it has an extension
		src--;
	}

	strcat (path, extension);
}

/*	Returns the position (1 to argc-1) in the program's argument list
	where the given parameter apears, or 0 if not present
*/
int COM_CheckParm (char *parm)
{
	int i;

	for (i=1 ; i<com_argc ; i++)
	{
		if(!com_argv[i])
			continue;               // NEXTSTEP sometimes clears appkit vars.

		if(!strcmp (parm,com_argv[i]))
			return i;
	}

	return 0;
}

void _FileSystem_Path(void);

void COM_InitArgv (int argc, char **argv)
{
	bool			safe;
	int				i, j, n;

// reconstitute the command line for the cmdline externally visible cvar
	n = 0;

	for (j=0 ; (j<MAX_NUM_ARGVS) && (j< argc) ; j++)
	{
		i = 0;

		while ((n < (CMDLINE_LENGTH - 1)) && argv[j][i])
			com_cmdline[n++] = argv[j][i++];

		if (n < (CMDLINE_LENGTH - 1))
			com_cmdline[n++] = ' ';
		else
			break;
	}

	// [11/8/2013] Brought in from QuakeSpasm ~hogsy
	if(n > 0 && com_cmdline[n-1] == ' ')
		com_cmdline[n-1] = 0; //johnfitz -- kill the trailing space

	safe = false;

	for (com_argc=0 ; (com_argc<MAX_NUM_ARGVS) && (com_argc < argc) ; com_argc++)
	{
		largv[com_argc] = argv[com_argc];
		if (!strcmp ("-safe", argv[com_argc]))
			safe = true;
	}

	if (safe)
	{
	// force all the safe-mode switches. Note that we reserved extra space in
	// case we need to add these, so we don't need an overflow check
		for (i=0 ; i<NUM_SAFE_ARGVS ; i++)
		{
			largv[com_argc] = safeargvs[i];
			com_argc++;
		}
	}

	largv[com_argc] = argvdummy;
	com_argv = largv;
}

void COM_Init(void)
{
	uint8_t bSwapTest[2] =
		{	1,	0	};

	// set the byte swapping variables in a portable manner
	if(*(short*)bSwapTest == 1)
	{
		bigendien	= false;
		BigShort	= ShortSwap;
		LittleShort = ShortNoSwap;
		BigLong		= LongSwap;
		LittleLong	= LongNoSwap;
		BigFloat	= FloatSwap;
		LittleFloat = FloatNoSwap;
	}
	else
	{
		bigendien	= true;
		BigShort	= ShortNoSwap;
		LittleShort = ShortSwap;
		BigLong		= LongNoSwap;
		LittleLong	= LongSwap;
		BigFloat	= FloatNoSwap;
		LittleFloat = FloatSwap;
	}

	Cmd_AddCommand("path",_FileSystem_Path);
}

/*	does a varargs printf into a temp buffer, so I don't need to have
	varargs versions of all text functions.
	FIXME: make this buffer size safe someday
*/
char *va(const char *format, ...)
{
	va_list		argptr;
	static char	string[1024];

	va_start (argptr, format);
	vsprintf (string, format, argptr);
	va_end (argptr);

	return string;
}

/*
=============================================================================

QUAKE FILESYSTEM

=============================================================================
*/

int     com_filesize;


//
// in memory
//

typedef struct
{
	char    name[MAX_QPATH];
	int             filepos, filelen;
} packfile_t;

typedef struct pack_s
{
	char			filename[PLATFORM_MAX_PATH];
	int             handle;
	int             numfiles;
	packfile_t      *files;
} pack_t;

//
// on disk
//
typedef struct
{
	char    name[56];
	int             filepos, filelen;
} dpackfile_t;

typedef struct
{
	char    id[4];
	int             dirofs;
	int             dirlen;
} dpackheader_t;

#define MAX_FILES_IN_PACK       2048

char    com_cachedir[PLATFORM_MAX_PATH];
char    com_gamedir[PLATFORM_MAX_PATH];

typedef struct searchpath_s
{
	char    filename[PLATFORM_MAX_PATH];
	pack_t  *pack;          // only one of filename / pack will be used
	struct searchpath_s *next;
} searchpath_t;

searchpath_t    *com_searchpaths;
void _FileSystem_Path(void)
{
	searchpath_t    *s;

	Con_Printf ("Current search path:\n");
	for (s=com_searchpaths ; s ; s=s->next)
	{
		if (s->pack)
			Con_Printf("%s (%i files)\n",s->pack->filename,s->pack->numfiles);
		else
			Con_Printf("%s\n",s->filename);
	}
}

/*  Switches the given path to lowercase (solves issues on Linux).
*/
void FileSystem_UpdatePath(char cPath[])
{
	int i, iLength;

	iLength = strlen(cPath);
	for (i = 0; i < iLength; i++)
	{
		// Probably the end, or a messed up path.
		if (cPath[i] == ' ')
			break;
		// Switch it around.
		cPath[i] = tolower(cPath[i]);
	}
}

/*	The filename will be prefixed by the current game directory.
*/
void FileSystem_WriteFile(const char *ccFileName,void *data,int len)
{
	int		handle;
	char    name[PLATFORM_MAX_PATH];

	if (!plCreateDirectory(com_gamedir)) //johnfitz -- if we've switched to a nonexistant gamedir, create it now so we don't crash
		Sys_Error("Failed to create directory!\n");

	sprintf (name,"%s/%s",com_gamedir,ccFileName);

	handle = Sys_FileOpenWrite (name);
	if (handle == -1)
	{
		Sys_Printf ("COM_WriteFile: failed on %s\n", name);
		return;
	}

	Sys_Printf ("COM_WriteFile: %s\n", name);
	Sys_FileWrite (handle, data, len);
	Sys_FileClose (handle);
}

void FileSystem_CreatePath(char *path)
{
	char *ofs;

	for (ofs = path+1 ; *ofs ; ofs++)
	{
		if(*ofs == '/')
		{
			// create the directory
			*ofs = 0;
			if (!plCreateDirectory(path))
				Sys_Error("Failed to create directory!\n");
			*ofs = '/';
		}
	}
}

/*	Copies a file over from the net to the local cache, creating any directories
	needed.  This is for the convenience of developers using ISDN from home.
*/
void FileSystem_CopyFile(char *netpath,char *cachepath)
{
	int		in, out;
	int		remaining, count;
	char    buf[4096];

	remaining = Sys_FileOpenRead (netpath, &in);
	FileSystem_CreatePath(cachepath);     // create directories up to the cache file
	out = Sys_FileOpenWrite (cachepath);

	while (remaining)
	{
		if (remaining < sizeof(buf))
			count = remaining;
		else
			count = sizeof(buf);
		Sys_FileRead (in, buf, count);
		Sys_FileWrite (out, buf, count);
		remaining -= count;
	}

	Sys_FileClose(in);
	Sys_FileClose(out);
}

/*	Finds the file in the search path.
	Sets com_filesize and one of handle or file
*/
int COM_FindFile (const char *filename, int *handle, FILE **file)
{
	pack_t			*pak;
	searchpath_t    *search;
	char            netpath[PLATFORM_MAX_PATH];
	int             i;

	if(!file && !handle)
	{
		Sys_Error ("COM_FindFile: neither handle or file set");
		return 0;
	}
	else if(file && handle)
	{
		Sys_Error ("COM_FindFile: both handle and file set");
		return 0;
	}

	// search through the path, one element at a time
	search = com_searchpaths;
	for ( ; search ; search = search->next)
	{
		// is the element a pak file?
		if (search->pack)
		{
			// look through all the pak file elements
			pak = search->pack;
			for (i=0 ; i<pak->numfiles ; i++)
				if (!strcmp (pak->files[i].name, filename))
				{
					// found it!
					Sys_Printf ("PackFile: %s : %s\n",pak->filename, filename);
					if (handle)
					{
						*handle = pak->handle;
						Sys_FileSeek (pak->handle, pak->files[i].filepos);
					}
					else
					{       // open a new file on the pakfile
						*file = fopen (pak->filename, "rb");
						if (*file)
							fseek (*file, pak->files[i].filepos, SEEK_SET);
					}
					com_filesize = pak->files[i].filelen;
					return com_filesize;
				}
		}
		else
		{
			sprintf(netpath, "%s/%s", search->filename, filename);

			// Check if the file exists or not.
			if (!plFileExists(netpath))
				continue;

			Sys_Printf ("FindFile: %s\n",netpath);
			com_filesize = Sys_FileOpenRead (netpath, &i);
			if (handle)
				*handle = i;
			else
			{
				Sys_FileClose (i);
				*file = fopen (netpath, "rb");
			}
			return com_filesize;
		}
	}

	Sys_Printf ("FindFile: can't find %s\n", filename);

	if (handle)
		*handle = -1;
	else
		*file = NULL;
	com_filesize = -1;
	return -1;
}

/*	filename never has a leading slash, but may contain directory walks
	returns a handle and a length
	it may actually be inside a pak file
*/
int COM_OpenFile (const char *filename, int *handle)
{
	return COM_FindFile (filename, handle, NULL);
}

/*	If the requested file is inside a packfile, a new FILE * will be opened
	into the file.
*/
int COM_FOpenFile (const char *filename, FILE **file)
{
	return COM_FindFile (filename, NULL, file);
}

/*	If it is a pak file handle, don't really close it
*/
void COM_CloseFile (int h)
{
	searchpath_t    *s;

	for (s = com_searchpaths ; s ; s=s->next)
		if (s->pack && s->pack->handle == h)
			return;

	Sys_FileClose (h);
}

/*	Filename are relative to the quake directory.
	Allways appends a 0 byte.
*/
static uint8_t *COM_LoadFile(const char *path, int usehunk)
{
	int     h,len;
	uint8_t	*buf = NULL;  // quiet compiler warning
	char    base[PLATFORM_MAX_PATH];

	// look for it in the filesystem or pack files
	len = COM_OpenFile (path, &h);
	if (h == -1)
		return NULL;

	// extract the filename base name for hunk tag
	COM_FileBase (path, base);

	if (usehunk == 1)
		buf = (uint8_t*)Hunk_AllocName(len + 1, base);
	else if (usehunk == 2)
		buf = (uint8_t*)Hunk_TempAlloc(len + 1);
	else if (usehunk == 0)
		buf = (uint8_t*)malloc_or_die(len + 1);
	else
		Sys_Error ("COM_LoadFile: bad usehunk");

	if(!buf)
	{
		Sys_Error ("COM_LoadFile: not enough space for %s", path);
		return NULL;
	}

	((uint8_t*)buf)[len] = 0;

	Draw_BeginDisc();

	Sys_FileRead(h, buf, len);

	COM_CloseFile(h);

	return buf;
}

uint8_t *COM_LoadHunkFile(char *path)
{
	return COM_LoadFile(path,1);
}

uint8_t *COM_LoadTempFile(const char *path)
{
	return COM_LoadFile (path,2);
}

void *COM_LoadHeapFile(const char *path)
{
	return COM_LoadFile(path, 0);
}

/*	johnfitz -- modified based on topaz's tutorial

	Takes an explicit (not game tree related) path to a pak file.

	Loads the header and directory, adding the files at the beginning
	of the list so they override previous pack files.
*/
pack_t *FileSystem_LoadPackage(char *packfile)
{
	dpackheader_t   header;
	int                             i;
	packfile_t              *newfiles;
	int                             numpackfiles;
	pack_t                  *pack;
	int                             packhandle;
	dpackfile_t             *info;
	unsigned short          crc;

	if (Sys_FileOpenRead (packfile, &packhandle) == -1)
		return NULL;
	Sys_FileRead (packhandle, (void *)&header, sizeof(header));
	if (header.id[0] != 'P' || header.id[1] != 'A' || header.id[2] != 'C' || header.id[3] != 'K')
		Sys_Error ("%s is not a packfile", packfile);
	header.dirofs = LittleLong (header.dirofs);
	header.dirlen = LittleLong (header.dirlen);

	numpackfiles = header.dirlen / sizeof(dpackfile_t);

	if (numpackfiles > MAX_FILES_IN_PACK)
		Sys_Error ("%s has %i files", packfile, numpackfiles);

	//johnfitz -- dynamic gamedir loading
	newfiles = (packfile_t*)calloc_or_die(numpackfiles, sizeof(packfile_t));
	//johnfitz

	info = (dpackfile_t*)malloc(MAX_FILES_IN_PACK);
	if (!info)
		Sys_Error("Failed to allocate array!\n");

	Sys_FileSeek (packhandle, header.dirofs);
	Sys_FileRead (packhandle, (void *)info, header.dirlen);

	// crc the directory to check for modifications
	CRC_Init (&crc);
	for (i = 0; i < header.dirlen ; i++)
		CRC_ProcessByte(&crc, ((uint8_t *)info)[i]);

	// parse the directory
	for (i = 0; i < numpackfiles ; i++)
	{
#ifdef _MSC_VER
#pragma warning(suppress: 6011)	// We checked this already!
#endif
		strcpy(newfiles[i].name, info[i].name);
		newfiles[i].filepos = LittleLong(info[i].filepos);
		newfiles[i].filelen = LittleLong(info[i].filelen);
	}

	//johnfitz -- dynamic gamedir loading
	pack = (pack_t*)malloc_or_die(sizeof(pack_t));
	//johnfitz

	strcpy(pack->filename, packfile);
	pack->handle = packhandle;
	pack->numfiles = numpackfiles;
	pack->files = newfiles;

	free(info);

	return pack;
}

void FileSystem_AddPackage(char *file)
{
	searchpath_t    *search;
	pack_t			*pak;

	pak = FileSystem_LoadPackage(file);
	if (pak)
	{
		search = (searchpath_t*)calloc_or_die(1, sizeof(searchpath_t));
		search->pack = pak;
		search->next = com_searchpaths;
		com_searchpaths = search;
	}
}

void FileSystem_AddGameDirectory(char *dir)
{
	searchpath_t	*search;

	strcpy(com_gamedir, dir);

	// add the directory to the search path
	search = (searchpath_t*)calloc_or_die(1, sizeof(searchpath_t));
	strncpy(search->filename, dir, sizeof(search->filename));
	search->next = com_searchpaths;
	com_searchpaths = search;

	// Also scan for ye old packages.
	plScanDirectory(dir, ".pak", FileSystem_AddPackage);
}

/*
	Script Functions
*/

/*	Script specific function that sets the base data path.
	SetBasePath
*/
void _FileSystem_SetBasePath(char *cArg)
{
	strncpy(host_parms.basepath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetMaterialPath(char *cArg)
{
	strncpy(g_state.cMaterialPath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetTexturePath(char *cArg)
{
	strncpy(g_state.cTexturePath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetSoundPath(char *cArg)
{
	strncpy(g_state.cSoundPath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetLevelPath(char *cArg)
{
	strncpy(g_state.cLevelPath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetFontPath(char *cArg)
{
	strncpy(g_state.cFontPath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetScreenshotPath(char *cArg)
{
	strncpy(g_state.cScreenshotPath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetModulePath(char *cArg)
{
	strncpy(g_state.cModulePath, cArg, PLATFORM_MAX_PATH);
}

void _FileSystem_SetShaderPath(char *cArg)
{
	strncpy(g_state.cShaderPath, cArg, PLATFORM_MAX_PATH);
}

/*	Script specific function that adds a new data path.
	AddGameDirectory
*/
void _FileSystem_AddGameDirectory(char *cArg)
{
	FileSystem_AddGameDirectory(va("%s/%s",host_parms.basedir,cArg));
}

/**/

void FileSystem_Initialize(void)
{
	int				i,j;
	char			basedir[PLATFORM_MAX_PATH];
	searchpath_t	*search;

	COM_Init();

	i = COM_CheckParm ("-basedir");
	if (i && i < com_argc-1)
		strncpy(basedir, com_argv[i + 1], sizeof(basedir));
	else
		strncpy(basedir, host_parms.basedir, sizeof(basedir));

#ifdef _MSC_VER
#pragma warning(suppress: 6053)	// This isn't an issue in this case.
#endif
	j = strnlen(basedir,sizeof(basedir));
	if (j > 0)
		if ((basedir[j-1] == '\\') || (basedir[j-1] == '/'))
			basedir[j-1] = 0;

	i = COM_CheckParm ("-cachedir");
	if (i && i < com_argc-1)
	{
		if (com_argv[i+1][0] == '-')
			com_cachedir[0] = 0;
		else
			strcpy(com_cachedir, com_argv[i + 1]);
	}
	else if (host_parms.cachedir)
		strncpy(com_cachedir, host_parms.cachedir, sizeof(com_cachedir));
	else
		com_cachedir[0] = 0;

	// Add the engine directory to our search paths.
	FileSystem_AddGameDirectory(va("%s/engine", basedir));

	// Check out our paths script.
	if(!Script_Load("paths.script"))
		Sys_Error("Failed to load paths script!\n");

	// Check each of our set paths to make sure they're set!
	if(basedir[0] == ' ')
		// TODO: Close down in this instance, though I might change so we try to load "base" instead? ~hogsy
		Sys_Error("Base path wasn't set in paths script!\n");

	if (g_state.cLevelPath[0] == ' ')
	{
		Con_Warning("Levels path wasn't set in paths script!\n");
		sprintf(g_state.cLevelPath, "levels/");
	}

	if (g_state.cMaterialPath[0] == ' ')
	{
		Con_Warning("Materials path wasn't set in paths script!\n");
		sprintf(g_state.cMaterialPath, "materials/");
	}

	if (g_state.cScreenshotPath[0] == ' ')
	{
		Con_Warning("Screenshots path wasn't set in paths script!\n");
		sprintf(g_state.cScreenshotPath, "screenshots/");
	}

	if (g_state.cSoundPath[0] == ' ')
	{
		Con_Warning("Sounds path wasn't set in paths script!\n");
		sprintf(g_state.cSoundPath, "sounds/");
	}

	if (g_state.cFontPath[0] == ' ')
	{
		Con_Warning("Sounds path wasn't set in paths script!\n");
		sprintf(g_state.cFontPath, "fonts/");
	}

	if (g_state.cTexturePath[0] == ' ')
	{
		Con_Warning("Textures path wasn't set in paths script!\n");
		sprintf(g_state.cTexturePath, "textures/");
	}

	if (g_state.cShaderPath[0] == ' ')
	{
		Con_Warning("Shaders path wasn't set in paths script!\n");
		sprintf(g_state.cShaderPath, "shaders/");
	}

	// Start up with the default path
	FileSystem_AddGameDirectory(va("%s/%s", basedir, host_parms.basepath));
	strncpy(com_gamedir, va("%s/%s", basedir, host_parms.basepath), sizeof(com_gamedir));

	i = COM_CheckParm ("-game");
	if (i && i < com_argc-1)
		FileSystem_AddGameDirectory(va("%s/%s",basedir,com_argv[i+1]));

	i = COM_CheckParm ("-path");
	if(i)
	{
		com_searchpaths = NULL;

		while (++i < com_argc)
		{
			if (!com_argv[i] || com_argv[i][0] == '+' || com_argv[i][0] == '-')
				break;

			search = (searchpath_t*)Hunk_Alloc(sizeof(searchpath_t));
			if(!strcmp(COM_FileExtension(com_argv[i]), "pak") )
			{
				search->pack = FileSystem_LoadPackage(com_argv[i]);
				if (!search->pack)
					Sys_Error ("Couldn't load packfile: %s", com_argv[i]);
			}
			else
				strcpy(search->filename, com_argv[i]);

			search->next    = com_searchpaths;
			com_searchpaths = search;
		}
	}
}


