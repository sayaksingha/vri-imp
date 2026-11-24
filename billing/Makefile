# @(#) $Id: Makefile,v 1.1.4.3.2.1 2024/03/01 09:13:40 mram Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

#DEFS += -DCOMMONOAM -rdynamic

########################################################################
# define the list of source files.
SRCS = BillingApi.cc\
		 BillingUtil.cc\
       MmpPackageBilling.cc\
		 BillingServer.cc\
		 BillingServerMain.cc\
       BillingSem.cc\
       BillingShmManager.cc\
		 \

SRCS := $(SRCS:%=$(SRCDIR)/%)

########################################################################
# define the include flags.
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/include\
            -I $(PROJ_HOME)/ivrutil/include\
            -I $(PROJ_HOME)/db/include\
            -I $(PROJ_HOME)/mmp/include\
            -I $(PROJ_HOME)/mmp/db/include\
            -I $(PROJ_HOME)/mmp/mmputil/include\
            -I $(PROJ_HOME)/mmp/billing/include\
				-I $(MEDIA_HOME)/media/interface/include\
				-I $(MEDIA_HOME)/media/mediautil/include\
            -I $(CGW_HOME)/include \
            -I $(CGW_HOME)/cgwapi/include \
            -I $(CGW_HOME)/cgwutil/include \
            -I $(GGI_HOME)/include \
            -I $(GGI_HOME)/ggiutil/include \
            -I $(GGI_HOME)/db/include \
            -I $(GATEWAY_HOME)/base/include\
            -I $(GATEWAY_HOME)/include\
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
            -I $(SS7_HOME)/Telesoft/include \
	    -I /usr/include/tirpc/\
            -I $(SS7_HOME)/db/include \

########################################################################
# define the library flags.
ifneq (,$(findstring 64,$(shell getconf LONG_BIT) ))
	LIBDL=-ldl
	LIBPTHREAD=/usr/lib64/libpthread.so
	LIBSSL=/usr/lib64/libssl.so
	LIBCURL=/usr/lib64/libcurl.so
else
	LIBDL=/usr/lib/libdl.a
	LIBPTHREAD=/usr/lib/libpthread.so
	LIBSSL=/usr/lib/libssl.so
	LIBCURL=/usr/lib/libcurl.so
endif

LIBFLAGS =  $(LIBDL)\
	    $(LIBSSL)\
	    $(LIBPTHREAD)\
	    $(LIBCURL)\
            $(GGI_HOME)/ggiutil/lib/libGgiUtil.a \
            $(GGI_HOME)/ggiutil/lib/libGgiApi.a \
            $(GGI_HOME)/db/lib/libGgiDb.a\
				$(UTILDIR)/lib/libutil.a \
				$(PROJ_HOME)/db/lib/libIvrDbMgr.a\
				$(PROJ_HOME)/ivrutil/lib/libIvrUtil.a\
				$(PROJ_HOME)/mmp/mmputil/lib/libMmpUtil.a\
            $(PROJ_HOME)/mmp/db/lib/libMmpDbMgr.a\
            $(TPDIR)/SQLAPI/lib/libsqlapi.a
            #$(SS7_HOME)/db/lib/libSs7Db.a\


LIBDIR = lib

########################################################################
# list of generated object files.
OBJS1 = BillingApi.o\
		  BillingUtil.o\
		  \
		  \

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 
        
# output file name.
OFILE1 = libBillingApi.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

########################################################################
# list of generated object files.
OBJS2 = MmpPackageBilling.o\
        BillingShmManager.o\
        BillingSem.o\
        BillingUtil.o\

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 = \
		  \
        
# output file name.
OFILE2 = libBilling.a
 
# Program dependencies
$(OFILE2): $(LIBDIR)/$(OFILE2)
$(LIBDIR)/$(OFILE2): $(OBJS2) $(LIBS2)

########################################################################
# list of generated object files.
OBJS3 = MmpPackageBilling.o\
		  BillingServer.o\
		  BillingServerMain.o\

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
LIBS3 = lib/libBilling.a\
        
# output file name.
OFILE3 = IVR_BillingServer

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
OBJS = $(OBJS1) $(OBJS2) $(OBJS3) # $(OBJS4)

########################################################################
# List of all libraries to be created
OLIB = $(OFILE1) $(OFILE2) 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE3) #$(OFILE3) #$(OFILE4)
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
