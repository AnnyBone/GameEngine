/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#pragma once

#include "platform.h"

typedef struct PLViewport {
    PLint x, y;

    PLuint width, height;

    struct PLViewport *parent, **children;

    PLuint num_children;
} PLViewport;

PL_EXTERN_C

PL_EXTERN void plSetupViewport(PLViewport *viewport, PLint x, PLint y, PLuint width, PLuint height);

PL_EXTERN void plSetViewportSize(PLViewport *viewport, PLuint width, PLuint height);
PL_EXTERN void plSetViewportPosition(PLViewport *viewport, PLint x, PLint y);

PL_EXTERN void plSetCurrentViewport(PLViewport *viewport);

PL_EXTERN void plScreenshot(PLViewport *viewport, const PLchar *path);

PL_EXTERN_C_END
