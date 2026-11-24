# @(#) $Id: Makefile,v 1.1.1.1.2.5.2.2 2025/01/06 11:13:12 mram Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

#DEFS += -DCOMMONOAM -rdynamic

########################################################################
# define the list of source files.
SRCS = VXIclient.c\
		 VXIclientUtils.c\
		 ConfigFile.cc\
		 VXIobject.cc\
		 SBlogListeners.cc\
		 SBlogOSUtils.cc\
		 VxmlProcessor.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)

########################################################################
# define the include flags.
INCLFLAGS = -I include \
				-I $(SWISBSDK)/include\
				-I $(SWISBSDK)/src/VXIclient/include\
				-I $(SWISBSDK)/src/jsi\
				-I $(SWISBSDK)/src/trd\
				-I $(SWISBSDK)/src/char\
				-I $(SWISBSDK)/src/log\
				-I $(SWISBSDK)/src/logMapper\
				-I $(SWISBSDK)/src/cache\
				-I $(SWISBSDK)/src/inet\
				-I $(SWISBSDK)/src/VXI\
				-I $(SWISBSDK)/src/VXIinterfaces\
				-I $(PROJ_HOME)/ivrutil/include\
				-I $(PROJ_HOME)/include\
				-I $(PROJ_HOME)/vxml/prompt/include\
				-I $(PROJ_HOME)/billing/include\
				-I $(PROJ_HOME)/db/include\
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
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/ss7util/include\
            -I $(MEDIA_HOME)/media/mediautil/include\
            -I $(MEDIA_HOME)/media/interface/include/\
				-L $(PROJ_HOME)/lib

########################################################################
# define the library flags.
LIBFLAGS =  $(LIBDL) \
	    $(SWISBSDK)/lib/libSBjsiD.so\
				$(SWISBSDK)/lib/libSBtrdD.so\
				$(SWISBSDK)/lib/libSBcharD.so\
				$(SWISBSDK)/lib/libSBlogD.so\
				$(SWISBSDK)/lib/libSBlogMapperD.so\
				$(SWISBSDK)/lib/libSBcacheD.so\
				$(SWISBSDK)/lib/libSBinetD.so\
				$(SWISBSDK)/lib/libVXIvalueD.so\
				$(SWISBSDK)/lib/libSBtrdUtilD.a\
				$(SWISBSDK)/lib/libVXID.so\
				$(SWISBSDK)/lib/libVXItelD.so\
            $(PROJ_HOME)/ivrutil/lib/libIvrUtil.a \
            $(TPDIR)/SQLAPI/lib/libsqlapi.a\
	    $(UTILDIR)/lib/libutil.a\
		/home/mram/IvrSupportTools/js-1.7.0/js/src/Linux_All_OPT.OBJ/libjs.a\
	   $(PROJ_HOME)/vxml/prompt/lib/libPromptApi.so.0\
            $(MEDIA_HOME)/media/interface/lib/libIvrMedia.a\
            $(MEDIA_HOME)/media/mediautil/lib/libMediaUtil.a\
            $(PROJ_HOME)/billing/lib/libBillingApi.a\
	    $(PROJ_HOME)/db/lib/libIvrDbMgr.a\

#	  			$(XERCESDIR)/lib/libxerces-c.so\
            $(PROJ_HOME)/billing/lib/libBilling.a\


LIBDIR = lib

########################################################################
# list of generated object files.
OBJS1 = VXIclient.o\
		  VXIclientUtils.o\
		  ConfigFile.o\
		  VXIobject.o\
		  SBlogListeners.o\
		  SBlogOSUtils.o


OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 
        
# output file name.
OFILE1 = libVxml.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

########################################################################
# list of generated object files.
OBJS2 = VxmlProcessor.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 = $(PROJ_HOME)/vxml/kernel/lib/libVxml.a \
		  $(PROJ_HOME)/lib/libPromptApi.so.0\

        
# output file name.
OFILE2 = IVR_VxmlProcessor

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

########################################################################
# list of generated object files.
OBJS3 = 

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
LIBS3 = 
        
# output file name.
OFILE3 = 

# Program dependencies
$(OFILE3): $(BINDIR)/$(OFILE3)
$(BINDIR)/$(OFILE3): $(OBJS3) $(LIBS3)

########################################################################
# list of generated object files.
OBJS4 = 

OBJS4 := $(OBJS4:%=$(OBJDIR)/%)

# list of libraries.
LIBS4 = 
        
# output file name.
OFILE4 = 

# Program dependencies
$(OFILE4): $(BINDIR)/$(OFILE4)
$(BINDIR)/$(OFILE4): $(OBJS4) $(LIBS4)

########################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) #$(OBJS3) $(OBJS4)

########################################################################
# List of all libraries to be created
OLIB = $(OFILE1) #$(OFILE7)
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE2) #$(OFILE3) #$(OFILE4)
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
