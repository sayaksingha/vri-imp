# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod
CFLAGS += -D DB_CLIENT_HMP=SA_MySQL_Client
#DEFS += -DCOMMONOAM

######################################################################
# define the include flags.
# define the list of source files.
SRCS = HmpDbMgr.cc\

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/hmputil/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/rtsp/include\
            -I $(PROJ_HOME)/media/mediautil/include\
            -I $(OAM_HOME)/include\
            -I $(OAM_HOME)/log/include\
            -I $(OAM_HOME)/cdr/include\
            -I $(OAM_HOME)/oamutil/include\
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
            -I $(LIVE555)/UsageEnvironment/include\
            -I $(LIVE555)/groupsock/include\
            -I $(LIVE555)/liveMedia/include\
            -I $(LIVE555)/BasicUsageEnvironment/include\
            -I $(TPDIR)/media/dialogic-hmp/inc\
            -I $(PROJ_HOME)/media/interface/include\


######################################################################
# define the library flags.
LIBFLAGS = $(PROJ_HOME)/media/dialogic/hmp/src/hmputil/lib/libHmpUtil.a\

LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = HmpDbMgr.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%) 

# list of libraries.
LIBS1 =

# output file name.
OFILE1 = libHmpDb.a

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
