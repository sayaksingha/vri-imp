# @(#) $Id: Makefile,v 1.1.4.3 2016/04/12 21:59:40 shijuc Exp $
# @(#) $Id: Makefile,v 1.1.4.3 2016/04/12 21:59:40 shijuc Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod
DEFS+=-DDTD
DEFS+=-DTHREAD_SAVE

#################################################################
# define the list of source files.

SRCS = StubMPInput_single.cc\
       StubMPOutput.cc\
       StubMPOutput_Load.cc\
       StubChnlMgmtInput_single.cc\
       SNWChannelDisplay.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)

INCLFLAGS = -I include\
            -I $(ROOTDIR)/include\
            -I $(ROOTDIR)/media/interface/include\
            -I $(ROOTDIR)/media/mediautil/include\
            -I $(ROOTDIR)/media/synway/shd/src/kernel/include\
            -I $(ROOTDIR)/media/synway/shd/src/synwayutil/include\
            -I $(ROOTDIR)/media/synway/shd/src/include\
            -I $(ROOTDIR)/media/include\
            -I $(ROOTDIR)/ivrutil/include\
            -I $(ROOTDIR)/kernel/include\
				-I $(TPDIR)/media/synway/include\
            -I $(UTILDIR)/configinfo/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/trace/include\
            -I $(UTILDIR)/datastructures/include\
            -I $(UTILDIR)/diskmonitor/include\
            -I $(UTILDIR)/msgq/include\
            -I $(UTILDIR)/semaphore/include\
            -I $(UTILDIR)/socket/include\
            -I $(UTILDIR)/strtok/include\
            -I $(UTILDIR)/shmq/include\
            -I $(UTILDIR)/diskbuffer/include \
            -I $(UTILDIR)/timer/include\
            -I $(UTILDIR)/processlock/include\
            -I $(UTILDIR)/protocolstack/include\
            -I $(UTILDIR)/protocolstack/smpp/include\
            -I $(UTILDIR)/protocolstack/cimd2/include\
            -I $(UTILDIR)/octet/include\
            -I $(UTILDIR)/thirdparty/TDAPI/include\
            -I $(UTILDIR)/log/include\
	    -I $(UTILDIR)/db/include\
	    -I $(UTILDIR)/tssutil/include\
	    -I $(UTILDIR)/peg/include\
	    -I $(GATEWAY_HOME)/base/include\
            -I $(SS7_HOME)/include\
	    -I $(SS7_HOME)/ss7util/include\
            -I $(TPDIR)/TDAPI/include\
	    -I $(TPDIR)/SQLAPI/include

LIBFLAGS = $(ROOTDIR)/media/mediautil/lib/libMediaUtil.a\
           $(ROOTDIR)/media/synway/shd/src/synwayutil/lib/libSynwayUtil.a\
	   $(UTILDIR)/lib/libutil.a\
			-lshpa3\
			/usr/lib/libssl.so\
#	$(TPDIR)/media/synway/lib/libshpa3.so
LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 =  StubChnlMgmtInput_single.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# librarius required
LIBS1 =	

# output file name.
OFILE1 = SNW_STUB_ChnlMgmtInput_single

# Program dependencies
$(OFILE1): $(BINDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# list of generated object files.
OBJS2 =  StubMPOutput.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# librarius required
LIBS2 = 

# output file name.
OFILE2 = SNW_STUB_MPOutput

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)
#################################################################
# list of generated object files.
OBJS3 =  StubMPInput_single.o

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# librarius required
LIBS3 =

# output file name.
OFILE3 = SNW_STUB_MPInput_single

# Program dependencies
$(OFILE3): $(BINDIR)/$(OFILE3)
$(BINDIR)/$(OFILE3): $(OBJS3) $(LIBS3)
#################################################################
# list of generated object files.
OBJS4 =  StubMPOutput_Load.o

OBJS4 := $(OBJS4:%=$(OBJDIR)/%)

# librarius required
LIBS4 =

# output file name.
OFILE4 = SNW_STUB_MPOutput_Load

# Program dependencies
$(OFILE4): $(BINDIR)/$(OFILE4)
$(BINDIR)/$(OFILE4): $(OBJS4) $(LIBS4)
#################################################################
# list of generated object files.
OBJS5 =  SNWChannelDisplay.o

OBJS5 := $(OBJS5:%=$(OBJDIR)/%)

# librarius required
LIBS5 =

# output file name.
OFILE5 = SNW_ChannelDisplay

# Program dependencies
$(OFILE5): $(BINDIR)/$(OFILE5)
$(BINDIR)/$(OFILE5): $(OBJS5) $(LIBS5)
#################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5)

# List of all executables to be created
PROG = 
PROG := $(PROG:%=$(BINDIR)/%)

# List of all libraries to be created
OLIB = 
OLIB := $(OLIB:%=$(LIBDIR)/%)

# List of all stubs to be created
STUBS = $(OFILE1) $(OFILE2) $(OFILE3) $(OFILE4) $(OFILE5)
STUBS := $(STUBS:%=$(BINDIR)/%)

#################################################################

include $(ROOTDIR)/media/makeutil/footer.mod
