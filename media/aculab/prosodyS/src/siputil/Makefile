# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

DEFS += -DCOMMONOAM

######################################################################
# define the include flags.
# define the list of source files.
SRCS = SipUtil.cc\
       SipQueApi.cc\
		 SipHandleApi.cc\
		 SipChannelApi.cc
#SipEvrApi.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
				-I $(PROJ_HOME)/media/mediautil/include\
				-I $(PROJ_HOME)/media/interface/include\
            -I $(PROJ_HOME)/include\
            -I $(PROJ_HOME)/media/aculab/prosodyS/src/siputil/include\
            -I $(PROJ_HOME)/media/aculab/prosodyS/src/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/log/include\
            -I $(UTILDIR)/cdr/include\
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
            -I $(UTILDIR)/peg/include\
				-I $(TPDIR)/ACULAB/include\
				-I $(TPDIR)/ACULAB/TiNG/include\
				-I $(SS7_HOME)/include\

######################################################################
# define the library flags.

LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = SipUtil.o SipQueApi.o #SipEvrApi.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%) 

# list of libraries.
LIBS1 =

# output file name.
OFILE1 = libSipUtil.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# list of generated object files.
OBJS2 = SipHandleApi.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 =

# output file name.
OFILE2 = libSipHandle.a

# Program dependencies
$(OFILE2): $(LIBDIR)/$(OFILE2)
$(LIBDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

#################################################################
# list of generated object files.
OBJS3 = SipChannelApi.o

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
LIBS3 =

# output file name.
OFILE3 = libSipChannel.a

# Program dependencies
$(OFILE3): $(LIBDIR)/$(OFILE3)
$(LIBDIR)/$(OFILE3): $(OBJS3) $(LIBS3)


######################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) $(OBJS3)

########################################################################
# List of all libraries to be created
OLIB = $(OFILE1) $(OFILE2) $(OFILE3) 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG =
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
