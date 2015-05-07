####modify project information
PRJ_NAME =vtable
TARGET =vtable

CPPFLAGS += -I.
CXXFLAGS += -fno-rtti -g -Wall -O2 -W -fPIC  -march=armv5te
CXXFLAGS += $(CFLAGS)
#LDFLAGS += -L$(INSTALL_ROOT)/lib 
#LDFLAGS += -Wl,-rpath-link=$(INSTALL_ROOT)/lib
#LIBS +=-lEOS_WaveAudio_NoUI 
#LIBS += -lpulse

SRCFILES= 	myc.c mycpp.cpp stdafx.cpp vtable.cpp
			
include $(MKFILE_TMPLATE_DIR)/MAKE_BASE_EXECUTABLE.mk

