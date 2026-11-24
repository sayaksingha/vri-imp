# @(#) $Id: header.mod

############ Start of system configuration section. ############

SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .cc .o .a .tcc .h

# use "g++" to compile c++/c source files.
#CC = g++ -O1
CC = g++

# use "javac" to compile java source files.
JAVAC = javac

# the linker is also "g++". It might be something else with other compilers.
LD = g++ -ldl -lpthread -lrt -rdynamic -lcrypto -lssl -lcurl -lpq -lfcgi -lboost_regex

# use "ar" to create libraries
AR = ar

# use "astyle" to format source code
FORMAT = astyle

# Things you might add to DEFS: (These are defines that are used during compilation)
# -DOS=LINUX                  // If the OS is Linux
# -DOS=HPUX                   // If the OS is HP Unix
# -DOS=SOLARIS                // If the OS is Solaris
# -D_XOPEN_SOURCE
# -D_POSIX_C_SOURCE=199506L   // add IEEE Std 1003.1c-1995

#DEFS = -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=199506L
DEFS = -D_XOPEN_SOURCE=600 -DLINUX -DCOMMONOAM

# Trace options
ifeq (1,$(CTRACE))
DEFS += -DCTRACE
endif

# Debug options
ifeq (1,$(CDEBUG))
CDEBUG = -g
else
CDEBUG =
endif

# Options for code coverage utility (gcov)
# CCOV = -ftest-coverage -fprofile-arcs

# Warnings
#WFLAGS = #-Wall -W #-Wshadow -Wconversion
WFLAGS = -Wswitch -Wall -Wno-deprecated #-Wno-unused-variable

# Compiler flags go here.
#CFLAGS = $(CDEBUG) $(DEFS) -ansi $(WFLAGS) $(CCOV)
#CFLAGS = $(DEFS) -ansi $(WFLAGS) $(CCOV) -march=i686
#CFLAGS = $(DEFS) -ansi $(WFLAGS) $(CCOV) -march=x86_64
CFLAGS = $(DEFS) -ansi $(WFLAGS) $(CCOV)

# Linker flags go here. Currently there aren't any, but if we'll switch to
# code optimization, we might add "-s" here to strip debug info and symbols.
# LDFLAGS = -static

# Compiler flags go here.
ARFLAGS = -rsuv
SONAME=
SHFLAGS = -shared -Wl,-soname,$(SONAME) -o

# use this command to erase files.
RM = /bin/rm -f

# command to create dependency file
MAKEDEP = $(CC) $(CFLAGS) -MM
#MAKEDEP = makedepend -f- -pobj/ -- $(CFLAGS) -- $(SRCS)

############ End of system configuration section. ############

VPATH = src
OBJDIR = obj
SRCDIR = src
BINDIR = obj
EGDIR= examples

#################################################################

# top-level rule, to compile everything.
all:
