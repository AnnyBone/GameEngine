/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

static PL_INLINE PLuint32 GetBuildNumber(void) {
    static PLuint32 buildnum = 0;
    if (buildnum == 0)
        buildnum = (PLuint32) -floor(
                difftime(
                        plStringToTime("Jun 1 2011"),
                        plStringToTime(__DATE__)) / (60 * 60 * 24)
        );
    return buildnum;
}

#define ENGINE_VERSION_BUILD GetBuildNumber()