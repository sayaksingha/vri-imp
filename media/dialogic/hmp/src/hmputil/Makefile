# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod

DEFS += -DCOMMONOAM

######################################################################
# define the include flags.
# define the list of source files.
SRCS = HmpUtil.cc\
       HmpQueApi.cc\
       HmpEvrApi.cc\
       HmpReqMapApi.cc\
       HmpConfigApi.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/rtsp/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/hmputil/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/db/include\
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
            -I $(TPDIR)/media/dialogic-hmp/inc\
            -I $(PROJ_HOME)/media/interface/include\
            -I $(LIVE555)/UsageEnvironment/include\
            -I $(LIVE555)/groupsock/include\
            -I $(LIVE555)/liveMedia/include\
            -I $(LIVE555)/BasicUsageEnvironment/include\

######################################################################
# define the library flags.

LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = HmpUtil.o HmpQueApi.o HmpEvrApi.o HmpReqMapApi.o HmpConfigApi.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%) 

# list of libraries.
LIBS1 =

# output file name.
OFILE1 = libHmpUtil.a

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
