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

#include "platform.h"

#include "platform_video_layer.h"

typedef struct vlState_s
{
	unsigned int num_cards;	// Number of detected video cards.

	VLCullMode		current_cullmode;		// Vertex cull mode.
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