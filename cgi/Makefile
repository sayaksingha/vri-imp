# @(#) $Id: Makefile,v 1.1.1.1.2.3.2.6 2025/01/10 12:35:33 mram Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

#DEFS += -DCOMMONOAM

########################################################################
# define the list of source files.
SRCS = IvrDbOperation.cc\
       IvrDbOperation_fcgi.cc\
       IvrHttpClient.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)

########################################################################
# define the include flags.
INCLFLAGS = -I include \
				-I $(ROOTDIR)/include\
				-I $(ROOTDIR)/ivrutil/include\
				-I $(ROOTDIR)/db/include\
				-I $(TPDIR)/SQLAPI/include\
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/Telesoft/include\
				-I $(SS7_HOME)/ss7util/include\
				-I $(TPDIR)/TDAPI/include\
				-I $(UTILDIR)/datastructures/include\
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
				-I $(TPDIR)/fcgi/include\

########################################################################
ifneq (,$(findstring 64,$(shell getconf LONG_BIT) ))
        LIBDL=/usr/lib64/libdl.a
        LIBPTHREAD=/usr/lib64/libpthread.so
        LIBSSL=/usr/lib64/libssl.so
        LIBCURL=/usr/lib64/libcurl.so
else
        LIBDL=/usr/lib/libdl.a
        LIBPTHREAD=/usr/lib/libpthread.so
        LIBSSL=/usr/lib/libssl.so
        LIBCURL=/usr/lib/libcurl.so
endif

LIBFLAGS =		  $(UTILDIR)/lib/libutil.a \
			  $(ROOTDIR)/ivrutil/lib/libIvrUtil.a\
			  $(ROOTDIR)/db/lib/libIvrDbMgr.a\
			  $(TPDIR)/SQLAPI/lib/libsqlapi.a\
			  #/usr/local/lib/libfcgi.a\
			  $(LIBDL)\
			  $(LIBSSL)\
          # /usr/local/lib/libfcgi.a


LIBDIR = lib

########################################################################
# list of generated object files.
OBJS1 = IvrDbOperation.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 =  /usr/lib64/libfcgi.so
        
# output file name.
OFILE1 = IVR_ExecuteOperation.cgi

# Program dependencies
$(OFILE1): $(BINDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

########################################################################
# list of generated object files.
OBJS2 = IvrDbOperation_fcgi.o 

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 = /usr/lib64/libfcgi.so 
        
# output file name.
OFILE2 = IVR_ExecuteOperation.fcgi

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)


 ########################################################################
 # list of generated object files.
OBJS3 = IvrHttpClient.o

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

 # list of libraries.
 #$(TPLIB)/libboost_system.a $(TPLIB)/libboost_regex.a
 #LIBS3 = $(TPLIB)/libplustache.a /home/tsstp/cvs/Thirdparty/lib/libboost_system.a /home/tsstp/cvs/Thirdparty/lib/libboost_regex.a
 
LIBS3 = /usr/local/lib/libplustache.a

# output file name.
OFILE3 = IVR_HttpClient.cgi

 # Program dependencies
 $(OFILE3): $(BINDIR)/$(OFILE3)
 $(BINDIR)/$(OFILE3): $(OBJS3) $(LIBS3)

########################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) 

########################################################################
# List of all libraries to be created
OLIB = 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE1) $(OFILE2)  $(OFILE3) 
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
