# @(#) $Id: Makefile,v 1.1.1.1.2.5.2.4 2025/01/06 11:08:31 mram Exp $

ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

########################################################################
# define the list of source files.
SRCS = SessionManagerMain.cc\
		 SessionManager.cc\
		 IvrWrapperMain.cc\
		 IvrWrapper.cc\
		 IvrPegHandlerMain.cc\
		 IvrPegHandler.cc\
       IvrPegWriter.cc\
       IvrScheduler.cc\
       IvrSchedulerMain.cc\
       IvrChannelHdlr.cc\
       IvrChannelHdlrMain.cc\
       IvrEvrWriter.cc\
       IvrFeedback.cc\
       IvrFeedbackMain.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)

########################################################################
# define the include flags.
INCLFLAGS = -I include\
				-I $(PROJ_HOME)/ivrutil/include\
            -I $(PROJ_HOME)/billing/include\
				-I $(PROJ_HOME)/include\
				-I $(PROJ_HOME)/db/include\
				-I $(MEDIA_HOME)/media/interface/include\
				-I $(MEDIA_HOME)/media/mediautil/include\
				-I $(TPDIR)/SQLAPI/include \
				-I $(UTILDIR)/include \
				-I $(UTILDIR)/msgq/include \
				-I $(UTILDIR)/shmq/include \
				-I $(UTILDIR)/semaphore/include \
				-I $(UTILDIR)/datastructures/include \
				-I $(UTILDIR)/strtok/include \
				-I $(UTILDIR)/socket/include \
				-I $(UTILDIR)/trace/include \
				-I $(UTILDIR)/timer/include \
				-I $(UTILDIR)/octet/include/ \
				-I $(UTILDIR)/filedeletion/include \
				-I $(UTILDIR)/configinfo/include \
				-I $(UTILDIR)/protocolstack/include \
				-I $(UTILDIR)/diskmonitor/include \
				-I $(UTILDIR)/processlock/include \
				-I $(UTILDIR)/diskbuffer/include \
				-I $(UTILDIR)/log/include\
				-I $(UTILDIR)/db/include\
				-I $(UTILDIR)/tssutil/include\
				-I $(UTILDIR)/peg/include\
                                -I $(UTILDIR)/cdr/include\
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/ss7util/include\
                                -I $(TPDIR)/redis-6.2.12/deps/\
#-I $(OKTOROOT)/trd\
				-I $(OKTOROOT)/inet\
				-I $(OKTOROOT)/jsi\
				-I $(OKTOROOT)/Logging

########################################################################
# define the library flags.
ifneq (,$(findstring 64,$(shell getconf LONG_BIT) ))
       #LIBDL=/usr/lib64/libdl.a
        LIBPTHREAD=/usr/lib64/libpthread.so
        LIBSSL=/usr/lib64/libssl.so
        LIBCURL=/usr/lib64/libcurl.so
else
        #LIBDL=/usr/lib/libdl.a
        LIBPTHREAD=/usr/lib/libpthread.so
        LIBSSL=/usr/lib/libssl.so
        LIBCURL=/usr/lib/libcurl.so
endif

LIBFLAGS = $(LIBSSL)\
				$(LIBPTHREAD)\
				$(LIBCURL)\
				$(UTILDIR)/lib/libutil.a \
				$(PROJ_HOME)/ivrutil/lib/libIvrUtil.a\
				$(PROJ_HOME)/db/lib/libIvrDbMgr.a\
            $(TPDIR)/SQLAPI/lib/libsqlapi.a\
				$(MEDIA_HOME)/media/interface/lib/libIvrMedia.a\
            $(MEDIA_HOME)/media/mediautil/lib/libMediaUtil.a\
            $(PROJ_HOME)/billing/lib/libBillingApi.a\
            $(PROJ_HOME)/billing/lib/libBilling.a\
	    #$(LIBDL)\

LIBDIR = lib

########################################################################
# list of generated object files.
OBJS1 = SessionManagerMain.o\
		  SessionManager.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = $(PROJ_HOME)/ivrutil/lib/libIvrRedisUtil.a\
	$(TPDIR)/redis-6.2.12/deps/hiredis/libhiredis.a		
       #/usr/local/lib/libhiredis.a
        
# output file name.
OFILE1 = IVR_SessionManager

# Program dependencies
$(OFILE1): $(BINDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

########################################################################
# list of generated object files.
OBJS2 = IvrWrapperMain.o\
		  IvrWrapper.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2  = 

# output file name.
OFILE2 = IVR_MsgWrapper

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

########################################################################
# list of generated object files.
OBJS3 = IvrPegHandlerMain.o\
		  IvrPegHandler.o

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
LIBS3  = 

# output file name.
OFILE3 = IVR_PegHandler

# Program dependencies
$(OFILE3): $(BINDIR)/$(OFILE3)
$(BINDIR)/$(OFILE3): $(OBJS3) $(LIBS3)

########################################################################
# list of generated object files.
OBJS4 = IvrPegWriter.o

OBJS4 := $(OBJS4:%=$(OBJDIR)/%)

# list of libraries.
LIBS4  = 

# output file name.
OFILE4 = IVR_PegWriter

# Program dependencies
$(OFILE4): $(BINDIR)/$(OFILE4)
$(BINDIR)/$(OFILE4): $(OBJS4) $(LIBS4)
########################################################################
# list of generated object files.
OBJS5 = IvrScheduler.o IvrSchedulerMain.o 

OBJS5 := $(OBJS5:%=$(OBJDIR)/%)

# list of libraries.
LIBS5  = 

# output file name.
OFILE5 = IVR_Scheduler

# Program dependencies
$(OFILE5): $(BINDIR)/$(OFILE5)
$(BINDIR)/$(OFILE5): $(OBJS5) $(LIBS5)
########################################################################
# list of generated object files.
OBJS6 = IvrChannelHdlr.o IvrChannelHdlrMain.o 

OBJS6 := $(OBJS6:%=$(OBJDIR)/%)

# list of libraries.
LIBS6  = 

# output file name.
OFILE6 = IVR_ChannelMgr

# Program dependencies
$(OFILE6): $(BINDIR)/$(OFILE6)
$(BINDIR)/$(OFILE6): $(OBJS6) $(LIBS6)
########################################################################
# list of generated object files.
OBJS7 = IvrFeedback.o IvrFeedbackMain.o 

OBJS7 := $(OBJS7:%=$(OBJDIR)/%)

# list of libraries.
LIBS7  = 

# output file name.
OFILE7 = IVR_Feedback

# Program dependencies
$(OFILE7): $(BINDIR)/$(OFILE7)
$(BINDIR)/$(OFILE7): $(OBJS7) $(LIBS7)
########################################################################
# list of generated object files.
OBJS8 = IvrEvrWriter.o

OBJS8 := $(OBJS8:%=$(OBJDIR)/%)

# list of libraries.
LIBS8 =

# output file name.
OFILE8 = IVR_EvrWriter

# Program dependencies
$(OFILE8): $(BINDIR)/$(OFILE8)
$(BINDIR)/$(OFILE8): $(OBJS8) $(LIBS8)
########################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) $(OBJS5) $(OBJS6) $(OBJS7) $(OBJS8) 

########################################################################
# List of all libraries to be created
OLIB = #$(OFILE1) #$(OFILE7)
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE1) $(OFILE2)  $(OFILE5) $(OFILE6) $(OFILE7) $(OFILE8) 
PROG := $(PROG:%=$(BINDIR)/%)
########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
