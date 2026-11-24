# @(#) $Id: Makefile,v 1.1.4.3 2016/04/12 21:59:28 shijuc Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod
DEFS+=-DDTD
DEFS+=-DTHREAD_SAVE

#################################################################
# define the list of source files.
SRCS = AculabMpMain.cc\
       AculabMpMgr.cc\

SRCS := $(SRCS:%=$(SRCDIR)/%)

INCLFLAGS = -I include\
            -I $(ROOTDIR)/include\
            -I $(ROOTDIR)/media/interface/include\
            -I $(ROOTDIR)/media/mediautil/include\
            -I $(ROOTDIR)/media/aculab/prosodyX/src/acuutil/include\
            -I $(ROOTDIR)/media/aculab/prosodyX/src/include\
            -I $(ROOTDIR)/media/include\
            -I $(ROOTDIR)/ivrutil/include\
            -I $(ROOTDIR)/kernel/include\
            -I $(OAM_HOME)/include\
            -I $(OAM_HOME)/log/include\
            -I $(OAM_HOME)/oamutil/include\
            -I $(UTILDIR)/configinfo/include\
            -I $(UTILDIR)/include\
            -I $(UTILDIR)/trace/include\
            -I $(UTILDIR)/datastructures/include\
            -I $(UTILDIR)/diskmonitor/include\
            -I $(UTILDIR)/msgq/include\
            -I $(UTILDIR)/semaphore/include\
            -I $(UTILDIR)/socket/include\
            -I $(UTILDIR)/strtok/include\
            -I $(UTILDIR)/shmq/include\
            -I $(UTILDIR)/diskbuffer/include \
            -I $(UTILDIR)/timer/include\
            -I $(UTILDIR)/processlock/include\
            -I $(UTILDIR)/protocolstack/include\
            -I $(UTILDIR)/protocolstack/smpp/include\
            -I $(UTILDIR)/protocolstack/cimd2/include\
            -I $(UTILDIR)/octet/include\
            -I $(UTILDIR)/thirdparty/TDAPI/include\
            -I $(GATEWAY_HOME)/base/include\
				-I $(TPDIR)/media/aculab/include\
            -I $(TPDIR)/TDAPI/include\
            -I $(TPDIR)/ACULAB/include\
            -I $(TPDIR)/ACULAB/TiNG/include\
            -I $(TPDIR)/ACULAB/ProsodyLibraries/Group3Fax/API/include\


LIBFLAGS = $(ROOTDIR)/media/mediautil/lib/libMediaUtil.a\
           $(UTILDIR)/lib/libutil.a\
           $(OAM_HOME)/lib/libOamUtil.a\
           $(OAM_HOME)/lib/libLogApi.a\
           $(TPDIR)/ACULAB/lib/libacu_res.so\
           $(TPDIR)/ACULAB/lib/libTiNGshared.so\
           $(TPDIR)/ACULAB/lib/libacu_sw.so\
           $(TPDIR)/ACULAB/lib/libacu_cl.so\
			  -lshpa3\
			  /usr/lib/libssl.so
           
LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = AculabMpMain.o AculabMpMgr.o 

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = $(ROOTDIR)/media/aculab/prosodyX/src/acuutil/lib/libAcuMediaUtil.a\

# output file name.
OFILE1 = ACU_MediaProcess

# Program dependencies
$(OFILE1): $(BINDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# List of all object files
OBJS = $(OBJS1)

# List of all executables to be created
PROG =  $(OFILE1)
PROG := $(PROG:%=$(BINDIR)/%)

# List of all libraries to be created
OLIB = #$(OFILE1)
OLIB := $(OLIB:%=$(LIBDIR)/%)

#################################################################

include $(ROOTDIR)/media/makeutil/footer.mod
