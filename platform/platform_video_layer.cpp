/*	Copyright (C) 2011-2016 OldTimes Software
*/

#include "platform.h"

#include "platform_video_layer.h"

typedef struct vlState_s
{
	unsigned int num_cards;	// Number of detected video cards.

	vlCullMode_t	current_cullmode;		// Vertex cull mode.
	plColour_t		current_clearcolour;	// Buffer clear colour.
	unsigned int	current_capabilities;	// Enabled capabilities.

	bool debug;
} vlState_t;
vlState_t vl_state;

/*===========================
	INITIALIZATION
===========================*/

void vlInit(void)
{
	memset(&vl_state, 0, sizeof(vlState_t));
}

/*
	C Wrapper
*/