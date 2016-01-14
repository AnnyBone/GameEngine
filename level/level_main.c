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

#include "bsp5.h"
#include "threads.h"

// Command line flags
bool	nofill,
		notjunc,
		noclip,
		onlyents,
		verbose,
		allverbose,
		forcevis,
		waterlightmap;

int		subdivide_size;

int		valid;

char	filename_map[1024],
		cFilenameEntity[1024],
		filename_bsp[1024],
		filename_prt[1024],
		filename_pts[1024];

char	wadPath[PLATFORM_MAX_PATH];

int		Vis_Main( int argc, char **argv );
int		Light_Main( int argc, char **argv );
int		Bsp2Prt_Main( int argc, char **argv );
int		BspInfo_Main(void);

//===========================================================================

void qprintf( char *fmt, ... )
{
	va_list argptr;

	if(!verbose)
		return;		// only print if verbose

	va_start( argptr, fmt );
	vprintf( fmt, argptr );
	va_end( argptr );
}

//===========================================================================

void BSP_ProcessEntity (int entnum, int modnum, int hullnum)
{
	entity_t	*ent;
	tree_t		*tree;

	ent = &entities[entnum];
	if (!ent->brushes)
		return;

	tree = Tree_ProcessEntity (ent, hullnum);
	EmitNodePlanes (tree->headnode);

	if (hullnum != 0)
		EmitClipNodes (tree->headnode, modnum, hullnum);
	else
	{
		EmitNodeFaces (tree->headnode);
		EmitDrawNodes (tree->headnode);
	}

	FreeTree (tree);
}

void UpdateEntLump (void)
{
	int		m, entnum;
	char	mod[80];

	m = 1;
	for (entnum = 1 ; entnum < num_entities ; entnum++)
	{
		if (!entities[entnum].brushes)
			continue;
		sprintf (mod, "*%i", m);
		SetKeyValue (&entities[entnum], "model", mod);
		m++;
	}

	UnparseEntities();
}

void CreateSingleHull (int hullnum)
{
	int	entnum,modnum;

	// for each entity in the map file that has geometry
	verbose = true;	// print world
	for(entnum = 0,modnum = 0; entnum < num_entities; entnum++)
	{
		if(!entities[entnum].brushes)
			continue;

		BSP_ProcessEntity(entnum,modnum++,hullnum);
		if (!allverbose)
			verbose = false;	// don't print rest of entities
	}
}

void CreateHulls (void)
{
	int	i;

	// Hull 0 is always point-sized
	VectorClear(hullinfo.hullsizes[0][0]);
	VectorClear(hullinfo.hullsizes[0][1]);

	CreateSingleHull(0);

	// Commanded to ignore the hulls altogether
	if(noclip)
	{
		hullinfo.numhulls	=
		hullinfo.filehulls	= 1;
		return;
	}

	// create the hulls sequentially
	printf("building hulls sequentially...\n");

	hullinfo.numhulls	=
	hullinfo.filehulls	= BSP_MAX_HULLS;
	for (i = 1; i < BSP_MAX_HULLS; i++)
	{
		VectorCopy(vHullSizes[i][0], hullinfo.hullsizes[i][0]);
		VectorCopy(vHullSizes[i][1], hullinfo.hullsizes[i][1]);
	}

	for(i = 1; i < hullinfo.numhulls; i++)
		CreateSingleHull(i);
}

void ProcessFile(char *sourcebase)
{
	if (!onlyents)
	{
		remove(filename_bsp);
		remove(filename_prt);
		remove(filename_pts);
		remove(cFilenameEntity);
	}

	// load brushes and entities
	LoadMapFile(sourcebase);

	if(onlyents)
	{
		LoadBSPFile(filename_bsp);
		UpdateEntLump();
		WriteBSPFile(filename_bsp);
		return;
	}

	// init the tables to be shared by all models
	BeginBSPFile ();

	// the clipping hulls will be written out to text files by forked processes
	CreateHulls ();

	UpdateEntLump ();

	WriteMiptex ();

	FinishBSPFile ();
}

int main(int argc,char **argv)
{
	int		i;
	double	start, end;

	printf("Katana Level (based on hmap2 by LordHavoc and Vic)\n");
	printf("Compiled: "__DATE__" "__TIME__"\n");
	printf("\n" );

	// Check command line flags
	if(argc == 1)
		goto STARTERROR;

	// create all the filenames pertaining to this map
	strcpy(filename_map, argv[argc - 1]);
	ReplaceExtension(filename_map,BSP_EXTENSION, ".map", ".map");
	strcpy(filename_bsp, filename_map);
	ReplaceExtension(filename_bsp, ".map",BSP_EXTENSION,BSP_EXTENSION);
	strcpy(filename_prt, filename_bsp);
	ReplaceExtension(filename_prt,BSP_EXTENSION, ".prt", ".prt");
	strcpy(filename_pts, filename_bsp);
	ReplaceExtension(filename_pts,BSP_EXTENSION, ".pts", ".pts");
	strcpy(cFilenameEntity, filename_pts);
	ReplaceExtension(cFilenameEntity, ".pts", ".entity", ".entity");

	if(!strcmp(filename_map, filename_bsp))
		Error("filename_map \"%s\" == filename_bsp \"%s\"\n", filename_map, filename_bsp);

/*	for (i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i],"-threads"))
		{
			i++;
			if( i >= argc )
				Error( "no value was given to -numthreads\n" );
			numthreads = atoi (argv[i]);
		}
	}*/

	i = 1;
	if(argc == i)
		goto STARTERROR;

	if(!strcmp(argv[i],"-threads"))
	{
		if(i >= argc)
			Error("No value was given to -threads!\n");
		numthreads = atoi(argv[i+1]);
	}

	ThreadSetDefault();

	if (!strcmp (argv[i], "-bsp2prt"))
		return Bsp2Prt_Main (argc-i, argv+i);
	else if (!strcmp (argv[i], "-bspinfo"))
		return BspInfo_Main();
	else if (!strcmp (argv[i], "-vis"))
		return Vis_Main (argc-i, argv+i);
	else if (!strcmp (argv[i], "-light"))
		return Light_Main (argc-i, argv+i);

	nofill			= false;
	notjunc			= false;
	noclip			= false;
	onlyents		= false;
	verbose			= true;
	allverbose		= false;
	transwater		= true;
	forcevis		= true;
	waterlightmap	= true;
	subdivide_size	= 1024;

	for (; i < argc; i++)
	{
		if (argv[i][0] != '-')
			break;
		else if (!strcmp (argv[i],"-nowater"))
			transwater = false;
		else if (!strcmp (argv[i],"-notjunc"))
			notjunc = true;
		else if (!strcmp (argv[i],"-nofill"))
			nofill = true;
		else if (!strcmp (argv[i],"-noclip"))
			noclip = true;
		else if (!strcmp (argv[i],"-onlyents"))
			onlyents = true;
		else if (!strcmp (argv[i],"-verbose"))
			allverbose = true;
		else if (!strcmp (argv[i],"-nowaterlightmap"))
			waterlightmap = false;
		else if (!strcmp (argv[i],"-subdivide"))
		{
			subdivide_size = atoi(argv[i+1]);
			i++;
		}
		else if (!strcmp (argv[i],"-noforcevis"))
			forcevis = false;
		else if (!strcmp(argv[i], "-wadpath"))
		{ 
			sprintf(wadPath, "%s", argv[i + 1]);
			i++;
		}
		else
			Error("Unknown option '%s'",argv[i]);
	}

	if(i != argc-1)
STARTERROR:
		Error(	"%s",
				"usage: hmap2 [options] sourcefile\n"
				"Compiles .map to "BSP_EXTENSION", does not compile vis or lighting data\n"
				"\n"
				"other utilities available:\n"
				"-bsp2prt    bsp2prt utility, run -bsp2prt as the first parameter for more\n"
				"-bspinfo    bspinfo utility, run -bspinfo as the first parameter for more\n"
				"-light      lighting utility, run -light as the first parameter for more\n"
				"-vis        vis utility, run -vis as the first parameter for more\n"
				"\n"
				"What the options do:\n"
				"-wadpath (Sets the path to load WADs from.)\n"
				"-nowater			disable watervis; r_wateralpha in glquake will not work right\n"
				"-notjunc			disable tjunction fixing; glquake will have holes between polygons\n"
				"-nofill			disable sealing of map and vis, used for ammoboxes\n"
				"-onlyents			patchs entities in existing "BSP_EXTENSION", for relighting\n"
				"-verbose			show more messages\n"
				"-subdivide			allow really big polygons (e.g. -subdivide 1024)\n"
				"-noforcevis		don't make a .prt if the map leaks\n"
				"-nowaterlightmap	disable darkplaces lightmapped water feature\n"
				"-notex				store blank textures instead of real ones, smaller bsp if zipped\n");

	printf("Input:	%s\n", filename_map);
	printf("Output: %s\n", filename_bsp);

	// Init memory
	Q_InitMem ();

	// Do it!
	start = I_DoubleTime ();

	ProcessFile(filename_map);

	end = I_DoubleTime ();
	printf("%5.1f seconds elapsed\n\n",end-start);

	// Print memory stats
	Q_PrintMem();

	// Free allocated memory
	Q_ShutdownMem();

	return 0;
}
