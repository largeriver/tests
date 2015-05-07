####modify project information
PRJ_NAME =sensors
TARGET =sensors

CPPFLAGS += -I. -Iinclude
CXXFLAGS += $(CFLAGS)
LDFLAGS += -L$(INSTALL_ROOT)/lib 
#LDFLAGS += -Wl,-rpath-link=$(INSTALL_ROOT)/lib
#LIBS +=-lEOS_WaveAudio_NoUI 
#LIBS += -lpulse

SRCFILES= 	main.c hardware.c
			
include $(MKFILE_TMPLATE_DIR)/MAKE_BASE_EXECUTABLE.mk

