#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I. -Iclient -Iserver -I../platform/include -I../shared
CFLAGS = -Wall -pg -g -fPIC -DOPENKATANA -DGAME_OPENKATANA
RESINC = 
LIBDIR = -L../platform/lib
LIB = 
LDFLAGS = -pg

INC_X64 = $(INC)
CFLAGS_X64 = $(CFLAGS)
RESINC_X64 = $(RESINC)
RCFLAGS_X64 = $(RCFLAGS)
LIBDIR_X64 = $(LIBDIR)
LIB_X64 = $(LIB)../platform/lib/libplatformx64.a
LDFLAGS_X64 = $(LDFLAGS)
OBJDIR_X64 = obj/Release
DEP_X64 = 
OUT_X64 = ../../openkatana/game.x64.so

INC_X86 = $(INC)
CFLAGS_X86 = $(CFLAGS)
RESINC_X86 = $(RESINC)
RCFLAGS_X86 = $(RCFLAGS)
LIBDIR_X86 = $(LIBDIR)
LIB_X86 = $(LIB)libplatformx86.a
LDFLAGS_X86 = $(LDFLAGS)
OBJDIR_X86 = obj/Release
DEP_X86 = 
OUT_X86 = ../../openkatana/game.x86.so

OBJ_X64 = $(OBJDIR_X64)/server/openkatana/weapon_trident.o $(OBJDIR_X64)/server/server_main.o $(OBJDIR_X64)/server/server_item.o $(OBJDIR_X64)/server/server_gib.o $(OBJDIR_X64)/server/server_entity.o $(OBJDIR_X64)/server/server_area.o $(OBJDIR_X64)/server/openkatana/weapon_zeus.o $(OBJDIR_X64)/server/openkatana/weapon_sidewinder.o $(OBJDIR_X64)/server/openkatana/weapon_shotcycler6.o $(OBJDIR_X64)/server/openkatana/weapon_shockwave.o $(OBJDIR_X64)/server/openkatana/weapon_pulserifle.o $(OBJDIR_X64)/server/openkatana/weapon_midas.o $(OBJDIR_X64)/server/openkatana/weapon_kineticore.o $(OBJDIR_X64)/server/openkatana/weapon_ionrifle.o $(OBJDIR_X64)/server/openkatana/weapon_ionblaster.o $(OBJDIR_X64)/server/server_player.o $(OBJDIR_X64)/server/server_weapon.o $(OBJDIR_X64)/server/server_waypoint.o $(OBJDIR_X64)/server/server_vehicle.o $(OBJDIR_X64)/server/server_point.o $(OBJDIR_X64)/server/openkatana/weapon_greekfire.o $(OBJDIR_X64)/server/server_physics.o $(OBJDIR_X64)/server/server_monster.o $(OBJDIR_X64)/server/server_misc.o $(OBJDIR_X64)/server/ServerEntity.o $(OBJDIR_X64)/game_main.o $(OBJDIR_X64)/client/client_view.o $(OBJDIR_X64)/client/client_main.o $(OBJDIR_X64)/client/client_effects.o $(OBJDIR_X64)/client/cl_player.o $(OBJDIR_X64)/server/monster_bot.o $(OBJDIR_X64)/server/openkatana/monster_lasergat.o $(OBJDIR_X64)/server/openkatana/weapon_glock.o $(OBJDIR_X64)/server/openkatana/weapon_discus.o $(OBJDIR_X64)/server/openkatana/weapon_daikatana.o $(OBJDIR_X64)/server/openkatana/weapon_c4vizatergo.o $(OBJDIR_X64)/server/openkatana/weapon_barrier.o $(OBJDIR_X64)/server/openkatana/weapon_ballista.o $(OBJDIR_X64)/server/openkatana/weapon_axe.o $(OBJDIR_X64)/server/openkatana/monster_prisoner.o $(OBJDIR_X64)/server/openkatana/monster_inmater.o $(OBJDIR_X64)/server/openkatana/monster_cambot.o $(OBJDIR_X64)/server/openkatana/mode_vektar.o $(OBJDIR_X64)/server/openkatana/mode_deathmatch.o $(OBJDIR_X64)/server/openkatana/mode_ctf.o $(OBJDIR_X64)/server/openkatana/decoration_barrel.o

OBJ_X86 = $(OBJDIR_X86)/server/openkatana/weapon_trident.o $(OBJDIR_X86)/server/server_main.o $(OBJDIR_X86)/server/server_item.o $(OBJDIR_X86)/server/server_gib.o $(OBJDIR_X86)/server/server_entity.o $(OBJDIR_X86)/server/server_area.o $(OBJDIR_X86)/server/openkatana/weapon_zeus.o $(OBJDIR_X86)/server/openkatana/weapon_sidewinder.o $(OBJDIR_X86)/server/openkatana/weapon_shotcycler6.o $(OBJDIR_X86)/server/openkatana/weapon_shockwave.o $(OBJDIR_X86)/server/openkatana/weapon_pulserifle.o $(OBJDIR_X86)/server/openkatana/weapon_midas.o $(OBJDIR_X86)/server/openkatana/weapon_kineticore.o $(OBJDIR_X86)/server/openkatana/weapon_ionrifle.o $(OBJDIR_X86)/server/openkatana/weapon_ionblaster.o $(OBJDIR_X86)/server/server_player.o $(OBJDIR_X86)/server/server_weapon.o $(OBJDIR_X86)/server/server_waypoint.o $(OBJDIR_X86)/server/server_vehicle.o $(OBJDIR_X86)/server/server_point.o $(OBJDIR_X86)/server/openkatana/weapon_greekfire.o $(OBJDIR_X86)/server/server_physics.o $(OBJDIR_X86)/server/server_monster.o $(OBJDIR_X86)/server/server_misc.o $(OBJDIR_X86)/server/ServerEntity.o $(OBJDIR_X86)/game_main.o $(OBJDIR_X86)/client/client_view.o $(OBJDIR_X86)/client/client_main.o $(OBJDIR_X86)/client/client_effects.o $(OBJDIR_X86)/client/cl_player.o $(OBJDIR_X86)/server/monster_bot.o $(OBJDIR_X86)/server/openkatana/monster_lasergat.o $(OBJDIR_X86)/server/openkatana/weapon_glock.o $(OBJDIR_X86)/server/openkatana/weapon_discus.o $(OBJDIR_X86)/server/openkatana/weapon_daikatana.o $(OBJDIR_X86)/server/openkatana/weapon_c4vizatergo.o $(OBJDIR_X86)/server/openkatana/weapon_barrier.o $(OBJDIR_X86)/server/openkatana/weapon_ballista.o $(OBJDIR_X86)/server/openkatana/weapon_axe.o $(OBJDIR_X86)/server/openkatana/monster_prisoner.o $(OBJDIR_X86)/server/openkatana/monster_inmater.o $(OBJDIR_X86)/server/openkatana/monster_cambot.o $(OBJDIR_X86)/server/openkatana/mode_vektar.o $(OBJDIR_X86)/server/openkatana/mode_deathmatch.o $(OBJDIR_X86)/server/openkatana/mode_ctf.o $(OBJDIR_X86)/server/openkatana/decoration_barrel.o

all: x64

before_x64: 
	test -d ../../openkatana || mkdir -p ../../openkatana
	test -d $(OBJDIR_X64)/server/openkatana || mkdir -p $(OBJDIR_X64)/server/openkatana
	test -d $(OBJDIR_X64)/server || mkdir -p $(OBJDIR_X64)/server
	test -d $(OBJDIR_X64) || mkdir -p $(OBJDIR_X64)
	test -d $(OBJDIR_X64)/client || mkdir -p $(OBJDIR_X64)/client

after_x64: 

x64: before_x64 out_x64 after_x64

out_x64: before_x64 $(OBJ_X64) $(DEP_X64)
	$(LD) -shared $(LIBDIR_X64) $(OBJ_X64)  -o $(OUT_X64) $(LDFLAGS_X64) $(LIB_X64)

$(OBJDIR_X64)/server/openkatana/weapon_trident.o: server/openkatana/weapon_trident.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_trident.c -o $(OBJDIR_X64)/server/openkatana/weapon_trident.o

$(OBJDIR_X64)/server/server_main.o: server/server_main.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_main.c -o $(OBJDIR_X64)/server/server_main.o

$(OBJDIR_X64)/server/server_item.o: server/server_item.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_item.c -o $(OBJDIR_X64)/server/server_item.o

$(OBJDIR_X64)/server/server_gib.o: server/server_gib.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_gib.c -o $(OBJDIR_X64)/server/server_gib.o

$(OBJDIR_X64)/server/server_entity.o: server/server_entity.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_entity.c -o $(OBJDIR_X64)/server/server_entity.o

$(OBJDIR_X64)/server/server_area.o: server/server_area.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_area.c -o $(OBJDIR_X64)/server/server_area.o

$(OBJDIR_X64)/server/openkatana/weapon_zeus.o: server/openkatana/weapon_zeus.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_zeus.c -o $(OBJDIR_X64)/server/openkatana/weapon_zeus.o

$(OBJDIR_X64)/server/openkatana/weapon_sidewinder.o: server/openkatana/weapon_sidewinder.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_sidewinder.c -o $(OBJDIR_X64)/server/openkatana/weapon_sidewinder.o

$(OBJDIR_X64)/server/openkatana/weapon_shotcycler6.o: server/openkatana/weapon_shotcycler6.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_shotcycler6.c -o $(OBJDIR_X64)/server/openkatana/weapon_shotcycler6.o

$(OBJDIR_X64)/server/openkatana/weapon_shockwave.o: server/openkatana/weapon_shockwave.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_shockwave.c -o $(OBJDIR_X64)/server/openkatana/weapon_shockwave.o

$(OBJDIR_X64)/server/openkatana/weapon_pulserifle.o: server/openkatana/weapon_pulserifle.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_pulserifle.c -o $(OBJDIR_X64)/server/openkatana/weapon_pulserifle.o

$(OBJDIR_X64)/server/openkatana/weapon_midas.o: server/openkatana/weapon_midas.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_midas.c -o $(OBJDIR_X64)/server/openkatana/weapon_midas.o

$(OBJDIR_X64)/server/openkatana/weapon_kineticore.o: server/openkatana/weapon_kineticore.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_kineticore.c -o $(OBJDIR_X64)/server/openkatana/weapon_kineticore.o

$(OBJDIR_X64)/server/openkatana/weapon_ionrifle.o: server/openkatana/weapon_ionrifle.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_ionrifle.c -o $(OBJDIR_X64)/server/openkatana/weapon_ionrifle.o

$(OBJDIR_X64)/server/openkatana/weapon_ionblaster.o: server/openkatana/weapon_ionblaster.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_ionblaster.c -o $(OBJDIR_X64)/server/openkatana/weapon_ionblaster.o

$(OBJDIR_X64)/server/server_player.o: server/server_player.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_player.c -o $(OBJDIR_X64)/server/server_player.o

$(OBJDIR_X64)/server/server_weapon.o: server/server_weapon.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_weapon.c -o $(OBJDIR_X64)/server/server_weapon.o

$(OBJDIR_X64)/server/server_waypoint.o: server/server_waypoint.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_waypoint.c -o $(OBJDIR_X64)/server/server_waypoint.o

$(OBJDIR_X64)/server/server_vehicle.o: server/server_vehicle.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_vehicle.c -o $(OBJDIR_X64)/server/server_vehicle.o

$(OBJDIR_X64)/server/server_point.o: server/server_point.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_point.c -o $(OBJDIR_X64)/server/server_point.o

$(OBJDIR_X64)/server/openkatana/weapon_greekfire.o: server/openkatana/weapon_greekfire.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_greekfire.c -o $(OBJDIR_X64)/server/openkatana/weapon_greekfire.o

$(OBJDIR_X64)/server/server_physics.o: server/server_physics.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_physics.c -o $(OBJDIR_X64)/server/server_physics.o

$(OBJDIR_X64)/server/server_monster.o: server/server_monster.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_monster.c -o $(OBJDIR_X64)/server/server_monster.o

$(OBJDIR_X64)/server/server_misc.o: server/server_misc.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/server_misc.c -o $(OBJDIR_X64)/server/server_misc.o

$(OBJDIR_X64)/server/ServerEntity.o: server/ServerEntity.cpp
	$(CXX) $(CFLAGS_X64) $(INC_X64) -c server/ServerEntity.cpp -o $(OBJDIR_X64)/server/ServerEntity.o

$(OBJDIR_X64)/game_main.o: game_main.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c game_main.c -o $(OBJDIR_X64)/game_main.o

$(OBJDIR_X64)/client/client_view.o: client/client_view.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c client/client_view.c -o $(OBJDIR_X64)/client/client_view.o

$(OBJDIR_X64)/client/client_main.o: client/client_main.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c client/client_main.c -o $(OBJDIR_X64)/client/client_main.o

$(OBJDIR_X64)/client/client_effects.o: client/client_effects.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c client/client_effects.c -o $(OBJDIR_X64)/client/client_effects.o

$(OBJDIR_X64)/client/cl_player.o: client/cl_player.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c client/cl_player.c -o $(OBJDIR_X64)/client/cl_player.o

$(OBJDIR_X64)/server/monster_bot.o: server/monster_bot.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/monster_bot.c -o $(OBJDIR_X64)/server/monster_bot.o

$(OBJDIR_X64)/server/openkatana/monster_lasergat.o: server/openkatana/monster_lasergat.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/monster_lasergat.c -o $(OBJDIR_X64)/server/openkatana/monster_lasergat.o

$(OBJDIR_X64)/server/openkatana/weapon_glock.o: server/openkatana/weapon_glock.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_glock.c -o $(OBJDIR_X64)/server/openkatana/weapon_glock.o

$(OBJDIR_X64)/server/openkatana/weapon_discus.o: server/openkatana/weapon_discus.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_discus.c -o $(OBJDIR_X64)/server/openkatana/weapon_discus.o

$(OBJDIR_X64)/server/openkatana/weapon_daikatana.o: server/openkatana/weapon_daikatana.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_daikatana.c -o $(OBJDIR_X64)/server/openkatana/weapon_daikatana.o

$(OBJDIR_X64)/server/openkatana/weapon_c4vizatergo.o: server/openkatana/weapon_c4vizatergo.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_c4vizatergo.c -o $(OBJDIR_X64)/server/openkatana/weapon_c4vizatergo.o

$(OBJDIR_X64)/server/openkatana/weapon_barrier.o: server/openkatana/weapon_barrier.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_barrier.c -o $(OBJDIR_X64)/server/openkatana/weapon_barrier.o

$(OBJDIR_X64)/server/openkatana/weapon_ballista.o: server/openkatana/weapon_ballista.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_ballista.c -o $(OBJDIR_X64)/server/openkatana/weapon_ballista.o

$(OBJDIR_X64)/server/openkatana/weapon_axe.o: server/openkatana/weapon_axe.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/weapon_axe.c -o $(OBJDIR_X64)/server/openkatana/weapon_axe.o

$(OBJDIR_X64)/server/openkatana/monster_prisoner.o: server/openkatana/monster_prisoner.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/monster_prisoner.c -o $(OBJDIR_X64)/server/openkatana/monster_prisoner.o

$(OBJDIR_X64)/server/openkatana/monster_inmater.o: server/openkatana/monster_inmater.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/monster_inmater.c -o $(OBJDIR_X64)/server/openkatana/monster_inmater.o

$(OBJDIR_X64)/server/openkatana/monster_cambot.o: server/openkatana/monster_cambot.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/monster_cambot.c -o $(OBJDIR_X64)/server/openkatana/monster_cambot.o

$(OBJDIR_X64)/server/openkatana/mode_vektar.o: server/openkatana/mode_vektar.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/mode_vektar.c -o $(OBJDIR_X64)/server/openkatana/mode_vektar.o

$(OBJDIR_X64)/server/openkatana/mode_deathmatch.o: server/openkatana/mode_deathmatch.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/mode_deathmatch.c -o $(OBJDIR_X64)/server/openkatana/mode_deathmatch.o

$(OBJDIR_X64)/server/openkatana/mode_ctf.o: server/openkatana/mode_ctf.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/mode_ctf.c -o $(OBJDIR_X64)/server/openkatana/mode_ctf.o

$(OBJDIR_X64)/server/openkatana/decoration_barrel.o: server/openkatana/decoration_barrel.c
	$(CC) $(CFLAGS_X64) $(INC_X64) -c server/openkatana/decoration_barrel.c -o $(OBJDIR_X64)/server/openkatana/decoration_barrel.o

clean_x64: 
	rm -f $(OBJ_X64) $(OUT_X64)

	rm -rf $(OBJDIR_X64)/server/openkatana
	rm -rf $(OBJDIR_X64)/server
	rm -rf $(OBJDIR_X64)
	rm -rf $(OBJDIR_X64)/client

before_x86: 
	test -d ../bin/openkatana || mkdir -p ../bin/openkatana
	test -d $(OBJDIR_X86)/server/openkatana || mkdir -p $(OBJDIR_X86)/server/openkatana
	test -d $(OBJDIR_X86)/server || mkdir -p $(OBJDIR_X86)/server
	test -d $(OBJDIR_X86) || mkdir -p $(OBJDIR_X86)
	test -d $(OBJDIR_X86)/client || mkdir -p $(OBJDIR_X86)/client

after_x86: 

x86: before_x86 out_x86 after_x86

out_x86: before_x86 $(OBJ_X86) $(DEP_X86)
	$(LD) -shared $(LIBDIR_X86) $(OBJ_X86)  -o $(OUT_X86) $(LDFLAGS_X86) $(LIB_X86)

$(OBJDIR_X86)/server/openkatana/weapon_trident.o: server/openkatana/weapon_trident.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_trident.c -o $(OBJDIR_X86)/server/openkatana/weapon_trident.o

$(OBJDIR_X86)/server/server_main.o: server/server_main.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_main.c -o $(OBJDIR_X86)/server/server_main.o

$(OBJDIR_X86)/server/server_item.o: server/server_item.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_item.c -o $(OBJDIR_X86)/server/server_item.o

$(OBJDIR_X86)/server/server_gib.o: server/server_gib.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_gib.c -o $(OBJDIR_X86)/server/server_gib.o

$(OBJDIR_X86)/server/server_entity.o: server/server_entity.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_entity.c -o $(OBJDIR_X86)/server/server_entity.o

$(OBJDIR_X86)/server/server_area.o: server/server_area.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_area.c -o $(OBJDIR_X86)/server/server_area.o

$(OBJDIR_X86)/server/openkatana/weapon_zeus.o: server/openkatana/weapon_zeus.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_zeus.c -o $(OBJDIR_X86)/server/openkatana/weapon_zeus.o

$(OBJDIR_X86)/server/openkatana/weapon_sidewinder.o: server/openkatana/weapon_sidewinder.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_sidewinder.c -o $(OBJDIR_X86)/server/openkatana/weapon_sidewinder.o

$(OBJDIR_X86)/server/openkatana/weapon_shotcycler6.o: server/openkatana/weapon_shotcycler6.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_shotcycler6.c -o $(OBJDIR_X86)/server/openkatana/weapon_shotcycler6.o

$(OBJDIR_X86)/server/openkatana/weapon_shockwave.o: server/openkatana/weapon_shockwave.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_shockwave.c -o $(OBJDIR_X86)/server/openkatana/weapon_shockwave.o

$(OBJDIR_X86)/server/openkatana/weapon_pulserifle.o: server/openkatana/weapon_pulserifle.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_pulserifle.c -o $(OBJDIR_X86)/server/openkatana/weapon_pulserifle.o

$(OBJDIR_X86)/server/openkatana/weapon_midas.o: server/openkatana/weapon_midas.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_midas.c -o $(OBJDIR_X86)/server/openkatana/weapon_midas.o

$(OBJDIR_X86)/server/openkatana/weapon_kineticore.o: server/openkatana/weapon_kineticore.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_kineticore.c -o $(OBJDIR_X86)/server/openkatana/weapon_kineticore.o

$(OBJDIR_X86)/server/openkatana/weapon_ionrifle.o: server/openkatana/weapon_ionrifle.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_ionrifle.c -o $(OBJDIR_X86)/server/openkatana/weapon_ionrifle.o

$(OBJDIR_X86)/server/openkatana/weapon_ionblaster.o: server/openkatana/weapon_ionblaster.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_ionblaster.c -o $(OBJDIR_X86)/server/openkatana/weapon_ionblaster.o

$(OBJDIR_X86)/server/server_player.o: server/server_player.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_player.c -o $(OBJDIR_X86)/server/server_player.o

$(OBJDIR_X86)/server/server_weapon.o: server/server_weapon.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_weapon.c -o $(OBJDIR_X86)/server/server_weapon.o

$(OBJDIR_X86)/server/server_waypoint.o: server/server_waypoint.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_waypoint.c -o $(OBJDIR_X86)/server/server_waypoint.o

$(OBJDIR_X86)/server/server_vehicle.o: server/server_vehicle.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_vehicle.c -o $(OBJDIR_X86)/server/server_vehicle.o

$(OBJDIR_X86)/server/server_point.o: server/server_point.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_point.c -o $(OBJDIR_X86)/server/server_point.o

$(OBJDIR_X86)/server/openkatana/weapon_greekfire.o: server/openkatana/weapon_greekfire.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_greekfire.c -o $(OBJDIR_X86)/server/openkatana/weapon_greekfire.o

$(OBJDIR_X86)/server/server_physics.o: server/server_physics.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_physics.c -o $(OBJDIR_X86)/server/server_physics.o

$(OBJDIR_X86)/server/server_monster.o: server/server_monster.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_monster.c -o $(OBJDIR_X86)/server/server_monster.o

$(OBJDIR_X86)/server/server_misc.o: server/server_misc.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/server_misc.c -o $(OBJDIR_X86)/server/server_misc.o

$(OBJDIR_X86)/server/ServerEntity.o: server/ServerEntity.cpp
	$(CXX) $(CFLAGS_X86) $(INC_X86) -c server/ServerEntity.cpp -o $(OBJDIR_X86)/server/ServerEntity.o

$(OBJDIR_X86)/game_main.o: game_main.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c game_main.c -o $(OBJDIR_X86)/game_main.o

$(OBJDIR_X86)/client/client_view.o: client/client_view.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c client/client_view.c -o $(OBJDIR_X86)/client/client_view.o

$(OBJDIR_X86)/client/client_main.o: client/client_main.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c client/client_main.c -o $(OBJDIR_X86)/client/client_main.o

$(OBJDIR_X86)/client/client_effects.o: client/client_effects.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c client/client_effects.c -o $(OBJDIR_X86)/client/client_effects.o

$(OBJDIR_X86)/client/cl_player.o: client/cl_player.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c client/cl_player.c -o $(OBJDIR_X86)/client/cl_player.o

$(OBJDIR_X86)/server/monster_bot.o: server/monster_bot.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/monster_bot.c -o $(OBJDIR_X86)/server/monster_bot.o

$(OBJDIR_X86)/server/openkatana/monster_lasergat.o: server/openkatana/monster_lasergat.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/monster_lasergat.c -o $(OBJDIR_X86)/server/openkatana/monster_lasergat.o

$(OBJDIR_X86)/server/openkatana/weapon_glock.o: server/openkatana/weapon_glock.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_glock.c -o $(OBJDIR_X86)/server/openkatana/weapon_glock.o

$(OBJDIR_X86)/server/openkatana/weapon_discus.o: server/openkatana/weapon_discus.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_discus.c -o $(OBJDIR_X86)/server/openkatana/weapon_discus.o

$(OBJDIR_X86)/server/openkatana/weapon_daikatana.o: server/openkatana/weapon_daikatana.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_daikatana.c -o $(OBJDIR_X86)/server/openkatana/weapon_daikatana.o

$(OBJDIR_X86)/server/openkatana/weapon_c4vizatergo.o: server/openkatana/weapon_c4vizatergo.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_c4vizatergo.c -o $(OBJDIR_X86)/server/openkatana/weapon_c4vizatergo.o

$(OBJDIR_X86)/server/openkatana/weapon_barrier.o: server/openkatana/weapon_barrier.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_barrier.c -o $(OBJDIR_X86)/server/openkatana/weapon_barrier.o

$(OBJDIR_X86)/server/openkatana/weapon_ballista.o: server/openkatana/weapon_ballista.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_ballista.c -o $(OBJDIR_X86)/server/openkatana/weapon_ballista.o

$(OBJDIR_X86)/server/openkatana/weapon_axe.o: server/openkatana/weapon_axe.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/weapon_axe.c -o $(OBJDIR_X86)/server/openkatana/weapon_axe.o

$(OBJDIR_X86)/server/openkatana/monster_prisoner.o: server/openkatana/monster_prisoner.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/monster_prisoner.c -o $(OBJDIR_X86)/server/openkatana/monster_prisoner.o

$(OBJDIR_X86)/server/openkatana/monster_inmater.o: server/openkatana/monster_inmater.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/monster_inmater.c -o $(OBJDIR_X86)/server/openkatana/monster_inmater.o

$(OBJDIR_X86)/server/openkatana/monster_cambot.o: server/openkatana/monster_cambot.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/monster_cambot.c -o $(OBJDIR_X86)/server/openkatana/monster_cambot.o

$(OBJDIR_X86)/server/openkatana/mode_vektar.o: server/openkatana/mode_vektar.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/mode_vektar.c -o $(OBJDIR_X86)/server/openkatana/mode_vektar.o

$(OBJDIR_X86)/server/openkatana/mode_deathmatch.o: server/openkatana/mode_deathmatch.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/mode_deathmatch.c -o $(OBJDIR_X86)/server/openkatana/mode_deathmatch.o

$(OBJDIR_X86)/server/openkatana/mode_ctf.o: server/openkatana/mode_ctf.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/mode_ctf.c -o $(OBJDIR_X86)/server/openkatana/mode_ctf.o

$(OBJDIR_X86)/server/openkatana/decoration_barrel.o: server/openkatana/decoration_barrel.c
	$(CC) $(CFLAGS_X86) $(INC_X86) -c server/openkatana/decoration_barrel.c -o $(OBJDIR_X86)/server/openkatana/decoration_barrel.o

clean_x86: 
	rm -f $(OBJ_X86) $(OUT_X86)
	rm -rf ../bin/openkatana
	rm -rf $(OBJDIR_X86)/server/openkatana
	rm -rf $(OBJDIR_X86)/server
	rm -rf $(OBJDIR_X86)
	rm -rf $(OBJDIR_X86)/client

.PHONY: before_x64 after_x64 clean_x64 before_x86 after_x86 clean_x86

