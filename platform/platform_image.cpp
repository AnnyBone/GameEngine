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

#include "platform_image.h"
#include "platform_filesystem.h"

#if 0
PLresult plLoadImage(const PLchar *path, PLImage *out)
{
	plFunctionStart();

	if (path[0] == ' ')
		return PL_RESULT_FILEREAD;
	
	// Xenon uses a lot of long extensions, as do some other modern
	// applications, so that's why we're using a size 16.
	PLchar extension[16] = { 0 };
	plGetFileExtension(extension, path);
	if (extension[0] == ' ')
	{
		// This is the slowest loader type, now we need to take a stab
		// at which format this file potentially is using some trickery
		// but it's useful for cases in which we don't care so much about
		// the type of file we're loading.

		if (plFileExists(path))
		{
			// Apparently it exists without an extension... Ho boy...

			
		}
		
		std::string full_name = path;
	}
	else
	{
		if ((extension[0] == 'd') || (extension[0] == 'D'))
		{
			if (strncmp(extension, PLIMAGE_EXTENSION_DTX, 3)) {}
		}
		else if((extension[0] == 'f') || (extension[0] == 'F'))
		{ 
			if (strncmp(extension, PLIMAGE_EXTENSION_FTX, 3)) {}
		}
		else if((extension[0] == 'p') || (extension[0] == 'P'))
		{ 
			if (strncmp(extension, PLIMAGE_EXTENSION_PNG, 3)) {}
		}
	}

	plFunctionEnd();
}
#endif
