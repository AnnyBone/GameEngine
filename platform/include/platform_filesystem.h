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

plEXTERN_C_START

extern void	plGetUserName(PLchar *out);
extern void	plGetWorkingDirectory(PLchar *out);

extern void plStripExtension(PLchar *dest, const PLchar *in);
extern PLchar *plGetFileExtension(PLchar *dest, const PLchar *in);
extern const PLchar *plGetFileName(const PLchar *path);

extern void	plScanDirectory(const PLchar *path, const PLchar *extension, void(*Function)(PLchar *filepath));
extern void	plLowerCasePath(PLchar *out);

extern PLbool plCreateDirectory(const PLchar *path);

// File I/O ...

extern PLbool plFileExists(const PLchar *path);
extern PLbool plIsFileModified(time_t oldtime, const PLchar *path);

extern time_t plGetFileModifiedTime(const PLchar *path);

extern PLint plGetLittleShort(FILE *fin);
extern PLint plGetLittleLong(FILE *fin);

plEXTERN_C_END
