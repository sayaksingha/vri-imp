# @(#) $Id: Makefile,v 1.1.2.4 2016/04/12 21:59:37 shijuc Exp $
ROOTDIR = $(PROJ_HOME)
include $(ROOTDIR)/makeutil/header.mod
#DEFS += -DCOMMONOAM

########################################################################
# define the list of source files.
SRCS = MediaInterface.cc 
		   
SRCS := $(SRCS:%=$(SRCDIR)/%)

INCLFLAGS = -I include\
				-I $(ROOTDIR)/include\
            -I $(ROOTDIR)/media/mediautil/include\
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/ss7util/include\
				-I $(UTILDIR)/include\
				-I $(UTILDIR)/octet/include\
				-I $(UTILDIR)/timer/include\
				-I $(UTILDIR)/datastructures/include\
				-I $(UTILDIR)/configinfo/include\
				-I $(UTILDIR)/trace/include\
				-I $(UTILDIR)/socket/include\
				-I $(UTILDIR)/msgq/include\
				-I $(UTILDIR)/shmq/include\
				-I $(UTILDIR)/strtok/include\
				-I $(UTILDIR)/processlock/include\
				-I $(UTILDIR)/protocolstack/include\
				-I $(UTILDIR)/semaphore/include\
				-I $(UTILDIR)/log/include\
				-I $(UTILDIR)/db/include\
				-I $(UTILDIR)/tssutil/include\
				-I $(UTILDIR)/peg/include\
				-I $(TPDIR)/SQLAPI/include

########################################################################
# define the library flags.
LIBFLAGS =
LIBDIR = lib
########################################################################
# list of generated object files.
OBJS1 = MediaInterface.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 =

# output file name.
OFILE1 = libIvrMedia.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)


#################################################################
# List of all object files
OBJS = $(OBJS1)

#################################################################
# List of all executables to be created
PROG =
PROG := $(PROG:%=$(BINDIR)/%)

#################################################################
# List of all libraries to be created
OLIB = $(OFILE1)
OLIB := $(OLIB:%=$(LIBDIR)/%)

#################################################################

include $(ROOTDIR)/makeutil/footer.mod

