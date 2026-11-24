# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod
#DEFS += -DCOMMONOAM
#DEFS += -DCOMMONOAM -rdynamic
######################################################################
# define the include flags.
# define the list of source files.
SRCS = HmpIpmDevice.cc\
       HmpDxDevice.cc\
       HmpDtiDevice.cc\
       HmpDevice.cc\
       HmpHandler.cc\

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/rtsp/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/hmputil/include\
            -I $(PROJ_HOME)/media/dialogic/hmp/src/db/include\
            -I $(PROJ_HOME)/media/mediautil/include\
            -I $(OAM_HOME)/include\
            -I $(OAM_HOME)/log/include\
            -I $(OAM_HOME)/cdr/include\
            -I $(OAM_HOME)/oamutil/include\
            -I $(OAM_HOME)/oamprocess/handler/includ\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/msgq/include\
            -I $(UTILDIR)/shmq/include\
            -I $(UTILDIR)/semaphore/include\
            -I $(UTILDIR)/strtok/include\
            -I $(UTILDIR)/timer/include\
            -I $(UTILDIR)/configinfo/include\
            -I $(UTILDIR)/diskmonitor/include\
            -I $(UTILDIR)/processlock/include\
            -I $(UTILDIR)/diskbuffer/include\
            -I $(UTILDIR)/datastructures/include\
            -I $(UTILDIR)/trace/include\
            -I $(TPDIR)/SQLAPI/include\
            -I $(TPDIR)/media/dialogic-hmp/inc\
            -I $(PROJ_HOME)/media/interface/include\
            -I $(LIVE555)/UsageEnvironment/include\
            -I $(LIVE555)/groupsock/include\
            -I $(LIVE555)/liveMedia/include\
            -I $(LIVE555)/BasicUsageEnvironment/include\
            -L $(TPDIR)/media/dialogic-hmp/lib

######################################################################
LIBFLAGS =  -lipm\
            -lrtf\
            -lxerces\
            -ldmr4cleo\
            -lsrl\
            -ldevmap\
            -loswin32\
            -ldevnamedoler\
            -ldlgsysloggerclient\
            -lOAMTransport\
            -ldlgoslayer\
            -lOAMOSSL\
            -lOAMNaming\
            -ldevmgmt\
            -ldxxx\
            -lgc\
            -ldti\
            $(PROJ_HOME)/media/dialogic/hmp/src/hmputil/lib/libHmpUtil.a\
            $(PROJ_HOME)/media/dialogic/hmp/src/db/lib/libHmpDb.a\
            $(OAM_HOME)/lib/libOamUtil.a\
            $(OAM_HOME)/lib/libLogApi.a\
            $(UTILDIR)/lib/libutil.a\
            $(TPDIR)/SQLAPI/lib/libsqlapi.a\
            /usr/lib/libssl.so\

LIBDIR = lib

######################################################################
# list of generated object files.
OBJS1 =  HmpIpmDevice.o\
         HmpDxDevice.o\
         HmpDtiDevice.o\
         HmpDevice.o


OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = $(PROJ_HOME)/media/dialogic/hmp/src/rtsp/lib/libHmpRtsp.a

# output file name. 
OFILE1 = libHmpDevice.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# list of generated object files.
OBJS2 = HmpHandler.o\
        HmpHandlerMain.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

LIBS2 = -lboost_thread\
        $(PROJ_HOME)/media/dialogic/hmp/src/kernel/lib/libHmpDevice.a\
        $(PROJ_HOME)/media/dialogic/hmp/src/rtsp/lib/libHmpRtsp.a

# output file name. 
OFILE2 = DLG_HostMediaProcess

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

######################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2)

########################################################################
# List of all libraries to be created
OLIB = $(OFILE1) 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE2) 
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/media/makeutil/footer.mod
# DO NOT DELETE
