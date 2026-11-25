# @(#) $Id: Makefile,v 1.1.1.1.2.1 2016/04/12 21:59:03 shijuc Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

#DEFS+=-DORACLE
#DEFS+= -D DB_CLIENT=SA_Oracle_Client
#CFLAGS += -D DB_CLIENT=SA_Oracle_Client
CFLAGS += -D DB_CLIENT=SA_MySQL_Client
#         DB_CLIENT=SA_Client_NotSpecified
#         DB_CLIENT=SA_Oracle_Client
#         DB_CLIENT=SA_SQLServer_Client
#         DB_CLIENT=SA_DB2_Client
#         DB_CLIENT=SA_ODBC_Client
#         DB_CLIENT=SA_Informix_Client
#         DB_CLIENT=SA_Sybase_Client
#################################################################
# define the list of source files.
SRCS = IvrDbManager.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)

#################################################################
# define the include flags.
INCLFLAGS = -Iinclude\
            -I$(ROOTDIR)/include\
            -I$(ROOTDIR)/ivrutil/include\
            -I$(ROOTDIR)/kernel/include\
				-I $(SS7_HOME)/include\
				-I $(SS7_HOME)/ss7util/include\
				-I $(SS7_HOME)/Telesoft/include\
            -I$(LOGDIR)/include\
            -I$(LOGDIR)/log/include\
            -I$(UTILDIR)/include\
            -I$(UTILDIR)/octet/include\
            -I$(UTILDIR)/msgq/include\
            -I$(UTILDIR)/protocolstack/include\
            -I$(UTILDIR)/shmq/include\
            -I$(UTILDIR)/strtok/include\
            -I$(UTILDIR)/semaphore/include\
            -I$(UTILDIR)/datastructures/include\
            -I$(UTILDIR)/processlock/include\
            -I$(UTILDIR)/configinfo/include\
            -I$(UTILDIR)/trace/include\
            -I$(TPDIR)/SQLAPI/include\
            -I$(UTILDIR)/socket/include\
            -I$(UTILDIR)/timer/include\
	    -I $(UTILDIR)/log/include\
	    -I $(UTILDIR)/db/include\
	    -I $(UTILDIR)/tssutil/include\
	    -I $(UTILDIR)/peg/include\
       #     -I$(LOGDIR)/oamutil/cfgread/include\
            -I$(LOGDIR)/oamutil/include

LIBDL = /usr/lib/libdl.a
      
# define the library flags.
LIBFLAGS =  $(ROOTDIR)/ivrutil/lib/libIvrutil.a\
            $(TPDIR)/SQLAPI/lib/libsqlapi.a\
          -L$(UTILDIR)/lib -lutil\
            $(LIBDL)

# define the output library path.
LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = IvrDbManager.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 

# output file name.
OFILE1 = libIvrDbMgr.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# List of all object files
OBJS = $(OBJS1)

# List of all executables to be created
PROG := $(PROG:%=$(BINDIR)/%)

# List of all libraries to be created
OLIB = $(OFILE1)
OLIB := $(OLIB:%=$(LIBDIR)/%)

#################################################################
include $(ROOTDIR)/makeutil/footer.mod
