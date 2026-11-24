# @(#) $Id: Makefile,v 1.1.4.3.2.1 2024/03/01 09:51:13 mram Exp $
 
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

#DEFS +=  -Danand

#################################################################
# define the list of source files.
SRCS =   MmpUtil.cc\
         MmpConfigApi.cc\
         MmpQueApi.cc
         
SRCS := $(SRCS:%=$(SRCDIR)/%)

#################################################################
# define the include flags.
INCLFLAGS = -I include\
            -I $(ROOTDIR)/include\
            -I $(ROOTDIR)/ivrutil/include\
            -I $(ROOTDIR)/billing/include\
            -I $(ROOTDIR)/mmp/include\
            -I $(ROOTDIR)/mmp/db/include\
            -I $(ROOTDIR)/mmp/kernel/include\
	    -I $(SS7_HOME)/include\
	    -I $(SS7_HOME)/TDAPI/include\
            -I $(SS7_HOME)/include\
            -I $(SS7_HOME)/ss7util/include\
            -I $(GGI_HOME)/include\
            -I $(GATEWAY_HOME)/base/include\
            -I $(GATEWAY_HOME)/include\
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
	    -I $(CGW_HOME)/include \
            -I $(CGW_HOME)/cgwutil/include \
            -I $(CGW_HOME)/cgwapi/include \
            -I $(TPDIR)/TDAPI1800/include\
            -I $(TPDIR)/SQLAPI/include\
	    -I /usr/include/tirpc/\
	    -I $(SS7_HOME)/include\
	    -I $(SS7_HOME)/Telesoft/include\
	    -I $(SS7_HOME)/ss7util/include

#################################################################
# define the library flags.
LIBFLAGS = 

LIBDIR = lib


#################################################################
# list of generated object files.
OBJS1 = MmpUtil.o\
        MmpConfigApi.o\
        MmpQueApi.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 

# output file name.
OFILE1 = libMmpUtil.a

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
