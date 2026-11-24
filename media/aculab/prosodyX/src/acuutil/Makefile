# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod

DEFS += -DCOMMONOAM

######################################################################
# define the include flags.
# define the list of source files.
SRCS = AcuMediaUtil.cc\
       AcuMsgQueApi.cc\
       AcuCardDetails.cc\
       AcuModuleInfo.cc\
       AcuEventApi.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/media/mediautil/include\
            -I $(PROJ_HOME)/media/interface/include\
            -I $(PROJ_HOME)/media/aculab/prosodyX/src/include\
            -I $(PROJ_HOME)/media/aculab/prosodyX/src/kernel/include\
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
            -I $(TPDIR)/media/aculab/include\
            -I $(TPDIR)/TDAPI/include\
            -I $(TPDIR)/ACULAB/include\
            -I $(TPDIR)/ACULAB/TiNG/include\
            -I $(TPDIR)/ACULAB/ProsodyLibraries/Group3Fax/API/include\
            -I $(LIVE555)/UsageEnvironment/include\
            -I $(LIVE555)/groupsock/include\
            -I $(LIVE555)/liveMedia/include\
            -I $(LIVE555)/BasicUsageEnvironment/include\

######################################################################
# define the library flags.
LIBFLAGS = $(ROOTDIR)/media/mediautil/lib/libMediaUtil.a\
           $(UTILDIR)/lib/libutil.a\
           $(OAM_HOME)/lib/libOamUtil.a\
           $(OAM_HOME)/lib/libLogApi.a\
           $(TPDIR)/ACULAB/lib/libacu_res.so\
           $(TPDIR)/ACULAB/lib/libTiNGshared.so\
           $(TPDIR)/ACULAB/lib/libacu_sw.so\
           $(TPDIR)/ACULAB/lib/libacu_cl.so\
           -lshpa3\
           /usr/lib/libssl.so

LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = AcuMediaUtil.o AcuMsgQueApi.o AcuCardDetails.o AcuModuleInfo.o AcuEventApi.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%) 

# list of libraries.
LIBS1 =

# output file name.
OFILE1 = libAcuMediaUtil.a

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

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
