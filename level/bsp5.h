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

#ifndef __BSP5__
#define __BSP5__
// bsp5.h

#include "cmdlib.h"
#include "mathlib.h"
#include "bspfile.h"
#include "mem.h"
#include "winding.h"
#include "map.h"

#include "platform_log.h"

// the exact bounding box of the brushes is expanded some for the headnode
// volume.  is this still needed?
#define	SIDESPACE	24

#define MAX_MIPTEXFILES 32768				// Max number of miptex files we can load from a WAD

//============================================================================

typedef struct visfacet_s
{
	struct visfacet_s	*next;

	int					planenum;
	int					planeside;		// which side is the front of the face
	int					texturenum;
	int					contents[2];	// 0 = front side

	struct visfacet_s	*original;		// face on node
	int					outputnumber;	// only valid for original faces after write surfaces
	winding_t			*winding;
} face_t;

typedef struct surface_s
{
	struct surface_s	*next;

	int					planenum;
	vec3_t				mins, maxs;
	bool				onnode;			// true if surface has already been used as a splitting node
	face_t				*faces;			// links to all the faces on either side of the surf
} surface_t;

typedef struct brush_s
{
	struct brush_s		*next;

	vec3_t				mins, maxs;
	face_t				*faces;
	int					contents;
} brush_t;

//
// there is a node_t structure for every node and leaf in the bsp tree
//
#define	PLANENUM_LEAF		-1

typedef struct node_s
{
	vec3_t				mins, maxs;		// bounding volume, not just points inside
	int					planenum;		// PLANENUM_LEAF = leaf node

	// information for decision nodes
	int					outputplanenum;	// only valid after EmitNodePlanes
	int					firstface;		// decision node only
	int					numfaces;		// decision node only
	struct node_s		*children[2];	// only valid for decision nodes
	face_t				*faces;			// decision nodes only, list for both sides

	// information for leafs
	int					contents;		// leaf nodes (0 for decision nodes)
	face_t				**markfaces;	// leaf nodes only, point to node faces
	struct portal_s		*portals;
	int					visleafnum;		// -1 = solid
	int					valid;			// for flood filling
	int					occupied;		// light number in leaf for outside filling
} node_t;

typedef struct portal_s
{
	int					planenum;
	node_t				*nodes[2];		// [0] = front side of planenum
	struct portal_s		*next[2];
	winding_t			*winding;
} portal_t;

typedef struct
{
	vec3_t				mins, maxs;		// bounding box
	brush_t				*brushes;		// NULL terminated list
	face_t				*validfaces[BSP_MAX_PLANES];	// build surfaces is also used by GatherNodeFaces
	surface_t			*surfaces;
	node_t				*headnode;
} tree_t;

//=============================================================================

// brush.c
brush_t *AllocBrush( void );
void FreeBrush( brush_t *b );
void Brush_LoadEntity( entity_t *ent, tree_t *tree, int hullnum );

//=============================================================================

// csg4.c
surface_t *AllocSurface( void );
void FreeSurface( surface_t *s );
void BuildSurfaces( tree_t *tree );
void CSGFaces( tree_t *tree );

//=============================================================================

// solidbsp.c
node_t *AllocNode( void );
void FreeNode( node_t *n );

void CalcSurfaceInfo( surface_t *surf );
void SolidBSP( tree_t *tree, bool midsplit );

//=============================================================================

// merge.c

//=============================================================================

// surfaces.c
face_t *AllocFace( void );
void FreeFace( face_t *f );
face_t *NewFaceFromFace( face_t *in );

void SplitFace( face_t *in, plane_t *split, face_t **front, face_t **back );
int SubdivideFace( face_t *f, face_t **prevptr );

face_t *MergeFaceToList_r( face_t *face, face_t *list );
face_t *FreeMergeListScraps( face_t *merged );
void MergeTreeFaces( tree_t *tree );

void GatherTreeFaces( tree_t *tree );

//=============================================================================

// portals.c
extern	node_t	outside_node;		// portals outside the world face this

portal_t *AllocPortal( void );
void FreePortal( struct portal_s *p );

void PortalizeTree( tree_t *tree );
void FreeTreePortals( tree_t *tree );
void WritePortalfile( tree_t *tree );

//=============================================================================

// tjunc.c
void FixTJunctions( tree_t *tree );

//=============================================================================

// tree.c
tree_t *AllocTree( void );
void FreeTree( tree_t *t );
tree_t *Tree_ProcessEntity( entity_t *ent, int hullnum );

//=============================================================================

// writebsp.c
void EmitVertex( vec3_t point );
void EmitEdge( unsigned int v1, unsigned int v2 );
void EmitNodeFaceEdges( node_t *headnode );
void EmitNodeFaces( node_t *node );
void EmitNodePlanes( node_t *headnode );
void EmitClipNodes( node_t *nodes, int modnum, int hullnum );
void EmitDrawNodes( node_t *headnode );

void BeginBSPFile( void );
void FinishBSPFile( void );

//=============================================================================

// outside.c

bool FillOutside( tree_t *tree, int hullnum );

//=============================================================================

// wad.c

void WriteMiptex (void);

//=============================================================================

extern	bool nofill;
extern	bool notjunc;
extern	bool noclip;
extern	bool verbose;
extern	bool forcevis;
extern	bool transwater;
extern	bool waterlightmap;

extern char wadPath[PLATFORM_MAX_PATH];

extern	int		subdivide_size;

extern	int		valid;


// misc functions

typedef enum
{
	MSG_NORMAL,
	MSG_WARNING,
	MSG_ERROR,
	MSG_VERBOSE,
} LevelMessageType_t;

void Level_Log(LevelMessageType_t msgtype, const char *msg, ...);

void qprintf( char *fmt, ... );

#endif
