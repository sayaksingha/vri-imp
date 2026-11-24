# @(#) $Id: Makefile,v 1.1.1.1.2.5.2.2 2024/12/31 12:55:55 mram Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

#DEFS += -DCOMMONOAM

########################################################################
# define the list of source files.
SRCS = OktoEventThread.cc\
		 OktoClient.cc\
		 CcxmlProcessor.cc\
		 CcxmlProcessorMain.cc
#PostCcxmlEvt.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)

########################################################################
# define the include flags.
INCLFLAGS = -I include \
				-I $(ROOTDIR)/include\
				-I $(ROOTDIR)/ivrutil/include\
            -I $(PROJ_HOME)/billing/include\
				-I $(MEDIA_HOME)/media/interface/include\
				-I $(MEDIA_HOME)/media/mediautil/include\
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/ss7util/include\
				-I $(SS7_HOME)/Telesoft/include\
				-I $(TPDIR)/TDAPI/include\
				-I $(OKTOROOT)/cxi\
				-I $(OKTOROOT)/trd\
				-I $(OKTOROOT)/include\
				-I $(OKTOROOT)/inet\
				-I $(OKTOROOT)/jsi\
				-I $(OKTOROOT)/Logging\
				-I $(UTILDIR)/datastructures/include\
				-I $(XERCESCROOT)/include/xercesc\
				-I $(XERCESCROOT)/include\
				-I $(UTILDIR)/include\
				-I $(UTILDIR)/configinfo/include\
				-I $(UTILDIR)/strtok/include\
				-I $(UTILDIR)/trace/include\
				-I $(UTILDIR)/processlock/include\
				-I $(UTILDIR)/semaphore/include\
				-I $(UTILDIR)/socket/include\
				-I $(UTILDIR)/msgq/include\
				-I $(UTILDIR)/timer/include\
				-I $(UTILDIR)/protocolstack/include\
				-I $(UTILDIR)/protocolstack/smpp/include\
				-I $(UTILDIR)/protocolstack/cimd2/include\
				-I $(UTILDIR)/octet/include\
				-I $(UTILDIR)/shmq/include\
				-I $(UTILDIR)/log/include\
				-I $(UTILDIR)/db/include\
				-I $(UTILDIR)/tssutil/include\
				-I $(UTILDIR)/peg/include\
				-I $(TPDIR)/SQLAPI/include
#		-I$(SWISBSDK)/include

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

LIBFLAGS = $(UTILDIR)/lib/libutil.a \
			  $(ROOTDIR)/ivrutil/lib/libIvrUtil.a\
			  $(OKTOROOT)/lib/liboktopous.so\
			  $(OKTOROOT)/lib/libValueTypes.so\
           $(OKTOROOT)/lib/libinet.so\
			  $(OKTOROOT)/lib/libjsi.so\
			  $(OKTOROOT)/lib/libLogging.so\
			  $(OKTOROOT)/lib/libtrd.so\
 			  $(XERCESCROOT)/lib/libxerces-c.so\
			  $(IVR_TOOLS)/js-1.7.0/js/src/Linux_All_OPT.OBJ/libjs.a\
			  $(PCREROOT)/.libs/libpcre.a\
			  $(PCREROOT)/.libs/libpcreposix.a\
            		  $(LIBSSL)\
            		  $(LIBPTHREAD)\
            		  $(LIBCURL)
			  #$(LIBDL)\

LIBDIR = lib

########################################################################
# list of generated object files.
OBJS1 = OktoEventThread.o OktoClient.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 
        
# output file name.
OFILE1 = libCcxml.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

########################################################################
# list of generated object files.
OBJS2 = CcxmlProcessorMain.o CcxmlProcessor.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 = $(ROOTDIR)/ccxml/kernel/lib/libCcxml.a 
        
# output file name.
OFILE2 = IVR_CcxmlProcessor 

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

########################################################################
# list of generated object files.
#OBJS3 = PostCcxmlEvt.o 

#OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
#LIBS3 = $(ROOTDIR)/ccxml/kernel/lib/libCcxml.a 
        
# output file name.
#OFILE3 = IVR_PostEvt

# Program dependencies
#$(OFILE3): $(BINDIR)/$(OFILE3)
#$(BINDIR)/$(OFILE3): $(OBJS3) $(LIBS3)

########################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) #$(OBJS3)

########################################################################
# List of all libraries to be created
OLIB = $(OFILE1) #$(OFILE7)
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE2) #$(OFILE3) 
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
