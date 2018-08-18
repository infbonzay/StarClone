#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(src/gener.cpp)
AM_INIT_AUTOMAKE(StarClone,0.3.3)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG(C++)
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

    autodetectmpqstorm=no
    AC_ARG_ENABLE([sdlmixer],
	[ --disable-sdlmixer     Disable Audio (Enabled by Default) ],
    	[ withaudio=no ],
	[ withaudio=yes ])
    AC_ARG_ENABLE([bit32],
	[ --enable-bit32         Enable 32-bit compile (Only for 64 bit multilib system) ],
    	[ with32=yes ],
	[ with32=no ])
    AC_ARG_ENABLE([debug],
	[ --enable-debug         Enable debugging (Disabled by Default) ],
    	[ debug=yes ],
	[ debug=no ])
    AC_ARG_ENABLE([warnings],
	[ --enable-warnings         Enable warnings (Disabled by Default) ],
    	[ warning=yes ],
	[ warning=no ])

    AC_ARG_ENABLE([locallibs],
	[ --enable-locallibs      linking with compiled libs in ./bin/libs directory (Enabled By Default)],
    	[ locallibs=yes ],
	[ locallibs=no ])

    AC_ARG_ENABLE([stormlib],
	[ --enable-stormlib      Enable compiling with StormLib ],
    	[ stormlib=yes mpqlib=no ],
	[ stormlib=no])

    AC_ARG_ENABLE([mpqlib],
	[ --enable-mpqlib        Enable compiling with mpqlib ],
    	[ mpqlib=yes stormlib=no],
	[ mpqlib=no ])
    
    AC_CHECK_LIB(stdc++, main, [HAVECPP="yes"],[HAVECPP="no"])
#    AC_CHECK_LIB(z, zlibVersion, [HAVEZLIB="yes"],
#	    AC_MSG_ERROR([zlibVersion was not found in libz]))

    if test "$mpqlib" = "no" ; then 
	if test "$stormlib" = "no" ; then 
	    autodetectmpqstorm=yes
	fi
    fi

checkver() 
{
	local needver="$1"
	local curver="$2"
	local mult="$3"
	local ver1=`echo $needver | cut -b1-1`
	local ver2=`echo $needver | cut -b3-3`
	local ver3=`echo $needver | cut -b5-`
	local numver=`echo $(( ((( $ver1 * $mult ) + $ver2 ) * $mult + $ver3 ) ))`
#	echo $needver $curver $numver
	ver3=`echo $((curver%$mult))`
	local curver2=`echo $((curver/$mult))`
	ver2=`echo $((curver2%$mult))`
	curver2=`echo $((curver2/$mult))`
	ver1=`echo $((curver2%$mult))`
	strver="$ver1.$ver2.$ver3"
	if test $curver -ge $numver ; then
	    echo "yes. Current version is $strver"
	    return 0
	else
	    echo "no. Current version is $strver"
	    return -1
	fi
}

AC_CHECK_SIZEOF([void *])

SIZEOF_VOID_P=$ac_cv_sizeof_void_p

if test "$SIZEOF_VOID_P" == "4"; then
    sys64="no"
    with32="no"
fi
if test "$SIZEOF_VOID_P" == "8"; then
    sys64="yes"
fi

if test "$locallibs" = "no" ; then
    
    COMPILEVER=no
		_libs=$LIBS
		LIBS="$LIBS -lz"
	    AC_LINK_IFELSE([
	        #include <stdio.h>
	        #include <stdlib.h>
		#include <zlib.h>
	        int main(void){
		    printf("%u",ZLIB_VERNUM>>4);
		    exit(0);
	        }
	     ],
	     [zcurver=`./conftest$EXEEXT` COMPILEVER="yes"],
	     [zcurver="0" COMPILEVER="no"]
	    )
    LIBS=$_libs
    NEEDVER="1.0.2"
    echo -n "checking for libz >= $NEEDVER..."
    if test "$COMPILEVER" = "yes"; then
	checkver $NEEDVER $zcurver 16
	result="$?"
	if test "$result" = "0"; then
	    zstrver=$strver
	    HAVEZLIB="yes"
	else
	    exit -1
	fi
    else
	echo "no. no any version installed"
	exit -1
    fi

    COMPILEVER=no
		_libs=$LIBS
		LIBS="$LIBS -lrt"
	    AC_LINK_IFELSE([
	        #include <stdio.h>
	        #include <stdlib.h>
		#include <time.h>
	        int main(void){
		    #ifdef _POSIX_C_SOURCE >= 199309L		    
			clock_gettime(CLOCK_MONOTONIC,NULL);
		    #else
			#error "rt is to old"
		    #endif
		    exit(0);
	        }
	     ],
	     [zcurver=`./conftest$EXEEXT` COMPILEVER="yes"],
	     [zcurver="0" COMPILEVER="no"]
	    )
    LIBS=$_libs
    echo -n "checking for rt "
    if test "$COMPILEVER" = "yes"; then
	HAVERT="yes"
	echo "yes"
    else
	echo "no"
	exit -1
    fi

    COMPILEVER=no
		_libs=$LIBS
		LIBS="$LIBS -lgrp"
	    AC_LINK_IFELSE([
	        #include <stdio.h>
	        #include <stdlib.h>
		#include <grplib/grp.h>
	        int main(void){
		    printf("%u",GetGRPLibVer());
		    exit(0);
	        }
	     ],
	     [grpcurver=`./conftest$EXEEXT` COMPILEVER="yes"],
	     [grpcurver="0" COMPILEVER="no"]
	    )
    LIBS=$_libs
    NEEDVER="0.4.20"
    echo -n "checking for libgrp [$curver] >= $NEEDVER..."
    if test "$COMPILEVER" = "yes"; then
	checkver $NEEDVER $grpcurver 256
	result="$?"
	if test "$result" = "0"; then
	    grpstrver=$strver
	    HAVEGRPLIB="yes"
	else
	    exit -1
	fi
    else

#	AC_OUTPUT(neededlibs/grplib/Makefile)
	
	echo "no. no any version installed"
	exit -1
    fi
    if test "$mpqlib" = "yes" || test "$autodetectmpqstorm" = "yes" ; then 
	COMPILEVER=no
		_libs=$LIBS
		LIBS="$LIBS -lmpq -lz"
	    AC_LINK_IFELSE([
	        #include <stdio.h>
	        #include <stdlib.h>
		#include <mpqlib/StormLib.h>
	        int main(void){
		    printf("%u",GetMPQLibVer());
		    exit(0);
	        }
	     ],
	     [mpqcurver=`./conftest$EXEEXT` COMPILEVER="yes"],
	     [mpqcurver="0" COMPILEVER="no"]
	    )
    fi
    LIBS=$_libs
    if test "$mpqlib" = "yes" || test "$autodetectmpqstorm" = "yes" ; then 
	NEEDVER="0.5.3"
	echo -n "checking for libmpq >= $NEEDVER..."
	if test "$COMPILEVER" = "yes"; then
	    checkver $NEEDVER $mpqcurver 256
	    result="$?"
	    if test "$result" = "0"; then
		HAVEMPQLIB="yes"
		mpqstrver=$strver
	    else
		if test "$autodetectmpqstorm" != "yes" ; then 
		    exit -1
		fi
		HAVEMPQLIB="no"
	    fi
	else
	    HAVEMPQLIB="no"
	    echo "no. no any version installed"
	    if test "$autodetectmpqstorm" != "yes" ; then 
		exit -1
	    fi
	fi
    fi
    if test "$stormlib" = "yes" || test "$autodetectmpqstorm" = "yes" ; then 
	COMPILEVER=no
		_libs=$LIBS
		LIBS="$LIBS -lStorm"
	    AC_LINK_IFELSE([
	        #include <stdio.h>
	        #include <stdlib.h>
		#include <StormLib/StormLib.h>
	        int main(void){
		    printf("%01u%02u%02u",STORMLIB_VERSION/256,STORMLIB_VERSION%256,0);
		    exit(0);
	        }
	     ],
	     [stormcurver=`./conftest$EXEEXT` COMPILEVER="yes"],
	     [stormcurver="0" COMPILEVER="no"]
	    )
    fi
    LIBS=$_libs
    if test "$stormlib" = "yes" || test "$autodetectmpqstorm" = "yes" ; then 
	NEEDVER="7.0.0"
	echo -n "checking for StormLib >= $NEEDVER..."
	if test "$COMPILEVER" = "yes"; then
	    checkver $NEEDVER $stormcurver 100
	    if test "$?" = "0"; then
		HAVESTORMLIB="yes"
		stormstrver=$strver
	    else
		if test "$autodetectmpqstorm" != "yes" ; then 
		    exit -1
		fi
		HAVESTORMLIB="no"
	    fi
	else
	    HAVESTORMLIB="no"
	    echo "no. no any version installed"
	    if test "$autodetectmpqstorm" != "yes" ; then 
		exit -1
	    fi
	fi
    fi
    if test "$HAVEMPQLIB" = "yes" && test "$HAVESTORMLIB" = "yes" ; then
	echo "Both mpqlib StormLib detected."
	echo "Using mpqlib."
	echo "if you want to use StormLib please add --enable-stormlib parameter"
	HAVESTORMLIB="no"
    fi
    if test "$HAVEMPQLIB" = "no" && test "$HAVESTORMLIB" = "no" ; then
	echo "No mpqlib or StormLib detected."
	exit -1
    fi
    COMPILEVER=no
		_libs=$LIBS
		LIBS="$LIBS -lSDL"
	    AC_LINK_IFELSE([
	        #include <stdio.h>
	        #include <stdlib.h>
		#include <SDL/SDL_version.h>
	        int main(void){
		    printf("%01u%02u%02u",SDL_MAJOR_VERSION,SDL_MINOR_VERSION,SDL_PATCHLEVEL);
		    exit(0);
	        }
	     ],
	     [sdlcurver=`./conftest$EXEEXT` COMPILEVER="yes"],
	     [sdlcurver="0" COMPILEVER="no"]
	    )
    LIBS=$_libs
    NEEDVER="1.2.5"
    echo -n "checking for libSDL >= $NEEDVER..."
    if test "$COMPILEVER" = "yes"; then
	checkver $NEEDVER $sdlcurver 100
	if test "$?" = "0"; then
	    HAVESDLLIB="yes"
	    sdlstrver=$strver
	else
	    exit -1
	fi
    else
	echo "no. no any version installed"
	exit -1
    fi

    if test "$withaudio" = "yes"; then
	COMPILEVER=no
		_libs=$LIBS
		LIBS="$LIBS -lSDL_mixer"
	    AC_LINK_IFELSE([
	        #include <stdio.h>
	        #include <stdlib.h>
		#include <SDL/SDL_mixer.h>
	        int main(void){
		    printf("%01u%02u%02u",SDL_MIXER_MAJOR_VERSION,SDL_MIXER_MINOR_VERSION,SDL_MIXER_PATCHLEVEL);
		    exit(0);
	        }
	     ],
	     [sdlmcurver=`./conftest$EXEEXT` COMPILEVER="yes"],
	     [sdlmcurver="0" COMPILEVER="no"]
	    )
	LIBS=$_libs
	NEEDVER="1.2.5"
	echo -n "checking for libSDL_mixer >= $NEEDVER..."
	if test "$COMPILEVER" = "yes"; then
	    checkver $NEEDVER $sdlmcurver 100
	    if test "$?" = "0"; then
		HAVESDLMIXERLIB="yes"
		sdlmstrver=$strver
	    else
		HAVESDLMIXERLIB="no"
    	    fi
	else
	    HAVESDLMIXERLIB="no"
	    echo "no. no any version installed"
	fi
    fi
else
    HAVELOCALLIBS="yes"
    HAVEZLIB="yes"
    HAVERT="yes"
    HAVEGRPLIB="yes"
    HAVEMPQLIB="yes"
    HAVESDLLIB="yes"
    HAVESDLMIXERLIB="yes"
    echo "Link with ./libs"
fi

    AM_CONDITIONAL(HAVEDEBUG,test "$debug" = "yes" )
    AM_CONDITIONAL(HAVECPP,test "$HAVECPP" = "yes")
    AM_CONDITIONAL(HAVELOCALLIBS,test "$locallibs" = "yes")
    AM_CONDITIONAL(HAVEGRPLIB,test "$HAVEGRPLIB" = "yes")
    AM_CONDITIONAL(HAVEMPQLIB,test "$HAVEMPQLIB" = "yes")
    AM_CONDITIONAL(HAVEZLIB,test "$HAVEZLIB" = "yes")
    AM_CONDITIONAL(HAVERT,test "$HAVERT" = "yes")
    AM_CONDITIONAL(HAVESDLLIB,test "$HAVESDLLIB" = "yes")
    AM_CONDITIONAL(HAVESDLMIXERLIB,test "$HAVESDLMIXERLIB" = "yes")
    AM_CONDITIONAL(FORCE32BIT,test "$with32" = "yes")
    AM_CONDITIONAL(SYS64,test "$sys64" = "yes")
    AM_CONDITIONAL(HAVEWARNING,test "$warning" = "yes")
    AM_CONDITIONAL(HAVESTORMLIB,test "$HAVESTORMLIB" = "yes")

AC_OUTPUT(Makefile)

    echo "***************************************"
    echo "Current Configuration:"
    if test "$sys64" = "yes"; then
	if test "$with32" = "yes"; then
	    echo "compile 32-bit binary with 32-bit library support (on 64-bit system)"
	else
	    echo "compile 64-bit binary with 64-bit library support"
	fi
    else
	echo "compile 32-bit binary with 32-bit library support"
    fi
    if test "$debug" = "yes"; then
	echo "binary with Debugging Enabled"
    else
	echo "binary with Debugging Disabled"
    fi
    if test "$warning" = "yes"; then
	echo "compile with all warnings enabled"
    else
	echo "compile with out warnings"
    fi
    if test $locallibs = "no" ; then
	if test "$HAVEZLIB" = "yes"; then
	    echo "link with libz $zstrver version"
	fi
	if test "$HAVERT" = "yes"; then
	    echo "link with rt"
	fi
	if test "$HAVEGRPLIB" = "yes"; then
	    echo "link with libgrp $grpstrver version"
	fi
	if test "$HAVEMPQLIB" = "yes"; then
	    echo "link with libmpq $mpqstrver version"
	fi
	if test "$HAVESTORMLIB" = "yes"; then
	    echo "link with StormLib $stormstrver version"
        fi
	if test "$HAVESDLLIB" = "yes"; then
	    echo "link with libSDL $sdlstrver version"
	fi
	if test "$HAVESDLMIXERLIB" = "yes"; then
	    echo "link with libSDL_mixer $sdlmstrver version"
	else
	    echo "link with no libSDL_mixer"
	fi
    else
	echo "link with compiled libraries"
    fi
    rm -f bin/${PACKAGE}.conf
    echo "***************************************"
    echo "CONFIGURATION COMPLETE."
    echo "Type 'make' to compile $PACKAGE"
    echo "If $PACKAGE does not copy StarDat.mpq and BrooDat.mpq on first launch,"
    echo "You can download them manually, place them onto ./bin directory."
    echo "Copy Maps to ./bin/maps directory. Create if needed."
    echo "After 'make' You can launch ./$PACKAGE from ./bin directory"

