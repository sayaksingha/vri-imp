# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

DEFS += -DCOMMONOAM

######################################################################
# define the include flags.
# define the list of source files.
SRCS = TimerStub.cc\

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/include\
            -I $(PROJ_HOME)/siputil/include\
            -I $(PROJ_HOME)/db/include\
            -I $(CGW_HOME)/include\
            -I $(CGW_HOME)/cgwapi/include\
            -I $(CGW_HOME)/cgwutil/include\
            -I $(SS7_HOME)/include\
            -I $(SS7_HOME)/Telesoft/include\
            -I $(SS7_HOME)/ss7util/include\
            -I $(SS7_HOME)/db/include\
            -I $(SS7_HOME)/protocol/cap/include\
            -I $(GGI_HOME)/include\
            -I $(GGI_HOME)/ggiutil/include\
            -I $(GGI_HOME)/db/include\
            -I $(OAM_HOME)/include\
            -I $(OAM_HOME)/log/include\
            -I $(OAM_HOME)/cdr/include\
            -I $(OAM_HOME)/oamutil/include\
            -I $(OAM_HOME)/oamprocess/handler/includ\
            -I $(LISTENERDIR)/include\
            -I $(LISTENERDIR)/kernel/include\
            -I $(LISTENERDIR)/util/asciitogsm/include\
            -I $(LISTENERDIR)/util/addrTranslation/include\
            -I $(GATEWAY_HOME)/base/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/msgq/include\
            -I $(UTILDIR)/shmq/include\
            -I $(UTILDIR)/semaphore/include\
            -I $(UTILDIR)/datastructures/include\
            -I $(UTILDIR)/strtok/include\
            -I $(UTILDIR)/socket/include\
            -I $(UTILDIR)/trace/include\
            -I $(UTILDIR)/timer/include\
            -I $(UTILDIR)/octet/include\
            -I $(UTILDIR)/filedeletion/include\
            -I $(UTILDIR)/configinfo/include\
            -I $(UTILDIR)/protocolstack/include\
            -I $(UTILDIR)/diskmonitor/include\
            -I $(UTILDIR)/processlock/include\
            -I $(UTILDIR)/diskbuffer/include\
            -I $(TPDIR)/SQLAPI/include\
            -I $(TPDIR)/TDAPI1800/include\

######################################################################
LIBFLAGS = $(UTILDIR)/lib/libutil.a\
#(PROJ_HOME)/db/lib/libSipDb.a\
            $(SS7_HOME)/db/lib/libSs7Db.a\
            $(SS7_HOME)/ss7util/lib/libSs7Util.a\
            $(GGI_HOME)/ggiutil/lib/libGgiUtil.a\
            $(GGI_HOME)/ggiutil/lib/libGgiApi.a\
            $(GGI_HOME)/db/lib/libGgiDb.a\
            $(OAM_HOME)/lib/libOamUtil.a\
            $(OAM_HOME)/lib/libLogApi.a\
            $(UTILDIR)/lib/libutil.a\
            $(TPDIR)/SQLAPI/lib/libsqlapi.a\
            /usr/lib/libdl.a\
            /usr/lib/libssl.so\
            /usr/lib/libpthread.so 

LIBDIR = lib

######################################################################
# list of generated object files.
OBJS1 = TimerStub.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 =  

# output file name. 
OFILE1 = SIP_TimerStub

# Program dependencies
$(OFILE1): $(BINDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# list of generated object files.
OBJS2 = Sip1xProcMain.o\
        Sip1xProc.o\

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 =  

# output file name. 
OFILE2 = SIP_1xProc

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

######################################################################
#    ********** For Library creation ***********
# list of generated object files.
#OBJS2 = Siplib.o 

#OBJS2 := $(OBJS2:%=$(OBJDIR)/%) 

# list of libraries.
#LIBS2 =

# output file name.
#OFILE2 = libSip.a

# Program dependencies
#$(OFILE2): $(LIBDIR)/$(OFILE2)
#$(LIBDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

######################################################################
# List of all object files
OBJS = $(OBJS1)

########################################################################
# List of all libraries to be created
OLIB = 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE1) 
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
