# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/makeutil/header.mod

DEFS += -DCOMMONOAM

######################################################################
# define the include flags.
# define the list of source files.
SRCS = SipMediaMgrMain.cc\
       SipMediaMgr.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/include\
				-I $(PROJ_HOME)/media/mediautil/include\
				-I $(PROJ_HOME)/media/interface/include\
				-I $(PROJ_HOME)/media/aculab/prosodyS/src/siputil/include\
				-I $(PROJ_HOME)/media/aculab/prosodyS/src/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/log/include\
            -I $(UTILDIR)/cdr/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/msgq/include\
            -I $(UTILDIR)/shmq/include\
            -I $(UTILDIR)/semaphore/include\
            -I $(UTILDIR)/datastructures/include\
            -I $(UTILDIR)/strtok/include\
            -I $(UTILDIR)/socket/include\
            -I $(UTILDIR)/trace/include\
            -I $(UTILDIR)/timer/include\
            -I $(UTILDIR)/octet/include\
            -I $(UTILDIR)/filedeletion/include\
            -I $(UTILDIR)/configinfo/include\
            -I $(UTILDIR)/protocolstack/include\
            -I $(UTILDIR)/diskmonitor/include\
            -I $(UTILDIR)/processlock/include\
            -I $(UTILDIR)/diskbuffer/include\
            -I $(UTILDIR)/peg/include\
				-I $(TPDIR)/ACULAB/include\
				-I $(TPDIR)/ACULAB/TiNG/include\
				-I $(SS7_HOME)/include\


######################################################################
LIBFLAGS =  $(PROJ_HOME)/media/aculab/prosodyS/src/siputil/lib/libSipUtil.a\
            $(UTILDIR)/lib/libutil.a\
				$(TPDIR)/ACULAB/lib64/libacu_res.so\
				$(TPDIR)/ACULAB/lib64/libTiNGshared.so\
				$(TPDIR)/ACULAB/lib64/libacu_sw.so\
				$(TPDIR)/ACULAB/lib64/libacu_cl.so\
				$(TPDIR)/ACULAB/lib64/libacu_sw.so\
         #/usr/lib/libssl.so\
#/usr/lib/libdl.a\
            /usr/lib/libpthread.so 

LIBDIR = lib

######################################################################
# list of generated object files.
OBJS1 = SipMediaMgrMain.o\
		  SipMediaMgr.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = $(PROJ_HOME)/media/aculab/prosodyS/src/siputil/lib/libSipHandle.a\
		  $(PROJ_HOME)/media/aculab/prosodyS/src/siputil/lib/libSipChannel.a 

# output file name. 
OFILE1 = ACU_SipHandler

# Program dependencies
$(OFILE1): $(BINDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# List of all object files
OBJS = $(OBJS1)

########################################################################
# List of all libraries to be created
OLIB = 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################
# List of all executables to be created
PROG = $(OFILE1)
PROG := $(PROG:%=$(BINDIR)/%)

########################################################################

include $(ROOTDIR)/makeutil/footer.mod
# DO NOT DELETE
