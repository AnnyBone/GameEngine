/*	Copyright (C) 2015 OldTimes Software
*/
#include "stdafx.h"

#include "platform.h"
#include "platform_filesystem.h"

char cCurrentDirectory[PLATFORM_MAX_PATH];

void Generator_WriteFile(char *cFile)
{

}

int main(int argc, char *argv[])
{
	printf("Getting working directory...\n");

	pFileSystem_GetWorkingDirectory(cCurrentDirectory);

	printf("Scanning directory...\n");

	pFileSystem_ScanDirectory(cCurrentDirectory, ".material", Generator_WriteFile);

	return 0;
}

