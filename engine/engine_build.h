
#pragma once

static PL_INLINE unsigned int GetBuildNumber(void) {
    static unsigned int buildnum = 0;
    if (!buildnum) {
        buildnum = (unsigned int)std::floor(
                difftime(
                        plStringToTime("Jun 1 2011"),
                        plStringToTime(__DATE__)
                ) / (60 * 60 * 24)
        );
    }
    return buildnum;
}

#define ENGINE_VERSION_BUILD GetBuildNumber()