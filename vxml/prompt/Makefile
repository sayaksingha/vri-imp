# @(#) $Id: Makefile,v 1.1.1.1.2.3 2016/04/12 22:00:15 shijuc Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

SONAME=libPromptApi.so.0
#################################################################
# define the list of source files.
SRCS = PromptApi.cc\

SRCS := $(SRCS:%=$(SRCDIR)/%)

#################################################################
# define the include flags.
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/include\
            -I $(PROJ_HOME)/ivrutil/include\
            -I $(PROJ_HOME)/db/include\
            -I $(PROJ_HOME)/provisioning/include\
            -I $(PROJ_HOME)/billing/include\
            -I $(SS7_HOME)/include\
            -I $(SS7_HOME)/sm-tl/include\
				-I $(TTS_HOME)/ivrutil/include\
            -I $(UTILDIR)/configinfo/include\
            -I $(GGI_HOME)/include\
            -I $(CG_HOME)/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/trace/include\
            -I $(UTILDIR)/datastructures/include\
            -I $(UTILDIR)/diskmonitor/include\
            -I $(UTILDIR)/semaphore/include\
            -I $(UTILDIR)/socket/include\
            -I $(UTILDIR)/strtok/include\
            -I $(UTILDIR)/msgq/include\
            -I $(UTILDIR)/shmq/include\
            -I $(UTILDIR)/timer/include\
            -I $(UTILDIR)/processlock/include\
            -I $(UTILDIR)/protocolstack/include\
	    -I $(UTILDIR)/log/include\
	    -I $(UTILDIR)/db/include\
	    -I $(UTILDIR)/tssutil/include\
	    -I $(UTILDIR)/peg/include\
            -I $(TPDIR)/TDAPI/include\
            -I $(TPDIR)/SQLAPI/include\
				-I $(SWISBSDK)/src/VXIinterfaces\
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/ss7util/include\
				-I $(PROJ_HOME)/MMP/billing/include\
            -I $(MEDIA_HOME)/media/interface/include\
            -I $(MEDIA_HOME)/media/mediautil/include\
# define the library flags.
#################################################################
LIBFLAGS =  $(LIBDL) $(LIBSSL) $(LIBPTHREAD) $(LIBCURL) \
				$(UTILDIR)/lib/libutil.a \
				$(PROJ_HOME)/ivrutil/lib/libIvrUtil.a\
				$(MEDIA_HOME)/media/interface/lib/libIvrMedia.a\

LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = PromptApi.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 =  

# output file name.
OFILE1 = libPromptApi.so.0

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

################################################################# 
# List of all object files
OBJS = $(OBJS1)# $(OBJS2) $(OBJS3)

# List of all executables to be created
PROG = 
PROG := $(PROG:%=$(BINDIR)/%)  

# List of all libraries(dynamic) to be created
OSLIB = $(OFILE1)
OSLIB := $(OSLIB:%=$(LIBDIR)/%)

# List of all libraries to be created
OLIB = #$(OFILE1) 
#	$(OFILE2) $(OFILE3)
OLIB := $(OLIB:%=$(LIBDIR)/%)

#################################################################

include $(ROOTDIR)/makeutil/footer.mod
