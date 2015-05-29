/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "menu_main.h"

#include "menu_hud.h"

void HUD_Draw(void)
{
	// Draw the crosshair...
	{
		Engine.Client_SetMenuCanvas(CANVAS_CROSSHAIR);
		Engine.DrawPic(va(MENU_HUD_PATH"crosshair%i",1),1.0f,-16,-16,32,32);
	}

	// Draw the rest...
	Engine.Client_SetMenuCanvas(CANVAS_DEFAULT);

#ifdef GAME_ADAMAS
	Engine.DrawFill(0,iMenuHeight-64,iMenuWidth,64,0,0,0,0.6f);

	if(cvShowHealth.bValue)
	{
		int	iClientArmor	= Engine.Client_GetStat(STAT_ARMOR),
			iClientHealth	= Engine.Client_GetStat(STAT_HEALTH);

		if(iClientHealth < 0)
			iClientHealth = 0;

		Engine.DrawPic(MENU_HUD_PATH"health",1.0f,30,iMenuHeight-64,64,64);

		if(iClientHealth >= 100)
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",iClientHealth/100),1.0f,80,iMenuHeight-64,64,64);

		if(iClientHealth >= 10)
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",(iClientHealth%100)/10),1.0f,112,iMenuHeight-64,64,64);

		Engine.DrawPic(va(MENU_HUD_PATH"num%i",iClientHealth%10),1.0f,144,iMenuHeight-64,64,64);
	}

	if(cvShowAmmo.bValue)
	{
		int iClientAmmo = Engine.Client_GetStat(STAT_AMMO);

		Engine.DrawPic(MENU_HUD_PATH"ammo",1.0f,200,iMenuHeight-64,64,64);

		if(iClientAmmo >= 100)
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",iClientAmmo/100),1.0f,250,iMenuHeight-64,64,64);

		if(iClientAmmo >= 10)
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",(iClientAmmo%100)/10),1.0f,282,iMenuHeight-64,64,64);

		Engine.DrawPic(va(MENU_HUD_PATH"num%i",iClientAmmo%10),1.0f,314,iMenuHeight-64,64,64);
	}

	// Display the players current score.
	Engine.DrawString(iMenuWidth-128,iMenuHeight-16,va("SCORE: %i",Engine.Client_GetStat(STAT_FRAGS)));
#if 0
	Engine.DrawString(iMenuWidth-128,iMenuHeight-32,va("LIVES: %i",Engine.Client_GetStat(STAT_LIVES)));
#endif
#elif GAME_OPENKATANA
	{
		int iArmor[3],iAmmo[3];
		int armor = Engine.Client_GetStat(STAT_ARMOR),
			health = Engine.Client_GetStat(STAT_HEALTH),
			ammo = Engine.Client_GetStat(STAT_AMMO);

		// Health

		Engine.DrawMaterialSurface(mIcons, 0, 70, iMenuHeight - 70, 32, 32, 1.0f);

		if (health >= 100)
			Engine.DrawMaterialSurface(mNums, (health / 100), 102, iMenuHeight-70, 32, 32, 1.0f);

		if(health >= 10)
			Engine.DrawMaterialSurface(mNums, ((health % 100) / 10), 134, iMenuHeight - 70, 32, 32, 1.0f);

		Engine.DrawMaterialSurface(mNums, (health % 10), 166, iMenuHeight - 70, 32, 32, 1.0f);

		// Armor

		Engine.DrawMaterialSurface(mIcons, 1, 70, iMenuHeight - 110, 32, 32, 1.0f);

		if (armor >= 100)
			Engine.DrawMaterialSurface(mNums, (armor / 100), 102, iMenuHeight-70, 32, 32, 1.0f);

		if(armor >= 10)
			Engine.DrawMaterialSurface(mNums, ((armor % 100) / 10), 134, iMenuHeight - 70, 32, 32, 1.0f);

#if 0
		if (armor >= 100)
		{
			iArmor[0] = armor/100;
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",iArmor[0]),1.0f,46,iMenuHeight-70,iNSize[WIDTH],iNSize[HEIGHT]);
		}

		if(armor >= 10)
		{
			iArmor[1] = (armor%100)/10;
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",iArmor[1]),1.0f,70,iMenuHeight-70,iNSize[WIDTH],iNSize[HEIGHT]);
		}

		iArmor[2] = armor%10;
		Engine.DrawPic(va(MENU_HUD_PATH"num%i",iArmor[2]),1.0f,94,iMenuHeight-70,iNSize[WIDTH],iNSize[HEIGHT]);

		if(ammo >= 100)
		{
			iAmmo[0] = ammo/100;
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",iAmmo[0]),1.0f,iMenuWidth-94,iMenuHeight-40,iNSize[WIDTH],iNSize[HEIGHT]);
		}

		if(ammo >= 10)
		{
			iAmmo[1] = (ammo % 100)/10;
			Engine.DrawPic(va(MENU_HUD_PATH"num%i",iAmmo[1]),1.0f,iMenuWidth-70,iMenuHeight-40,iNSize[WIDTH],iNSize[HEIGHT]);
		}

		iAmmo[2] = ammo % 10;
		Engine.DrawPic(va(MENU_HUD_PATH"num%i",iAmmo[2]),1.0f,iMenuWidth-46,iMenuHeight-40,iNSize[WIDTH],iNSize[HEIGHT]);
#endif
	}
#endif
}