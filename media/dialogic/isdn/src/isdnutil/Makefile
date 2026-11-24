# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod

DEFS += -DCOMMONOAM

######################################################################
# define the include flags.
# define the list of source files.
SRCS = IsdnUtil.cc\
       IsdnConfigApi.cc\
       IsdnQueApi.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/media/dialogic/isdn/src/include\
            -I $(PROJ_HOME)/media/dialogic/isdn/src/isdnutil/include\
            -I $(PROJ_HOME)/media/mediautil/include\
            -I $(GGI_HOME)/include\
            -I $(GGI_HOME)/ggiutil/include\
            -I $(SS7_HOME)/include\
            -I $(SS7_HOME)/ss7util/include\
            -I $(OAM_HOME)/include\
            -I $(OAM_HOME)/log/include\
            -I $(OAM_HOME)/cdr/include\
            -I $(OAM_HOME)/oamutil/include\
            -I $(LISTENERDIR)/include\
            -I $(GATEWAYDIR)/base/include\
            -I $(UTILDIR)/diskmonitor/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/configinfo/include\
            -I $(UTILDIR)/trace/include\
            -I $(UTILDIR)/msgq/include\
            -I $(UTILDIR)/shmq/include\
            -I $(UTILDIR)/socket/include\
            -I $(UTILDIR)/octet/include\
            -I $(UTILDIR)/semaphore/include\
            -I $(UTILDIR)/strtok/include\
            -I $(UTILDIR)/timer/include\
            -I $(UTILDIR)/datastructures/include\
            -I $(UTILDIR)/processlock/include\
            -I $(UTILDIR)/protocolstack/include\
            -I $(TPDIR)/SQLAPI/include\
            -I $(TPDIR)/TDAPI1800/include\
            -I $(TPDIR)/media/dialogic-isdn/inc\
            -I $(TPDIR)/media/dialogic-isdn/sctools\
            -I $(PROJ_HOME)/media/interface/include\

######################################################################
# define the library flags.

LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = IsdnUtil.o IsdnConfigApi.o  IsdnQueApi.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%) 

# list of libraries.
LIBS1 =

# output file name.
OFILE1 = libIsdnUtil.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

######################################################################
# List of all object files
OBJS = $(OBJS1) 

########################################################################
# List of all libraries to be created
OLIB = $(OFILE1) 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG =
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/media/makeutil/footer.mod
# DO NOT DELETE
