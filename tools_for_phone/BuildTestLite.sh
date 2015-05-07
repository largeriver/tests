#!/bin/bash
declare -a PRJS_PATH
PRJS_PATH=("tests/paplayi" "tests/eos_wavelib" "tests/eos_wavelib_noui" "tests/eos_player"  "tests/eos_record" "tests/helloworld")
#PRJS_PATH=("tests/paplayi" "tests/eos_wavelib" "tests/eos_player" "tests/eos_wavelib_fake" "tests/helloworld")
PRJS_COUNT="${#PRJS_PATH[@]}"
pos_start=0
let "pos_end=$PRJS_COUNT-1"

ACT_CONFIGURE=0
ACT_BUILD=0
ACT_INSTALL=0
ACT_CLEAN=0
ACT_UNINS=0

TARGET_PLAT=x86




function show_help(){
	echo "options: "
	echo "\t[all | buildall] configure,make,install"
	echo "\t[cleanall] uninstall,clean"
	echo "\t[unins] [uninstall] uninstall"
	echo "\t[build|make]  make"
	echo "\t[ins] [install] install"
	echo "\t[clean] clean"
	echo "\t[-b pos] begin pos 0-7"
	echo "\t[-e] end pos 0-7"
	echo "\t[-be pos] pos 0-7"
	echo "\t[x86] buildfor x86"
	echo "\t[arm] buildfor arm"
	echo "\t[ndk] buildfor ndk"
	echo "\t[ndkr6] buildfor ndkr6"
	echo "\t[eabi] buildfor ANDROID NDK USEING NDK"
	echo "\t[help] print this help"
	echo "\t[list] list all projects" 
	echo "\t[-P prj_path] build the project by prj_path" 
	exit 0
}

function list_prjs_name(){
	local index=0
	while [ "$index" -lt "$PRJS_COUNT" ]
	do
		echo $index  : ${PRJS_PATH[$index]}
		((index++))
	done
}

while [ -n "$1" ]
do
	case $1 in
		help)
			show_help
			exit 0
			;;
		list)
			list_prjs_name
			exit 0
			;;
		x86)
			TARGET_PLAT="x86"
			;;	
		arm)
			TARGET_PLAT="arm"
			;;	
		eabi)
			TARGET_PLAT="eabi"
			;;	
		ndk)
			TARGET_PLAT="ndk"
			;;
		ndkr6)
			TARGET_PLAT="ndkr6"
			;;				
		-be)
			shift
			pos_start=$1
			pos_end=$1
			echo "position $1"
			;;				
		"-b")
			shift
			pos_start=$1
			echo "position start $1"
			;;
		"-e")
			shift
			pos_end=$1
			echo "position stop $1"
			;;
		all | buildall)
			echo "actions: config,build,install"
			ACT_CONFIGURE=1
			ACT_BUILD=1
			ACT_INSTALL=1
			;;
		uni*)
			echo "actions: uninstall"
			ACT_UNINS=1
			;;
		clean)
			echo "actions: clean"
			ACT_CLEAN=1
			;;
		cleanall)
			echo "actions: uninstall clean"
			ACT_UNINS=1
			ACT_CLEAN=1
			;;
		config*)
			echo "actions: config"
			ACT_CONFIGURE=1
			;;
		build|"make")
			echo "actions: build"
			ACT_BUILD=1
			;;
		ins*)
			echo "actions: install"
			ACT_INSTALL=1
			;;
		-P)
			shift
			PRJS_PATH=($1)
			PRJS_COUNT="${#PRJS_PATH[@]}"
			pos_start=0
			pos_end=0
			echo "compile the project by path $1"
			;;
		*) echo "$1 is not a valid option"
			exit 1
			;;
	esac
shift
done


CURRENT_DIR=`pwd`
OBJECTS_ROOT=`pwd`/build/$TARGET_PLAT/objs/
DESTDIR_ROOT=`pwd`/build/$TARGET_PLAT/
MKFILE_TMPLATE_DIR=`pwd`



echo "-----------------------"

case $TARGET_PLAT in
#	Linux.i586 | 	Linux.i686)
	x86)
		INSTALL_ROOT=${HOME}/Programs
		makefile_name="Makefile"
		
		CC=gcc
		
		CPPFLAGS=" -I$INSTALL_ROOT/include "
		CFLAGS="-g -Wall -O2 -W -fPIC -fvisibility=hidden"
		LDFLAGS="-L$INSTALL_ROOT/lib " 
		LIBS=
		;;

	arm)
		if  echo $(arm-linux-gcc -v 2>&1)|grep "command not found"  ;then
			echo "you must choose cross compiler first"
			exit 1
		fi
		
		INSTALL_ROOT=${HOME}/Work/Build/tests/arm
		makefile_name="Makefile"
		
		CC=arm-linux-gcc
		
		CPPFLAGS=" -I$INSTALL_ROOT/include "
		CFLAGS="-march=armv6 -g -Wall -O2 -W -fPIC -fvisibility=hidden"
		LDFLAGS="-L$INSTALL_ROOT/lib " 
		LIBS=
		;;
	ndk)
		if  echo $(arm-eabi-gcc -v 2>&1)|grep "command not found"  ;then
			echo "you must choose cross compiler first"
			exit 1
		fi
	
		INSTALL_ROOT=${HOME}/Work/Build/tests/ndk
		makefile_name="Makefile"
		
		CC=arm-linux-gcc
		
		CPPFLAGS=" -I$INSTALL_ROOT/include "
		CFLAGS="-march=armv5te -g -Wall -O2 -W -fPIC -fvisibility=hidden"
		LDFLAGS="-L$INSTALL_ROOT/lib " 
		LIBS=
		;;
    ndkr6)
		CC=arm-linux-androideabi-gcc
        if  echo $( ${CC} -v 2>&1)|grep "command not found"  ;then
                echo "you must choose cross compiler first"
                exit 1
        fi  

        INSTALL_ROOT=${HOME}/Work/Build/tests/ndkr6
        makefile_name="Makefile"
      

        CPPFLAGS=" -I$INSTALL_ROOT/include "
        CFLAGS=" -g -Wall -O2 -W -fPIC -fvisibility=hidden"
        LDFLAGS="-L$INSTALL_ROOT/lib " 
        LIBS=
        ;;  
                 
	*)
		echo you must choose a TARGET_PLAT:x86,arm,ndk
		exit 1
		;;
esac



echo Building for $TARGET_PLAT with $CC
export MKFILE_TMPLATE_DIR
export OBJECTS_ROOT
export DESTDIR_ROOT
export INSTALL_ROOT

export CC
	
export CPPFLAGS
export CFLAGS
export LDFLAGS
export LIBS




function exec_build ()
{
	local index=$1

	local prj_path="${PRJS_PATH[index]}"
	
	echo ">>>> Building $index  in $prj_path for $TARGET_PLAT with $CPLUS"
		
	cd $prj_path
	
	if [ ! -f $makefile_name ]; then
		echo "<<<< Error the \"$makefile_name does not exist"
		return 1
	fi
	
	if [[ $ACT_UNINS > 0 ]]; then
	echo "**** ACT_UNINS"
		make -f $makefile_name uninstall
	fi
	

	if [[ $ACT_CLEAN > 0 ]]; then
	echo "**** ACT_CLEAN"
		make -f $makefile_name clean
	fi
	

	if [[ $ACT_CONFIGURE > 0 ]]; then
	echo "**** ACT_CONFIGURE"
	fi
	
	if [[ $ACT_BUILD > 0 ]]; then
	echo "**** ACT_BUILD"
		make -f $makefile_name -j2
		if [ $? -ne 0 ]; then 
			echo "wrong and stop"
		fi
	fi

	if [[ $ACT_INSTALL > 0 ]]; then
	echo "**** ACT_INSTALL"	
		make -f $makefile_name install
		if [ $? -ne 0 ]; then 
			echo "wrong and stop"
		fi
	fi
	
	cd $CURRENT_DIR
	echo "<<<< Finish  Building $index  in $prj_path for $TARGET_PLAT with $CPLUS"
}


index=$pos_start
echo "pos $pos_start $pos_end  "
while [ $index -le $pos_end ]
do
	exec_build $index
	if [ $? -ne 0 ]; then
		echo "faild to build ${PRJS_PATH[index]} ,stoped $?"
		exit 1
	fi
	let "++index"
done


