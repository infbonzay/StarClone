
prefix=@prefix@
exec_prefix=@exec_prefix@
INSTDIR=@INSTDIR@
CXX=@CXX@
if HAVEDEBUG
    DEBUG = -g3 -O0 -DDEBUG
else
    DEBUG = -O2
endif
if HAVEWARNING
    WARN = -Wall
else
    WARN = 
endif
if SYS64
if FORCE32BIT
  SUFFIX=32
  OPTIM=-m32
else
  SUFFIX=
  OPTIM=-m64
endif
else
    SUFFIX=
    OPTIM=
endif
EXTRA_DIST = bin bugs include obj src
ADDVARS = -DUNDERLINUX 
CHARTYPE = #-funsigned-char
CFLAGS=$(WARN) $(DEBUG) $(OPTIM) $(ADDVARS) $(CHARTYPE) -Iinclude -Iinclude/network
if HAVELOCALLIBS
    LDPATH= -L$(abs_builddir)/libs -L/lib$(SUFFIX) -L/usr/lib$(SUFFIX) -L/usr/local/lib$(SUFFIX)
else
    LDPATH= -L/lib$(SUFFIX) -L/usr/lib$(SUFFIX) -L/usr/local/lib$(SUFFIX)
endif
LFLAGS = $(WARN) $(DEBUG) $(OPTIM) $(LDPATH)

CFLAGS+= -Iinclude/video

if HAVELOCALLIBS
    LFLAGS+= -l:libs/librt-2.15.so -l:libs/libz.so.1.2.7  -l:libs/libgrp-0.4.so.18.0.0 -l:libs/libmpq-0.5.so.3.0.0 \
    -l:libs/libSDL-1.2.so.0.11.4 -l:libs/libSDL_mixer-1.2.so.0.12.0 -l:libs/libstdc++.so.6.0.24 -l:libs/libc-2.16.so \
    -l:libs/libgcc_s.so.1 -l:libs/libm.so.6  -l:libs/libdl-2.16.so
    CFLAGS+= -DWITHMPQLIB -Iinclude/sdl -DWITHSDL -DWITHSDLMIXER
    AUDIODIR=sound.sdl
else
if HAVERT
    LFLAGS+= -lrt
endif
if HAVEGRPLIB
    LFLAGS+= -lgrp
endif
if HAVEMPQLIB
    LFLAGS+= -lmpq
    CFLAGS+= -DWITHMPQLIB
endif
if HAVESTORMLIB
    LFLAGS+= -lStorm
    CFLAGS+= -DWITHSTORMLIB
endif
if HAVEZLIB
    LFLAGS+= -lz
endif
if HAVESDLLIB
    LFLAGS+= -lSDL
    CFLAGS+= -DWITHSDL
endif
if HAVEXLIB
    LFLAGS+= -lx11
    CFLAGS+= -DWITHX
endif
if HAVESDLMIXERLIB
    LFLAGS+= -lSDL_mixer
    CFLAGS+= -DWITHSDLAUDIO
    AUDIOFILE=src/audio/sdl/audiolib.cpp
endif
if HAVENOAUDIO
    CFLAGS+= -DWITHNOAUDIO
    AUDIOFILE=src/audio/none/audiolib.cpp
endif
endif
PROGS = bin/$(PACKAGE)
OBJS = obj/Controller.cpp.o obj/RegenObjMap.cpp.o obj/ScreenDraw.cpp.o obj/MenuItem.cpp.o obj/gamemenu.cpp.o obj/commandqueue.cpp.o \
	obj/flingy.cpp.o obj/iscript.cpp.o obj/images.cpp.o obj/music.cpp.o obj/triggers.cpp.o obj/briefing.cpp.o obj/video.cpp.o \
	obj/network_general.cpp.o obj/network_udp.cpp.o obj/network_tcp.cpp.o obj/network_dc.cpp.o obj/network_modem.cpp.o obj/network_dedicated.cpp.o \
	obj/netwgame.cpp.o obj/netwplay.cpp.o obj/mpqwrapper.cpp.o obj/sigorder.cpp.o \
	obj/construct.cpp.o obj/market.cpp.o obj/menu.cpp.o obj/weapons.cpp.o \
	obj/mouse.cpp.o obj/fog.cpp.o obj/auxil.cpp.o obj/rules.cpp.o obj/key.cpp.o obj/lists.cpp.o \
	obj/mylist.cpp.o obj/mytime.cpp.o obj/pcx.cpp.o obj/wmem.cpp.o obj/tbl.cpp.o obj/overlay.cpp.o \
	obj/diap.cpp.o obj/gener.cpp.o obj/gr.cpp.o obj/load.cpp.o obj/mage.cpp.o obj/objlists.cpp.o \
	obj/man.cpp.o obj/maps.cpp.o obj/objs.cpp.o obj/putobj.cpp.o obj/doodad.cpp.o obj/dialogbin.cpp.o \
	obj/scripts.cpp.o obj/vars.cpp.o obj/tempobj.cpp.o obj/loadlo.cpp.o obj/stars.cpp.o obj/fonts.cpp.o \
	obj/mpq.cpp.o obj/audio.cpp.o obj/starmap.cpp.o obj/creep.cpp.o obj/action.cpp.o obj/stringfiles.cpp.o \
	obj/unitaction.cpp.o obj/queue.cpp.o obj/portrait.cpp.o obj/actiondefault.cpp.o \
	obj/hod.cpp.o obj/walk.cpp.o obj/pylon.cpp.o obj/upgrade.cpp.o \
	obj/objinfo.cpp.o obj/rand.cpp.o obj/vision.cpp.o obj/messages.cpp.o \
	obj/audiolib.cpp.o obj/scrolltile.cpp.o \
	obj/player.cpp.o obj/debug.cpp.o obj/smacker.cpp.o obj/smk_bitstream.cpp.o obj/smk_hufftree.cpp.o

$(PROGS): $(OBJS) 
	$(CXX) -o $(PROGS) $(OBJS) $(LFLAGS)

obj/RegenObjMap.cpp.o: src/RegenObjMap.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/RegenObjMap.cpp -o obj/RegenObjMap.cpp.o
	
obj/List.cpp.o: src/List.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/List.cpp -o obj/List.cpp.o
	
obj/ScreenDraw.cpp.o: src/ScreenDraw.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/ScreenDraw.cpp -o obj/ScreenDraw.cpp.o
	
obj/MenuItem.cpp.o: src/MenuItem.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/MenuItem.cpp -o obj/MenuItem.cpp.o
	
obj/mycycle.cpp.o: src/mycycle.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mycycle.cpp -o obj/mycycle.cpp.o
	
obj/commandqueue.cpp.o: src/commandqueue.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/commandqueue.cpp -o obj/commandqueue.cpp.o
	
obj/flingy.cpp.o: src/flingy.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/flingy.cpp -o obj/flingy.cpp.o
	
obj/sigorder.cpp.o: src/sigorder.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/sigorder.cpp -o obj/sigorder.cpp.o
	
obj/images.cpp.o: src/images.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/images.cpp -o obj/images.cpp.o
	
obj/iscript.cpp.o: src/iscript.cpp src/iscriptcmds.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/iscript.cpp -o obj/iscript.cpp.o
	
obj/music.cpp.o: src/music.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/music.cpp -o obj/music.cpp.o
	
obj/video.cpp.o: src/video.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/video.cpp -o obj/video.cpp.o
	
obj/triggers.cpp.o: src/triggers.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/triggers.cpp -o obj/triggers.cpp.o
	
obj/briefing.cpp.o: src/briefing.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/briefing.cpp -o obj/briefing.cpp.o
	
obj/netwgame.cpp.o: src/network/netwgame.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/netwgame.cpp -o obj/netwgame.cpp.o
	
obj/netwplay.cpp.o: src/network/netwplay.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/netwplay.cpp -o obj/netwplay.cpp.o
	
obj/network_general.cpp.o: src/network/network_general.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_general.cpp -o obj/network_general.cpp.o
	
obj/network_modem.cpp.o: src/network/network_modem.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_modem.cpp -o obj/network_modem.cpp.o
	
obj/network_dc.cpp.o: src/network/network_dc.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_dc.cpp -o obj/network_dc.cpp.o
	
obj/network_tcp.cpp.o: src/network/network_tcp.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_tcp.cpp -o obj/network_tcp.cpp.o
	
obj/network_udp.cpp.o: src/network/network_udp.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_udp.cpp -o obj/network_udp.cpp.o
	
obj/network_dedicated.cpp.o: src/network/network_dedicated.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_dedicated.cpp -o obj/network_dedicated.cpp.o
	
obj/player.cpp.o: src/player.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/player.cpp -o obj/player.cpp.o
	
obj/stringfiles.cpp.o: src/stringfiles.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/stringfiles.cpp -o obj/stringfiles.cpp.o
	
obj/dialogbin.cpp.o: src/dialogbin.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/dialogbin.cpp -o obj/dialogbin.cpp.o
	
obj/debug.cpp.o: src/debug.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/debug.cpp -o obj/debug.cpp.o
	
obj/construct.cpp.o: src/construct.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/construct.cpp -o obj/construct.cpp.o
	
obj/overlay.cpp.o: src/overlay.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/overlay.cpp -o obj/overlay.cpp.o
	
obj/weapons.cpp.o: src/weapons.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/weapons.cpp -o obj/weapons.cpp.o
	
obj/market.cpp.o: src/market.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/market.cpp -o obj/market.cpp.o
	
obj/tbl.cpp.o: src/tbl.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/tbl.cpp -o obj/tbl.cpp.o
	
obj/messages.cpp.o: src/messages.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/messages.cpp -o obj/messages.cpp.o
	
obj/portrait.cpp.o: src/portrait.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/portrait.cpp -o obj/portrait.cpp.o
	
obj/stars.cpp.o: src/stars.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/stars.cpp -o obj/stars.cpp.o
	
obj/doodad.cpp.o: src/doodad.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/doodad.cpp -o obj/doodad.cpp.o
	
obj/mouse.cpp.o: src/mouse.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mouse.cpp -o obj/mouse.cpp.o
	
obj/auxil.cpp.o: src/auxil.cpp include/*.h Makefile
	$(CXX) -c $(CFLAGS) src/auxil.cpp -o obj/auxil.cpp.o
	
obj/rules.cpp.o: src/rules.cpp include/*.h Makefile 
	$(CXX) -c $(CFLAGS) src/rules.cpp -o obj/rules.cpp.o
	
obj/key.cpp.o: src/key.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/key.cpp -o obj/key.cpp.o
	
obj/lists.cpp.o: src/lists.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/lists.cpp -o obj/lists.cpp.o
	
obj/mylist.cpp.o: src/mylist.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mylist.cpp -o obj/mylist.cpp.o
	
obj/mytime.cpp.o: src/mytime.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mytime.cpp -o obj/mytime.cpp.o
	
obj/pcx.cpp.o: src/pcx.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/pcx.cpp -o obj/pcx.cpp.o
	
obj/wmem.cpp.o: src/wmem.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/wmem.cpp -o obj/wmem.cpp.o
	
obj/diap.cpp.o: src/diap.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/diap.cpp -o obj/diap.cpp.o
	
obj/gener.cpp.o: src/gener.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/gener.cpp -o obj/gener.cpp.o
	
obj/gr.cpp.o: src/gr.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/gr.cpp -o obj/gr.cpp.o
	
obj/load.cpp.o: src/load.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/load.cpp -o obj/load.cpp.o
	
obj/mage.cpp.o: src/mage.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mage.cpp -o obj/mage.cpp.o
	
obj/man.cpp.o: src/man.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/man.cpp -o obj/man.cpp.o
	
obj/maps.cpp.o: src/maps.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/maps.cpp -o obj/maps.cpp.o
	
obj/objs.cpp.o: src/objs.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/objs.cpp -o obj/objs.cpp.o
	
obj/putobj.cpp.o: src/putobj.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/putobj.cpp -o obj/putobj.cpp.o
	
obj/scripts.cpp.o: src/scripts.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/scripts.cpp -o obj/scripts.cpp.o
	
obj/vars.cpp.o: src/vars.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/vars.cpp -o obj/vars.cpp.o
	
obj/tempobj.cpp.o: src/tempobj.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/tempobj.cpp -o obj/tempobj.cpp.o
	
obj/loadlo.cpp.o: src/loadlo.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/loadlo.cpp -o obj/loadlo.cpp.o
	
obj/mpqwrapper.cpp.o: src/mpqwrapper.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mpqwrapper.cpp -o obj/mpqwrapper.cpp.o
	
obj/mpq.cpp.o: src/mpq.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mpq.cpp -o obj/mpq.cpp.o
	
obj/audio.cpp.o: src/audio.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/audio.cpp -o obj/audio.cpp.o
	
obj/starmap.cpp.o: src/starmap.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/starmap.cpp -o obj/starmap.cpp.o
	
obj/creep.cpp.o: src/creep.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/creep.cpp -o obj/creep.cpp.o
	
obj/action.cpp.o: src/action.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/action.cpp -o obj/action.cpp.o
	
obj/actiondefault.cpp.o: src/actiondefault.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/actiondefault.cpp -o obj/actiondefault.cpp.o
	
obj/unitaction.cpp.o: src/unitaction.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/unitaction.cpp -o obj/unitaction.cpp.o
	
obj/queue.cpp.o: src/queue.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/queue.cpp -o obj/queue.cpp.o
	
obj/menu.cpp.o: src/menu.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/menu.cpp -o obj/menu.cpp.o
	
obj/gamemenu.cpp.o: src/gamemenu.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/gamemenu.cpp -o obj/gamemenu.cpp.o
	
obj/fonts.cpp.o: src/fonts.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/fonts.cpp -o obj/fonts.cpp.o
	
obj/hod.cpp.o: src/hod.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/hod.cpp -o obj/hod.cpp.o
	
obj/walk.cpp.o: src/walk.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/walk.cpp -o obj/walk.cpp.o
	
obj/pylon.cpp.o: src/pylon.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/pylon.cpp -o obj/pylon.cpp.o
	
obj/upgrade.cpp.o: src/upgrade.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/upgrade.cpp -o obj/upgrade.cpp.o
	
obj/objinfo.cpp.o: src/objinfo.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/objinfo.cpp -o obj/objinfo.cpp.o
	
obj/objlists.cpp.o: src/objlists.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/objlists.cpp -o obj/objlists.cpp.o
	
obj/rand.cpp.o: src/rand.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/rand.cpp -o obj/rand.cpp.o
	
obj/vision.cpp.o: src/vision.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/vision.cpp -o obj/vision.cpp.o
	
obj/audiolib.cpp.o: $(AUDIOFILE) include/audiolowlevel.h  Makefile
	$(CXX) -c $(CFLAGS) $(shell sdl-config --cflags) $(AUDIOFILE) -o obj/audiolib.cpp.o
	
obj/Controller.cpp.o: src/video/sdl/Controller.cpp include/*.h Makefile
	$(CXX) -c $(CFLAGS) src/video/sdl/Controller.cpp -o obj/Controller.cpp.o
	
obj/fog.cpp.o: src/fog.cpp include/*.h Makefile
	$(CXX) -c $(CFLAGS) src/fog.cpp -o obj/fog.cpp.o
	
obj/scrolltile.cpp.o: src/scrolltile.cpp include/*.h Makefile
	$(CXX) -c $(CFLAGS) src/scrolltile.cpp -o obj/scrolltile.cpp.o
	
obj/smacker.cpp.o: src/smacker/smacker.cpp include/smacker/*.h Makefile
	$(CXX) -c $(CFLAGS) src/smacker/smacker.cpp -o obj/smacker.cpp.o
	
obj/smk_bitstream.cpp.o: src/smacker/smk_bitstream.cpp include/smacker/*.h Makefile
	$(CXX) -c $(CFLAGS) src/smacker/smk_bitstream.cpp -o obj/smk_bitstream.cpp.o
	
obj/smk_hufftree.cpp.o: src/smacker/smk_hufftree.cpp include/smacker/*.h Makefile
	$(CXX) -c $(CFLAGS) src/smacker/smk_hufftree.cpp -o obj/smk_hufftree.cpp.o
	
all: $(OBJS) $(PROGS) Makefile

clean: 
	-rm $(OBJS) $(PROGS)
#	-rm Makefile Makefile.in aclocal.m4 config.log config.status configure install-sh missing 2>/dev/null
#	-rm -r autom4te.cache 2>/dev/null
