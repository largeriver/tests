
LD_SHARED = -shared -Wl,-soname,$(TARGET)

OBJECTS_DIR=$(OBJECTS_ROOT)$(PRJ_NAME)_dir/
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
	$(CC) $(LD_SHARED) -o $(DESTDIR)$(TARGET) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS)

install: 
	cp -f $(DESTDIR)$(TARGET) $(INSTALL_ROOT)/lib

clean:
	rm -f $(DESTDIR)$(TARGET) $(COBJS) $(CPPOBJS)

.SUFFIXES: .cpp .c .o

$(OBJECTS_DIR)%.o:%.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c -o $@  $<

$(OBJECTS_DIR)%.o:%.c
	$(CC) $(CFLAGS)   $(CPPFLAGS) -c -o $@  $<