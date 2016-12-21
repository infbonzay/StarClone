
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
    WARN = -Wno-write-strings
else
    WARN = -Wno-write-strings
endif
if HAVE64
    SUFFIX=64
else
    SUFFIX=32
endif
EXTRA_DIST = bin bugs include obj src
ADDVARS = -DUNDERLINUX 
OPTIM = -m$(SUFFIX)
CFLAGS=$(WARN) $(DEBUG) $(OPTIM) $(ADDVARS) -funsigned-char -Iinclude -Iinclude/network
LDPATH= -L/lib$(SUFFIX) -L/usr/lib$(SUFFIX) -L/usr/local/lib$(SUFFIX)
LFLAGS = $(WARN) $(DEBUG) $(OPTIM) $(LDPATH)

if HAVECPP
    LFLAGS+= -lstdc++
endif
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
    CFLAGS+= -Iinclude/sdl -DWITHSDL
endif
if HAVESDLMIXERLIB
    LFLAGS+= -lSDL_mixer
    CFLAGS+= -DWITHSDLMIXER
    AUDIODIR=sound.sdl
else
    AUDIODIR=sound.null
endif

PROGS = bin/$(PACKAGE)
OBJS =  obj/flingy.o obj/iscript.o obj/images.o obj/music.o obj/myfifo.o obj/triggers.o obj/briefing.o obj/video.o \
	obj/network_general.o obj/network_udp.o obj/network_tcp.o obj/network_dc.o obj/network_modem.o \
	obj/netwgame.o obj/netwplay.o obj/mpqwrapper.o obj/sigorder.o \
	obj/construct.o obj/market.o obj/menu.o obj/gamemenu.o obj/weapons.o \
	obj/mouse.o obj/fog.o obj/auxil.o obj/rules.o obj/key.o obj/lists.o \
	obj/mylist.o obj/mytime.o obj/pcx.o obj/wmem.o obj/tbl.o obj/overlay.o \
	obj/diap.o obj/gener.o obj/gr.o obj/load.o obj/mage.o obj/objlists.o \
	obj/man.o obj/maps.o obj/objs.o obj/putobj.o obj/doodad.o obj/dialogbin.o \
	obj/scripts.o obj/vars.o obj/tempobj.o obj/loadlo.o obj/stars.o obj/fonts.o \
	obj/mpq.o obj/audio.o obj/starmap.o obj/creep.o obj/action.o obj/stringfiles.o \
	obj/unitaction.o obj/queue.o obj/portrait.o obj/actiondefault.o \
	obj/hod.o obj/walk.o obj/pylon.o obj/upgrade.o \
	obj/objinfo.o obj/rand.o obj/vision.o obj/messages.o \
	obj/grsdl.o obj/mousesdl.o obj/audiolowlevel.o obj/scrolltile.o \
	obj/player.o obj/debug.o obj/smacker.o obj/smk_bitstream.o obj/smk_hufftree.o 

$(PROGS): $(OBJS) 
	$(CXX) -o $(PROGS) $(OBJS) $(LFLAGS)

obj/flingy.o: src/flingy.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/flingy.cpp -o obj/flingy.o
	
obj/sigorder.o: src/sigorder.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/sigorder.cpp -o obj/sigorder.o
	
obj/images.o: src/images.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/images.cpp -o obj/images.o
	
obj/iscript.o: src/iscript.cpp src/iscriptcmds.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/iscript.cpp -o obj/iscript.o
	
obj/music.o: src/music.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/music.cpp -o obj/music.o
	
obj/myfifo.o: src/myfifo.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/myfifo.cpp -o obj/myfifo.o
	
obj/video.o: src/video.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/video.cpp -o obj/video.o
	
obj/triggers.o: src/triggers.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/triggers.cpp -o obj/triggers.o
	
obj/briefing.o: src/briefing.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/briefing.cpp -o obj/briefing.o
	
obj/netwgame.o: src/network/netwgame.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/netwgame.cpp -o obj/netwgame.o
	
obj/netwplay.o: src/network/netwplay.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/netwplay.cpp -o obj/netwplay.o
	
obj/network_general.o: src/network/network_general.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_general.cpp -o obj/network_general.o
	
obj/network_modem.o: src/network/network_modem.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_modem.cpp -o obj/network_modem.o
	
obj/network_dc.o: src/network/network_dc.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_dc.cpp -o obj/network_dc.o
	
obj/network_tcp.o: src/network/network_tcp.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_tcp.cpp -o obj/network_tcp.o
	
obj/network_udp.o: src/network/network_udp.cpp include/network/*.h include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/network/network_udp.cpp -o obj/network_udp.o
	
obj/player.o: src/player.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/player.cpp -o obj/player.o
	
obj/stringfiles.o: src/stringfiles.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/stringfiles.cpp -o obj/stringfiles.o
	
obj/dialogbin.o: src/dialogbin.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/dialogbin.cpp -o obj/dialogbin.o
	
obj/debug.o: src/debug.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/debug.cpp -o obj/debug.o
	
obj/construct.o: src/construct.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/construct.cpp -o obj/construct.o
	
obj/overlay.o: src/overlay.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/overlay.cpp -o obj/overlay.o
	
obj/weapons.o: src/weapons.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/weapons.cpp -o obj/weapons.o
	
obj/market.o: src/market.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/market.cpp -o obj/market.o
	
obj/tbl.o: src/tbl.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/tbl.cpp -o obj/tbl.o
	
obj/messages.o: src/messages.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/messages.cpp -o obj/messages.o
	
obj/portrait.o: src/portrait.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/portrait.cpp -o obj/portrait.o
	
obj/stars.o: src/stars.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/stars.cpp -o obj/stars.o
	
obj/doodad.o: src/doodad.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/doodad.cpp -o obj/doodad.o
	
obj/mouse.o: src/mouse.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mouse.cpp -o obj/mouse.o
	
obj/auxil.o: src/auxil.cpp include/*.h Makefile
	$(CXX) -c $(CFLAGS) src/auxil.cpp -o obj/auxil.o
	
obj/rules.o: src/rules.cpp include/*.h Makefile 
	$(CXX) -c $(CFLAGS) src/rules.cpp -o obj/rules.o
	
obj/key.o: src/key.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/key.cpp -o obj/key.o
	
obj/lists.o: src/lists.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/lists.cpp -o obj/lists.o
	
obj/mylist.o: src/mylist.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mylist.cpp -o obj/mylist.o
	
obj/mytime.o: src/mytime.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mytime.cpp -o obj/mytime.o
	
obj/pcx.o: src/pcx.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/pcx.cpp -o obj/pcx.o
	
obj/wmem.o: src/wmem.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/wmem.cpp -o obj/wmem.o
	
obj/diap.o: src/diap.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/diap.cpp -o obj/diap.o
	
obj/gener.o: src/gener.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/gener.cpp -o obj/gener.o
	
obj/gr.o: src/gr.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/gr.cpp -o obj/gr.o
	
obj/load.o: src/load.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/load.cpp -o obj/load.o
	
obj/mage.o: src/mage.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mage.cpp -o obj/mage.o
	
obj/man.o: src/man.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/man.cpp -o obj/man.o
	
obj/maps.o: src/maps.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/maps.cpp -o obj/maps.o
	
obj/objs.o: src/objs.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/objs.cpp -o obj/objs.o
	
obj/putobj.o: src/putobj.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/putobj.cpp -o obj/putobj.o
	
obj/scripts.o: src/scripts.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/scripts.cpp -o obj/scripts.o
	
obj/vars.o: src/vars.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/vars.cpp -o obj/vars.o
	
obj/tempobj.o: src/tempobj.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/tempobj.cpp -o obj/tempobj.o
	
obj/loadlo.o: src/loadlo.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/loadlo.cpp -o obj/loadlo.o
	
obj/mpqwrapper.o: src/mpqwrapper.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mpqwrapper.cpp -o obj/mpqwrapper.o
	
obj/mpq.o: src/mpq.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/mpq.cpp -o obj/mpq.o
	
obj/audio.o: src/audio.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/audio.cpp -o obj/audio.o
	
obj/starmap.o: src/starmap.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/starmap.cpp -o obj/starmap.o
	
obj/creep.o: src/creep.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/creep.cpp -o obj/creep.o
	
obj/action.o: src/action.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/action.cpp -o obj/action.o
	
obj/actiondefault.o: src/actiondefault.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/actiondefault.cpp -o obj/actiondefault.o
	
obj/unitaction.o: src/unitaction.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/unitaction.cpp -o obj/unitaction.o
	
obj/queue.o: src/queue.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/queue.cpp -o obj/queue.o
	
obj/menu.o: src/menu.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/menu.cpp -o obj/menu.o
	
obj/gamemenu.o: src/gamemenu.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/gamemenu.cpp -o obj/gamemenu.o
	
obj/fonts.o: src/fonts.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/fonts.cpp -o obj/fonts.o
	
obj/hod.o: src/hod.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/hod.cpp -o obj/hod.o
	
obj/walk.o: src/walk.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/walk.cpp -o obj/walk.o
	
obj/pylon.o: src/pylon.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/pylon.cpp -o obj/pylon.o
	
obj/upgrade.o: src/upgrade.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/upgrade.cpp -o obj/upgrade.o
	
obj/objinfo.o: src/objinfo.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/objinfo.cpp -o obj/objinfo.o
	
obj/objlists.o: src/objlists.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/objlists.cpp -o obj/objlists.o
	
obj/rand.o: src/rand.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/rand.cpp -o obj/rand.o
	
obj/vision.o: src/vision.cpp include/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/vision.cpp -o obj/vision.o
	
obj/audiolowlevel.o: src/$(AUDIODIR)/audiolowlevel.cpp include/audiolowlevel.h  Makefile
	$(CXX) -c $(CFLAGS) $(shell sdl-config --cflags) src/$(AUDIODIR)/audiolowlevel.cpp -o obj/audiolowlevel.o
	
obj/grsdl.o: src/sdl/grsdl.cpp include/*.h include/sdl/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/sdl/grsdl.cpp -o obj/grsdl.o
	
obj/mousesdl.o: src/sdl/mousesdl.cpp include/*.h include/sdl/*.h  Makefile
	$(CXX) -c $(CFLAGS) src/sdl/mousesdl.cpp -o obj/mousesdl.o
	
obj/fog.o: src/fog.cpp include/*.h Makefile
	$(CXX) -c $(CFLAGS) src/fog.cpp -o obj/fog.o
	
obj/scrolltile.o: src/scrolltile.cpp include/*.h Makefile
	$(CXX) -c $(CFLAGS) src/scrolltile.cpp -o obj/scrolltile.o
	
obj/smacker.o: src/smacker/smacker.cpp include/smacker/*.h Makefile
	$(CXX) -c $(CFLAGS) src/smacker/smacker.cpp -o obj/smacker.o
	
obj/smk_bitstream.o: src/smacker/smk_bitstream.cpp include/smacker/*.h Makefile
	$(CXX) -c $(CFLAGS) src/smacker/smk_bitstream.cpp -o obj/smk_bitstream.o
	
obj/smk_hufftree.o: src/smacker/smk_hufftree.cpp include/smacker/*.h Makefile
	$(CXX) -c $(CFLAGS) src/smacker/smk_hufftree.cpp -o obj/smk_hufftree.o
	
all: $(OBJS) $(PROGS) Makefile

clean: 
	-rm $(OBJS) $(PROGS)
#	-rm Makefile Makefile.in aclocal.m4 config.log config.status configure install-sh missing 2>/dev/null
#	-rm -r autom4te.cache 2>/dev/null
