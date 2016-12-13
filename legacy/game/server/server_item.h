#ifndef __SERVERITEM__
#define __SERVERITEM__

#include "shared_game.h"

typedef struct
{
	int		iNumber;	// Our items unique ID

	char	*cName,		// The name of the item
			*cModel,	// Model path
			*cSound;	// Sound that's emitted upon picking the item up

	bool	bRespawn;
} Item_t;

Item_t *Item_GetItem(int iItemID);
bool Item_CheckInventory(Item_t *iItem,ServerEntity_t *eEntity);
Item_t *Item_GetInventory(int iItemID, ServerEntity_t *eEntity);

void Item_Spawn(ServerEntity_t *eItem);
void Item_AddInventory(Item_t *iItem, ServerEntity_t *eEntity);
void Item_RemoveInventory(Item_t *iItem, ServerEntity_t *eEntity);
void Item_ClearInventory(ServerEntity_t *eEntity);
void Item_Precache(void);

#endif
