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

#include <time.h>

#pragma once

// http://stackoverflow.com/questions/1765014/convert-string-from-date-into-a-time-t
static PL_INLINE time_t StringToTime(const char *ts)
{
	char s_month[5];
	int day, year;
	sscanf(ts, "%s %d %d", s_month, &day, &year);

	static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	int month = (strstr(months, s_month) - months) / 3;
	struct tm time = { 0 };
	time.tm_mon		= month;
	time.tm_mday	= day;
	time.tm_year	= year - 1900;
	time.tm_isdst	= -1;

	return mktime(&time);
}

static PL_INLINE unsigned int GetBuildNumber(void)
{
	return (unsigned int)-floor(difftime(StringToTime("Jun 1 2011"), StringToTime(__DATE__)) / (60 * 60 * 24));
}

//	Build is updated each day that work is done on the engine.
#define ENGINE_VERSION_BUILD GetBuildNumber()