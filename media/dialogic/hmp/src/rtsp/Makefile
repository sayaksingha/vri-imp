# @(#) $Id:
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod
#DEFS += -DCOMMONOAM
#DEFS += -DCOMMONOAM -rdynamic
######################################################################
# define the include flags.
# define the list of source files.
SRCS = RtspClient.cc\
       RtspEnv.cc\

SRCS := $(SRCS:%=$(SRCDIR)/%)
######################################################################
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/include\
            -I $(LIVE555)/UsageEnvironment/include\
            -I $(LIVE555)/groupsock/include\
            -I $(LIVE555)/liveMedia/include\
            -I $(LIVE555)/BasicUsageEnvironment/include\
#-I $(UTILDIR)/datastructures/include\

######################################################################
               LIBFLAGS =  

               LIBDIR = lib

######################################################################
# list of generated object files.
               OBJS1 =  RtspClient.o\
                        RtspEnv.o\

                        OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
   LIBS1 = $(LIVE555)/liveMedia/libliveMedia.a\
           $(LIVE555)/groupsock/libgroupsock.a\
           $(LIVE555)/BasicUsageEnvironment/libBasicUsageEnvironment.a\
           $(LIVE555)/UsageEnvironment/libUsageEnvironment.a\

# output file name. 
           OFILE1 = libHmpRtsp.a

# Program dependencies
   $(OFILE1): $(LIBDIR)/$(OFILE1)
                $(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# List of all object files
                                       OBJS = $(OBJS1)

########################################################################
# List of all libraries to be created
   OLIB = $(OFILE1) 
OLIB := $(OLIB:%=$(LIBDIR)/%)

########################################################################

   include $(ROOTDIR)/media/makeutil/footer.mod
# DO NOT DELETE
