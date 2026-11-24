# @(#) $Id: Makefile,v 1.1.1.1.2.3.2.3 2025/01/06 11:07:34 mram Exp $
 
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

#DEFS +=  -Danand

#################################################################
# define the list of source files.
SRCS =   IvrUtil.cc\
			IvrQueApi.cc\
			IvrChannelApi.cc\
			IvrXmlFileApi.cc\
         IvrConfigApi.cc\
         IvrEvrApi.cc\
			IvrRedisUtil.cc
#         PduDump.cc\
         
SRCS := $(SRCS:%=$(SRCDIR)/%)

#################################################################
# define the include flags.
INCLFLAGS = -I include\
            -I $(ROOTDIR)/include\
            -I $(ROOTDIR)/db/include\
            -I $(ROOTDIR)/billing/include\
            -I $(ROOTDIR)/kernel/include\
            -I $(SS7_HOME)/include\
	    -I $(SS7_HOME)/TDAPI/include\
            -I $(UTILDIR)/log/include\
	    -I $(SS7_HOME)/include\
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
            -I $(UTILDIR)/db/include\
            -I $(UTILDIR)/tssutil/include\
	    -I $(UTILDIR)/peg/include\
            -I $(CGW_HOME)/include \
            -I $(CGW_HOME)/cgwutil/include \
            -I $(CGW_HOME)/cgwapi/include \
            -I $(TPDIR)/TDAPI1800/include\
            -I $(TPDIR)/SQLAPI/include\
	    -I $(TPDIR)/redis-6.2.6/deps/\
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/Telesoft/include\
            -I $(ROOTDIR)/media/interface/include\
            -I $(ROOTDIR)/media/mediautil/include\
				#-I $(SS7_HOME)/ss7util/include\

#################################################################
# define the library flags.
LIBFLAGS = 

LIBDIR = lib


#################################################################
# list of generated object files.
OBJS1 = #IvrOpRefApi.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 

# output file name.
OFILE1 = #libIvrRefApi.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

################################################################# 
# list of generated object files.
OBJS2 = IvrUtil.o\
		  IvrQueApi.o\
		  IvrChannelApi.o\
		  IvrXmlFileApi.o\
        IvrConfigApi.o\
        IvrEvrApi.o
#        PduDump.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 =                      

# output file name.
OFILE2 = libIvrUtil.a

# Program dependencies
$(OFILE2): $(LIBDIR)/$(OFILE2)
$(LIBDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

#################################################################
# list of generated object files.
OBJS3 = IvrRedisUtil.o

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.

LIBS3 =  $(TPDIR)/redis-6.2.12/deps/hiredis/libhiredis.a
#LIBS3 = /usr/local/lib/libhiredis.a 

# output file name.
OFILE3 = libIvrRedisUtil.a

# Program dependencies
$(OFILE3): $(LIBDIR)/$(OFILE3)
$(LIBDIR)/$(OFILE3): $(OBJS3) $(LIBS3)

################################################################# 


# List of all object files
OBJS = $(OBJS2) $(OBJS3)

################################################################# 
# List of all executables to be created
PROG = 
PROG := $(PROG:%=$(BINDIR)/%)  

################################################################# 
# List of all libraries to be created
OLIB = $(OFILE2) $(OFILE3)
OLIB := $(OLIB:%=$(LIBDIR)/%)

#################################################################

include $(ROOTDIR)/makeutil/footer.mod
