####modify project information
PRJ_NAME = gluetiosclient
TARGET = gluetiosclient

CPPFLAGS += -I. -Iinclude
CXXFLAGS += $(CFLAGS)
LDFLAGS += -L$(INSTALL_ROOT)/lib -L. 
#LDFLAGS += -Wl,-rpath-link=$(INSTALL_ROOT)/lib
#LIBS +=-lEOS_WaveAudio_NoUI 
LIBS +=  -lprotobuf -lm -lstdc++ 

SRCFILES= 	main.cpp GlueProtocol.pb.cpp

include $(MKFILE_TMPLATE_DIR)/MAKE_BASE_EXECUTABLE.mk

