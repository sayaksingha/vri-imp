# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod
#DEFS += -DCOMMONOAM
#DEFS += -DCOMMONOAM -rdynamic
######################################################################
# define the include flags.
# define the list of source files.
SRCS = StubMPInput_single.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/media/dialogic/isdn/src/include\
            -I $(PROJ_HOME)/media/dialogic/isdn/src/isdnutil/include\
            -I $(PROJ_HOME)/media/mediautil/include\
            -I $(OAM_HOME)/include\
            -I $(OAM_HOME)/log/include\
            -I $(OAM_HOME)/cdr/include\
            -I $(OAM_HOME)/oamutil/include\
            -I $(OAM_HOME)/oamprocess/handler/includ\
            -I $(SS7_HOME)/include\
            -I $(SS7_HOME)/ss7util/include\
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
            -I $(TPDIR)/media/dialogic-isdn/inc\
            -I $(TPDIR)/media/dialogic-isdn/sctools\
            -I $(PROJ_HOME)/media/interface/include\
            -L $(TPDIR)/media/dialogic-isdn/lib

######################################################################
#IBFLAGS =  -lsrl\
            -lrtf\
            -lxerces\
            -ldevmap\
            -ldevnamedoler\
            -ldlgsysloggerclient\
            -lOAMTransport\
            -ldlgoslayer\
            -lOAMOSSL\
            -lOAMNaming\
            -ldevmgmt\
            -ldxxx\
            -lgc\
            -ldti
LIBFLAGS =  $(PROJ_HOME)/media/dialogic/isdn/src/isdnutil/lib/libIsdnUtil.a\
            $(OAM_HOME)/lib/libOamUtil.a\
            $(OAM_HOME)/lib/libLogApi.a\
            $(UTILDIR)/lib/libutil.a\
            $(TPDIR)/SQLAPI/lib/libsqlapi.a\
            /usr/lib/libssl.so\

LIBDIR = lib

######################################################################
# list of generated object files.
OBJS1 =  


OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 

# output file name. 
OFILE1 = 

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# list of generated object files.
OBJS2 = StubMPInput_single.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

LIBS2 =  #-lboost_thread #\
         $(TPDIR)/media/dialogic-isdn/sctools/sctools.o\
         $(PROJ_HOME)/media/dialogic/isdn/src/kernel/lib/libIsdnDevice.a

# output file name. 
OFILE2 = DLG_STUB_MPInput_single

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

######################################################################
# List of all object files
OBJS = $(OBJS2)

########################################################################
# List of all libraries to be created
OLIB = 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = #$(OFILE2) 
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################
# List of all stubs to be created
STUBS = $(OFILE2) 
STUBS := $(STUBS:%=$(BINDIR)/%)
########################################################################

include $(ROOTDIR)/media/makeutil/footer.mod
# DO NOT DELETE
