CFLAGS   := -Wall -fPIC -ggdb
CXXFLAGS := $(CFLAGS) -std=c++11

INCLUDE := -I./platform/include/ -I./shared/ $(shell pkg-config sdl2 --cflags)

LAUNCHER_BIN  := bin/launcher
LAUNCHER_OBJS := launcher/launcher.o
LAUNCHER_LIBS := -ldl -lX11 -lpthread

PLATFORM_OBJS := platform/platform.o \
	platform/platform_filesystem.o \
	platform/platform_input.o \
	platform/platform_log.o \
	platform/platform_math.o \
	platform/platform_module.o \
	platform/platform_net.o \
	platform/platform_string.o \
	platform/platform_window.o

ENGINE_BIN  := bin/engine.x64.so
ENGINE_OBJS := engine/cmdlib.o \
	engine/conproc.o \
	engine/crc.o \
	engine/EngineAudio.o \
	engine/EngineAudioDirectSound.o \
	engine/EngineAudioDMA.o \
	engine/EngineAudioDummy.o \
	engine/EngineAudioMem.o \
	engine/EngineAudioMix.o \
	engine/engine_client.o \
	engine/EngineClient.o \
	engine/EngineClientChase.o \
	engine/EngineClientDemo.o \
	engine/EngineClientInput.o \
	engine/EngineClientParse.o \
	engine/EngineClientTempEntity.o \
	engine/EngineClientView.o \
	engine/EngineCommon.o \
	engine/EngineConsole.o \
	engine/EngineConsoleCommand.o \
	engine/EngineConsoleVariable.o \
	engine/EngineEditor.o \
	engine/EngineGame.o \
	engine/EngineHost.o \
	engine/EngineHostCommand.o \
	engine/EngineImage.o \
	engine/EngineInput.o \
	engine/EngineMain.o \
	engine/EngineMaterialEditor.o \
	engine/EngineMenu.o \
	engine/EngineModel.o \
	engine/engine_netdgrm.o \
	engine/EngineNetwork.o \
	engine/EngineNetworkDummy.o \
	engine/EngineNetworkLoop.o \
	engine/EngineScript.o \
	engine/EngineServer.o \
	engine/EngineServerEntity.o \
	engine/EngineServerPhysics.o \
	engine/EngineServerUser.o \
	engine/EngineServerWorld.o \
	engine/EngineVideoAlias.o \
	engine/EngineVideoEFrag.o \
	engine/EngineVideoFog.o \
	engine/EngineVideoFont.o \
	engine/EngineVideoLegacy2.o \
	engine/EngineVideoLegacy.o \
	engine/EngineVideoLights.o \
	engine/EngineVideoParticle.o \
	engine/EngineVideoScreen.o \
	engine/EngineVideoShadow.o \
	engine/EngineVideoSky.o \
	engine/EngineVideoSprite.o \
	engine/EngineVideoTextureManager.o \
	engine/EngineVideoWarp.o \
	engine/EngineVideoWorld.o \
	engine/keys.o \
	engine/material.o \
	engine/menu.o \
	engine/sys_win.o \
	engine/video.o \
	engine/video_brush.o \
	engine/video_draw.o \
	engine/video_layer.o \
	engine/video_sdlwindow.o \
	engine/zone.o \
	engine/engine_exception.o \
	engine/model_u3d.o \
	engine/shader_base.o \
	engine/video_framebuffer.o \
	engine/video_object.o \
	engine/video_shader.o \
	engine/client/effect_particle.o \
	engine/client/effect_sprite.o \
	shared/SharedFormats.o
ENGINE_LIBS := -lglee $(shell pkg-config sdl2 --libs)

MENU_BIN  := bin/menu.x64.so
MENU_OBJS := menu/menu_hud.o \
	menu/menu_main.o \
	menu/menu_panel.o
MENU_LIBS :=

GAME_BIN  := bin/game.x64.so
GAME_OBJS := game/client/client.o \
	game/client/client_effects.o \
	game/client/client_player.o \
	game/client/client_view.o \
	game/client/menu/menu.o \
	game/game.o \
	game/server/monster_bot.o \
	game/server/openkatana/decoration_barrel.o \
	game/server/openkatana/item_health.o \
	game/server/openkatana/mode_ctf.o \
	game/server/openkatana/mode_deathmatch.o \
	game/server/openkatana/monster_cambot.o \
	game/server/openkatana/monster_inmater.o \
	game/server/openkatana/monster_lasergat.o \
	game/server/openkatana/monster_prisoner.o \
	game/server/openkatana/weapon_axe.o \
	game/server/openkatana/weapon_ballista.o \
	game/server/openkatana/weapon_barrier.o \
	game/server/openkatana/weapon_c4vizatergo.o \
	game/server/openkatana/weapon_crossbow.o \
	game/server/openkatana/weapon_daikatana.o \
	game/server/openkatana/weapon_discus.o \
	game/server/openkatana/weapon_glock.o \
	game/server/openkatana/weapon_greekfire.o \
	game/server/openkatana/weapon_hermes.o \
	game/server/openkatana/weapon_ionblaster.o \
	game/server/openkatana/weapon_ionrifle.o \
	game/server/openkatana/weapon_kineticore.o \
	game/server/openkatana/weapon_midas.o \
	game/server/openkatana/weapon_pulserifle.o \
	game/server/openkatana/weapon_shockwave.o \
	game/server/openkatana/weapon_shotcycler6.o \
	game/server/openkatana/weapon_sidewinder.o \
	game/server/openkatana/weapon_trident.o \
	game/server/openkatana/weapon_zeus.o \
	game/server/server_animated.o \
	game/server/server_area.o \
	game/server/server.o \
	game/server/server_debug.o \
	game/server/server_effects.o \
	game/server/server_ent.o \
	game/server/server_entity.o \
	game/server/server_gib.o \
	game/server/server_item.o \
	game/server/server_menu.o \
	game/server/server_misc.o \
	game/server/server_monster.o \
	game/server/server_physics.o \
	game/server/server_player.o \
	game/server/server_point.o \
	game/server/server_vehicle.o \
	game/server/server_waypoint.o \
	game/server/server_weapon.o
GAME_LIBS :=

all: $(LAUNCHER_BIN) $(ENGINE_BIN) $(MENU_BIN) $(GAME_BIN)

clean:
	rm -f $(LAUNCHER_BIN) $(LAUNCHER_OBJS)
	rm -f                 $(PLATFORM_OBJS)
	rm -f $(ENGINE_BIN)   $(ENGINE_OBJS)
	rm -f $(MENU_BIN)     $(MENU_OBJS)
	rm -f $(GAME_BIN)     $(GAME_OBJS)

platform/%.o: platform/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

shared/%.o: shared/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

launcher/launcher.o: launcher/launcher.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(LAUNCHER_BIN): $(LAUNCHER_OBJS) $(PLATFORM_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LAUNCHER_LIBS)

engine/%.o: engine/%.c
	$(CC) $(CFLAGS) -DKATANA -I./engine/ -I./ $(INCLUDE) -c -o $@ $<

engine/%.o: engine/%.cpp
	$(CXX) $(CXXFLAGS) -DKATANA -I./engine/ -I./ $(INCLUDE) -c -o $@ $<

$(ENGINE_BIN): $(ENGINE_OBJS) $(PLATFORM_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(ENGINE_LIBS)

menu/%.o: menu/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

menu/%.o: menu/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(MENU_BIN): $(MENU_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(MENU_LIBS)

GAME_CFLAGS := -DGAME_OPENKATANA -DOPENKATANA -I./game/ -I./game/client/ -I./game/server/

game/%.o: game/%.c
	$(CC) $(CFLAGS) $(GAME_CFLAGS) $(INCLUDE) -c -o $@ $<

game/%.o: game/%.cpp
	$(CXX) $(CXXFLAGS) $(GAME_CFLAGS) $(INCLUDE) -c -o $@ $<

$(GAME_BIN): $(GAME_OBJS) $(PLATFORM_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(GAME_LIBS)
