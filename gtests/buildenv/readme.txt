# 如何编译项目？
请首先运行 source buildenv/envsetup_ndk.sh 
然后就可以到每个项目中直接使用 make -f ....
当然，如果编译x86版本,请调用 source buildenv/envsetup_x86.sh

# 如何判断编译环境?
举个例子
ifeq ($(TARGET_PLAT),ndk)
else
LIBS+= -lpthread
endif

LIBS+= -dl -lstdc++

# 如何快速批量加入目录的文件
SRCFILES = 	$(shell ls ole32/*.c)
SRCFILES += 	$(shell ls rpcrt4/*.c)
SRCFILES += 	$(shell ls rpcrt4/*.cpp)

# 如何为所有的项目增加宏定义和编译标识
请修改MAKE_BASE_ENV.mk

# 哪里有例子？
可执行文件 Makefile.execute
共享库 Makefile.share
静态库 Makefile.static