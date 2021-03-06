// trace.c

#include "light.h"

/*
==============================================================================

LINE TRACING

The major lighting operation is a point to point visibility test, performed
by recursive subdivision of the line by the BSP tree.

==============================================================================
*/

BSPLeaf_t *Light_PointInLeaf( const vec3_t point )
{
	int num = 0;

	while( num >= 0 )
		num = dnodes[num].iChildren[PlaneDiff(point,&dplanes[dnodes[num].iPlaneNum]) < 0];

	return dleafs + (-1 - num);
}

int Light_PointContents( const vec3_t point )
{
	return Light_PointInLeaf(point)->iContents;
}

#define TESTLINESTATE_BLOCKED 0
#define TESTLINESTATE_EMPTY 1
#define TESTLINESTATE_SOLID 2

static int RecursiveTestLine (lightTrace_t *trace, int num, float p1f, float p2f, const vec3_t p1, const vec3_t p2)
{
	int			side, ret;
	vec_t		t1, t2, frac, midf;
	vec3_t		mid;
	BSPNode_t	*node;
	BSPPlane_t	*plane;

	// LordHavoc: this function operates by doing depth-first front-to-back
	// recursion through the BSP tree, checking at every split for an empty to
	// solid transition (impact) in the children, and returns
	// TESTLINESTATE_BLOCKED if one is found

	// LordHavoc: note: 'no impact' does not mean it is empty, it occurs when
	// there is no transition from empty to solid; all solid or a transition
	// from solid to empty are not considered impacts. (this does mean that
	// tracing is not symmetrical; point A to point B may have different
	// results than point B to point A, if either start in solid)
	
	while (num >= 0)	
	{
		// find the point distances
		node = dnodes + num;
		plane = dplanes + node->iPlaneNum;

		if (plane->iType < 3)
		{
			t1 = p1[plane->iType]-plane->fDist;
			t2 = p2[plane->iType]-plane->fDist;
		}
		else
		{
			t1 = DotProduct(plane->fNormal,p1)-plane->fDist;
			t2 = DotProduct(plane->fNormal,p2)-plane->fDist;
		}

		if (t1 >= 0)
		{
			if (t2 >= 0)
			{
				num = node->iChildren[0];
				continue;
			}
			side = 0;
		}
		else
		{
			if (t2 < 0)
			{
				num = node->iChildren[1];
				continue;
			}
			side = 1;
		}
	
		frac = t1 / (t1 - t2);
		midf = p1f + (p2f - p1f) * frac;
		mid[0] = p1[0] + frac * (p2[0] - p1[0]);
		mid[1] = p1[1] + frac * (p2[1] - p1[1]);
		mid[2] = p1[2] + frac * (p2[2] - p1[2]);

		// front side first
		ret = RecursiveTestLine (trace, node->iChildren[side], p1f, midf, p1, mid);
		if (ret != TESTLINESTATE_EMPTY)
			return ret; // solid or blocked

		ret = RecursiveTestLine (trace, node->iChildren[!side], midf, p2f, mid, p2);
		if (ret != TESTLINESTATE_SOLID)
			return ret; // empty or blocked

		if (!side)
		{
			VectorCopy (plane->fNormal, trace->plane.normal);
			trace->plane.dist = plane->fDist;
			trace->plane.type = plane->iType;
		}
		else
		{
			VectorNegate (plane->fNormal, trace->plane.normal);
			trace->plane.dist = -plane->fDist;
			trace->plane.type = PLANE_ANYX;
		}

		// calculate the return fraction which is nudged off the surface a bit
		t1 = DotProduct(trace->plane.normal, trace->start) - trace->plane.dist;
		t2 = DotProduct(trace->plane.normal, trace->end) - trace->plane.dist;
		trace->fraction = (t1 - (1.0 / 32.0)) / (t1 - t2);

		return TESTLINESTATE_BLOCKED;
	}

	// check leaf contents
	num = dleafs[-num - 1].iContents;
	if (!trace->startcontents)
		trace->startcontents = num;

	trace->endcontents = num;
	if (num == BSP_CONTENTS_SOLID || (num == BSP_CONTENTS_SKY && trace->hitsky))
	{
		//VectorClear( trace->filter );
		return TESTLINESTATE_SOLID;
	}
	else if (num == BSP_CONTENTS_WATER)
	{
		//trace->filter[0] *= 0.6;
		//trace->filter[1] *= 0.6;
	}
	else if (num == BSP_CONTENTS_LAVA)
	{
		//trace->filter[1] *= 0.6;
		//trace->filter[2] *= 0.6;
	}

	return TESTLINESTATE_EMPTY;
}

void Light_TraceLine( lightTrace_t *trace, const vec3_t start, const vec3_t end, bool hitsky )
{
	if( !trace )
		return;

	memset(trace, 0, sizeof(*trace));
	trace->fraction = 1.0;
	trace->hitsky = hitsky;
	VectorSet (trace->filter, 1, 1, 1);
	VectorCopy (start, trace->start);
	VectorCopy (end, trace->end);

	RecursiveTestLine( trace, 0, 0, 1, start, end );

	trace->fraction = bound(0, trace->fraction, 1);
	trace->impact[0] = trace->start[0] + trace->fraction * (trace->end[0] - trace->start[0]);
	trace->impact[1] = trace->start[1] + trace->fraction * (trace->end[1] - trace->start[1]);
	trace->impact[2] = trace->start[2] + trace->fraction * (trace->end[2] - trace->start[2]);
}

