#MAKE_ROOT_DIR=`pwd`

ifeq ($(CC),gcc)
	TARGET_PLAT = x86
	INCLUDE_ROOT = $(MAKE_ROOT_DIR)/Include/
	OBJECTS_ROOT = $(MAKE_ROOT_DIR)/build/$(TARGET_PLAT)/objs/
	DESTDIR_ROOT = $(MAKE_ROOT_DIR)/build/$(TARGET_PLAT)/
	PREBUILD_LIB = $(MAKE_ROOT_DIR)/build/prebuild/$(TARGET_PLAT)/
	
	CPPFLAGS = -D__linux__=1 -I$(INCLUDE_ROOT)
#CPPFLAGS +=-I$INSTALL_ROOT/include
	CFLAGS = -g -Wall -O2 -W -fPIC
	CFLAGS += -fvisibility=hidden

	CXXFLAGS = -fno-rtti
	CXXFLAGS += -fno-exceptions

	LDFLAGS= -L$(DESTDIR_ROOT) -L$(PREBUILD_LIB)
	
	LIBS=
endif

ifeq ($(CC),arm-linux-androideabi-gcc)
	TARGET_PLAT = ndk
	INCLUDE_ROOT = $(MAKE_ROOT_DIR)/Include
	OBJECTS_ROOT = $(MAKE_ROOT_DIR)/build/$(TARGET_PLAT)/objs/
	DESTDIR_ROOT = $(MAKE_ROOT_DIR)/build/$(TARGET_PLAT)/
	PREBUILD_LIB = $(MAKE_ROOT_DIR)/build/prebuild/$(TARGET_PLAT)/
	
	CPPFLAGS = -D__linux__=1 -D__android__ -DANDROID -I$(INCLUDE_ROOT)
#CPPFLAGS +=-I$INSTALL_ROOT/include	
	CFLAGS = -g -Wall -O2 -W -fPIC 
#	CFLAGS += -march=armv5te
	CFLAGS += -fvisibility=hidden
	
	CXXFLAGS = -fno-rtti
	CXXFLAGS += -fno-exceptions
	
	LDFLAGS= -L$(DESTDIR_ROOT) -L$(PREBUILD_LIB)
	
	LIBS=

endif
