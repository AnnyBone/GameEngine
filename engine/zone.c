/*	Copyright (C) 2011-2016 OldTimes Software
	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others

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

#include "engine_base.h"

void Cache_FreeLow (int new_low_hunk);
void Cache_FreeHigh (int new_high_hunk);

void *malloc_or_die(size_t size)
{
	void *ptr = malloc(size);
	if(!ptr)
	{
		// Windows doesn't implement z size flag
		Sys_Error("Failed to allocate memory! (%llu bytes)\n",
			(unsigned long long int)(size));
	}

	return ptr;
}

void *calloc_or_die(size_t nmemb, size_t size)
{
	void *ptr = calloc(nmemb, size);
	if(!ptr)
	{
		// Windows doesn't implement z size flag
		Sys_Error("Failed to allocate memory! (%llu * %llu bytes)\n",
			(unsigned long long int)(nmemb), (unsigned long long int)(size));
	}

	return ptr;
}

void *realloc_or_die(void *ptr, size_t new_size)
{
	void *_ptr = realloc(ptr, new_size);
	if (!_ptr)
	{
		// Windows doesn't implement z size flag
		Sys_Error("Failed to reallocate memory! (%llu bytes)\n",
			(unsigned long long int)(new_size));
	}

	return _ptr;
}

//============================================================================

#define	HUNK_SENTINAL	0x1df001ed

typedef struct
{
	int		sentinal;
	int		size;		// including sizeof(hunk_t), -1 = not allocated
	char	name[8];
} hunk_t;

uint8_t		*hunk_base;
int			hunk_size;

int		hunk_low_used;
int		hunk_high_used;

void R_FreeTextures (void);

/*	Run consistancy and sentinal trahing checks
*/
void Hunk_Check (void)
{
	hunk_t	*h;

	for (h = (hunk_t *)hunk_base; (uint8_t *)h != hunk_base + hunk_low_used;)
	{
		if (h->sentinal != HUNK_SENTINAL)
			Sys_Error ("Hunk_Check: trahsed sentinal");
		if (h->size < 16 || h->size + (uint8_t *)h - hunk_base > hunk_size)
			Sys_Error ("Hunk_Check: bad size");
		h = (hunk_t *)((uint8_t *)h+h->size);
	}
}

/*	If "all" is specified, every single allocation is printed.
	Otherwise, allocations with the same name will be totaled up before printing.
*/
void Hunk_Print (bool all)
{
	hunk_t	*h, *next, *endlow, *starthigh, *endhigh;
	int		count, sum;
	int		totalblocks;
	char	name[9];

	name[8] = 0;
	count = 0;
	sum = 0;
	totalblocks = 0;

	h = (hunk_t *)hunk_base;
	endlow = (hunk_t *)(hunk_base + hunk_low_used);
	starthigh = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);
	endhigh = (hunk_t *)(hunk_base + hunk_size);

	Con_Printf ("          :%8i total hunk size\n", hunk_size);
	Con_Printf ("-------------------------\n");

	for(;;)
	{
		// Skip to the high hunk if done with low hunk
		if ( h == endlow )
		{
			Con_Printf ("-------------------------\n");
			Con_Printf ("          :%8i REMAINING\n", hunk_size - hunk_low_used - hunk_high_used);
			Con_Printf ("-------------------------\n");

			h = starthigh;
		}

		// If totally done, break
		if ( h == endhigh )
			break;

	//
	// run consistancy checks
	//
		if (h->sentinal != HUNK_SENTINAL)
			Sys_Error ("Hunk_Check: trahsed sentinal");
		if (h->size < 16 || h->size + (uint8_t *)h - hunk_base > hunk_size)
			Sys_Error ("Hunk_Check: bad size");

		next = (hunk_t *)((uint8_t *)h + h->size);
		count++;
		totalblocks++;
		sum += h->size;

	//
	// print the single block
	//
		memcpy (name, h->name, 8);
		if (all)
			Con_Printf ("%8p :%8i %8s\n",h, h->size, name);

	//
	// print the total
	//
		if (next == endlow || next == endhigh ||
		strncmp (h->name, next->name, 8) )
		{
			if (!all)
				Con_Printf ("          :%8i %8s (TOTAL)\n",sum, name);
			count = 0;
			sum = 0;
		}

		h = next;
	}

	Con_Printf ("-------------------------\n");
	Con_Printf ("%8i total blocks\n", totalblocks);

}

/*	Console command to call hunk_print
*/
void Hunk_Print_f (void)
{
	Hunk_Print(false);
}

void *Hunk_AllocName (int size, char *name)
{
	hunk_t	*h;

#ifdef PARANOID
	Hunk_Check ();
#endif

	if (size < 0)
		Sys_Error ("Hunk_Alloc: bad size: %i", size);

	size = sizeof(hunk_t) + ((size+15)&~15);

	if (hunk_size - hunk_low_used - hunk_high_used < size)
		Sys_Error ("Hunk_Alloc: failed on %i bytes",size);

	h = (hunk_t *)(hunk_base + hunk_low_used);
	hunk_low_used += size;

	Cache_FreeLow (hunk_low_used);

	memset(h,0,size);

	h->size = size;
	h->sentinal = HUNK_SENTINAL;

	strncpy(h->name, name, 8);

	return (void *)(h+1);
}

void *Hunk_Alloc (int size)
{
	return Hunk_AllocName (size, "unknown");
}

int	Hunk_LowMark (void)
{
	return hunk_low_used;
}

void Hunk_FreeToLowMark (int mark)
{
	if (mark < 0 || mark > hunk_low_used)
		Sys_Error ("Hunk_FreeToLowMark: bad mark %i (%i)", mark, hunk_low_used);
	memset (hunk_base + mark, 0, hunk_low_used - mark);
	hunk_low_used = mark;
}

int	Hunk_HighMark (void)
{
	return hunk_high_used;
}

void Hunk_FreeToHighMark(int mark)
{
	if (mark < 0 || mark > hunk_high_used)
		Sys_Error ("Hunk_FreeToHighMark: bad mark %i", mark);
	memset (hunk_base + hunk_size - hunk_high_used, 0, hunk_high_used - mark);
	hunk_high_used = mark;
}

void *Hunk_HighAllocName(int size,char *name)
{
	hunk_t	*h;

	if (size < 0)
		Sys_Error ("Hunk_HighAllocName: bad size: %i", size);

#ifdef PARANOID
	Hunk_Check ();
#endif

	size = sizeof(hunk_t) + ((size+15)&~15);

	if (hunk_size - hunk_low_used - hunk_high_used < size)
	{
		Con_Printf ("Hunk_HighAlloc: failed on %i bytes\n",size);
		return NULL;
	}

	hunk_high_used += size;
	Cache_FreeHigh (hunk_high_used);

	h = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);

	memset (h, 0, size);
	h->size = size;
	h->sentinal = HUNK_SENTINAL;
	strncpy(h->name, name, 8);

	return (void *)(h+1);
}

/*
===============================================================================

CACHE MEMORY

===============================================================================
*/

typedef struct cache_system_s
{
	int						size;		// including this header
	cache_user_t			*user;
	char					name[16];
	struct cache_system_s	*prev, *next;
	struct cache_system_s	*lru_prev, *lru_next;	// for LRU flushing
} cache_system_t;

cache_system_t *Cache_TryAlloc (int size, bool nobottom);

cache_system_t	cache_head;

void Cache_Move ( cache_system_t *c)
{
	cache_system_t	*cNewCache;

// we are clearing up space at the bottom, so only allocate it late
	cNewCache = Cache_TryAlloc(c->size,true);
	if(cNewCache)
	{
//		Con_Printf ("cache_move ok\n");

		memcpy(cNewCache+1,c+1,c->size-sizeof(cache_system_t));
		cNewCache->user = c->user;
		memcpy (cNewCache->name,c->name,sizeof(cNewCache->name));
		Cache_Free(c->user,false); //johnfitz -- added second argument
		cNewCache->user->data = (void*)(cNewCache+1);
	}
	else
	{
//		Con_Printf ("cache_move failed\n");

		Cache_Free(c->user,true); // tough luck... //johnfitz -- added second argument
	}
}

/*	Throw things out until the hunk can be expanded to the given point
*/
void Cache_FreeLow (int new_low_hunk)
{
	cache_system_t	*c;

	for(;;)
	{
		c = cache_head.next;
		if (c == &cache_head)
			return;		// nothing in cache at all
		if ((uint8_t *)c >= hunk_base + new_low_hunk)
			return;		// there is space to grow the hunk
		Cache_Move ( c );	// reclaim the space
	}
}

/*	Throw things out until the hunk can be expanded to the given point
*/
void Cache_FreeHigh (int new_high_hunk)
{
	cache_system_t	*c, *prev;

	prev = NULL;
	for(;;)
	{
		c = cache_head.prev;
		if (c == &cache_head)
			return;		// nothing in cache at all
		if ((uint8_t *)c + c->size <= hunk_base + hunk_size - new_high_hunk)
			return;		// there is space to grow the hunk
		if (prev && (c == prev))
			Cache_Free(c->user, true);	// didn't move out of the way //johnfitz -- added second argument
		else
		{
			Cache_Move (c);	// try to move it
			prev = c;
		}
	}
}

void Cache_UnlinkLRU (cache_system_t *cs)
{
	if (!cs->lru_next || !cs->lru_prev)
	{
		Sys_Error ("Cache_UnlinkLRU: NULL link");
		return;
	}

	cs->lru_next->lru_prev = cs->lru_prev;
	cs->lru_prev->lru_next = cs->lru_next;

	cs->lru_prev = cs->lru_next = NULL;
}

void Cache_MakeLRU (cache_system_t *cs)
{
	if (cs->lru_next || cs->lru_prev)
		Sys_Error ("Cache_MakeLRU: active link");

	cache_head.lru_next->lru_prev = cs;
	cs->lru_next = cache_head.lru_next;
	cs->lru_prev = &cache_head;
	cache_head.lru_next = cs;
}

/*
============
Cache_TryAlloc

Looks for a free block of memory between the high and low hunk marks
Size should already include the header and padding
============
*/
cache_system_t *Cache_TryAlloc (int size, bool nobottom)
{
	cache_system_t	*cs, *newc;

// is the cache completely empty?

	if (!nobottom && cache_head.prev == &cache_head)
	{
		if (hunk_size - hunk_high_used - hunk_low_used < size)
			Sys_Error ("Cache_TryAlloc: %i is greater than free hunk", size);

		newc = (cache_system_t *)(hunk_base + hunk_low_used);
		memset(newc, 0, sizeof(*newc));
		newc->size = size;

		cache_head.prev = cache_head.next = newc;
		newc->prev = newc->next = &cache_head;

		Cache_MakeLRU(newc);
		return newc;
	}

// search from the bottom up for space

	newc = (cache_system_t *)(hunk_base + hunk_low_used);
	cs = cache_head.next;

	do
	{
		if (!nobottom || cs != cache_head.next)
		{
			if ((uint8_t *)cs - (uint8_t *)newc >= size)
			{	// found space
				memset(newc, 0, sizeof(*newc));
				newc->size = size;

				newc->next = cs;
				newc->prev = cs->prev;
				cs->prev->next = newc;
				cs->prev = newc;

				Cache_MakeLRU(newc);

				return newc;
			}
		}

	// continue looking
		newc = (cache_system_t *)((uint8_t *)cs + cs->size);
		cs = cs->next;

	} while (cs != &cache_head);

// try to allocate one at the very end
	if (hunk_base + hunk_size - hunk_high_used - (uint8_t *)newc >= size)
	{
		memset(newc, 0, sizeof(*newc));
		newc->size = size;

		newc->next = &cache_head;
		newc->prev = cache_head.prev;
		cache_head.prev->next = newc;
		cache_head.prev = newc;

		Cache_MakeLRU(newc);

		return newc;
	}

	return NULL;		// couldn't allocate
}

/*	Throw everything out, so new data will be demand cached
*/
void Cache_Flush (void)
{
	while(cache_head.next != &cache_head)
		Cache_Free(cache_head.next->user, true); // reclaim the space //johnfitz -- added second argument
}

void Cache_Print (void)
{
	cache_system_t	*cd;

	for (cd = cache_head.next ; cd != &cache_head ; cd = cd->next)
		Con_Printf("%8i : %s\n", cd->size, cd->name);
}

void Cache_Report (void)
{
	Con_DPrintf("%4.1f megabyte data cache\n", (hunk_size - hunk_high_used - hunk_low_used) / (float)(1024*1024) );
}

void Cache_Init (void)
{
	cache_head.next = cache_head.prev = &cache_head;
	cache_head.lru_next = cache_head.lru_prev = &cache_head;

	Cmd_AddCommand ("flush", Cache_Flush);
}

/*	Frees the memory and removes it from the LRU list
*/
void Cache_Free(cache_user_t *c,bool freetextures) //johnfitz -- added second argument
{
	cache_system_t	*cs;

	if (!c->data)
		Sys_Error ("Cache_Free: not allocated");

	cs = ((cache_system_t *)c->data) - 1;
	if(!cs)
	{
		Sys_Error("Failed to reference pointer, this shouldn't happen!\n");
		return;
	}

	cs->prev->next = cs->next;
	cs->next->prev = cs->prev;
	cs->next = cs->prev = NULL;

	c->data = NULL;

	Cache_UnlinkLRU (cs);
}

void *Cache_Check (cache_user_t *c)
{
	cache_system_t	*cs;

	if (!c->data)
		return NULL;

	cs = ((cache_system_t *)c->data) - 1;

	// Move to head of LRU
	Cache_UnlinkLRU (cs);
	Cache_MakeLRU (cs);

	return c->data;
}

void *Cache_Alloc (cache_user_t *c, int size, char *name)
{
	cache_system_t	*cs;

	if (c->data)
		Sys_Error ("Cache_Alloc: allready allocated");

	if (size <= 0)
		Sys_Error ("Cache_Alloc: size %i", size);

	size = (size + sizeof(cache_system_t) + 15) & ~15;

	// Find memory for it
	for(;;)
	{
		cs = Cache_TryAlloc(size,false);
		if(cs)
		{
			strncpy(cs->name, name, sizeof(cs->name) - 1);
			c->data = (void*)(cs+1);
			cs->user = c;
			break;
		}

		// Free the least recently used cahedat
		if(cache_head.lru_prev == &cache_head)
			Sys_Error("Cache_Alloc: out of memory"); // not enough memory at all

		Cache_Free(cache_head.lru_prev->user,true); //johnfitz -- added second argument
	}

	return Cache_Check (c);
}

//============================================================================

void Memory_Initialize()
{
	hunk_base = (uint8_t*)host_parms.membase;
	hunk_size = host_parms.memsize;
	hunk_low_used = 0;
	hunk_high_used = 0;
	
	Cache_Init ();

	Cmd_AddCommand ("hunk_print", Hunk_Print_f); //johnfitz
}

