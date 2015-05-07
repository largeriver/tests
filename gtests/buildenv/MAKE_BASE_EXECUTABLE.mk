OBJECTS_DIR=$(OBJECTS_ROOT)$(PRJ_NAME)_dir/

CXXFLAGS	+= $(CFLAGS)
DESTDIR=$(DESTDIR_ROOT)
DEL_FILE = rm -f
MKDIR = mkdir -p

CFILES 	= 	$(filter %.c,$(SRCFILES))
CPPFILES =	$(filter %.cpp,$(SRCFILES))

first: all

all: $(OBJECTS_DIR) $(DESTDIR) $(DESTDIR)$(TARGET)

$(OBJECTS_DIR) :
	$(MKDIR) $(OBJECTS_DIR)

$(DESTDIR) :
	$(MKDIR) $(DESTDIR)

COBJS	=$(patsubst %.c,$(OBJECTS_DIR)%.o,$(CFILES))
CPPOBJS	=$(patsubst %.cpp,$(OBJECTS_DIR)%.o,$(CPPFILES))

$(DESTDIR)$(TARGET): $(COBJS) $(CPPOBJS)
	$(CC) -o $(DESTDIR)$(TARGET) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS)

install: 
	cp -f $(DESTDIR)$(TARGET) $(INSTALL_ROOT)/bin

clean:
	rm -f $(DESTDIR)$(TARGET) $(COBJS) $(CPPOBJS)
	rm -fr $(OBJECTS_DIR) 

.SUFFIXES: .cpp .c .o

$(OBJECTS_DIR)%.o:%.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c -o $@  $<

$(OBJECTS_DIR)%.o:%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS)   $(CPPFLAGS) -c -o $@  $<
	
# Makefile
# include $(MKFILE_TMPLATE_DIR)/MAKE_BASE_ENV_LIBRARY.mk 
# PRJ_NAME =tangram
# TARGET =libtangramcom.so	
# SRCFILES =$(shell ls *.cpp)
# include $(MKFILE_TMPLATE_DIR)/MAKE_BASE_EXECUTABLE.mk	
	