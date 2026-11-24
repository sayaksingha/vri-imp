# @(#) $Id: Makefile,v 1.1.4.3 2016/04/12 21:59:28 shijuc Exp $
ROOTDIR = $(PROJ_HOME)

include $(ROOTDIR)/media/makeutil/header.mod
DEFS+=-DDTD
DEFS+=-DTHREAD_SAVE

#################################################################
# define the list of source files.
SRCS = MediaPlayerMain.cc\
       MediaPlayer.cc\
       MediaRecorderMain.cc\
       MediaRecorder.cc\
       MediaDtmfMain.cc\
       MediaDtmfDetection.cc\
       MediaAsyncPlayerMain.cc\
       MediaAsyncPlayer.cc\
       MediaAsyncDtmfMain.cc\
       MediaAsyncDtmf.cc\
       MediaAsyncRecorderMain.cc\
       MediaAsyncRecorder.cc\
       MediaPlayDtmfMain.cc\
       MediaPlayDtmf.cc\
       MediaPlayRecDtmfMain.cc\
       MediaPlayRecDtmf.cc\
       MediaRecDtmfMain.cc\
       MediaRecDtmf.cc\
       MediaSendFaxMain.cc\
       MediaSendFax.cc\
       MediaRcvFaxMain.cc\
       MediaRcvFax.cc\
       StubWriteMpReq.cc\
       StubWriteLoadMpReq.cc\
       ResetSwitch.cc\
       AcuPlayer.cc\
       StubGenerateFaxFile.cc\
       StubReadModReqMsg.cc

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
            -I $(TPDIR)/ACULAB/TiNG/pubdoc/gen\

LIBFLAGS = $(ROOTDIR)/media/mediautil/lib/libMediaUtil.a\
           $(ROOTDIR)/media/aculab/prosodyX/src/acuutil/lib/libAcuMediaUtil.a\
           $(UTILDIR)/lib/libutil.a\
           $(OAM_HOME)/lib/libOamUtil.a\
           $(OAM_HOME)/lib/libLogApi.a\
           $(TPDIR)/ACULAB/lib/libfaxlib.so\
           $(TPDIR)/ACULAB/lib/libactiff.so\
           $(TPDIR)/ACULAB/lib/libacu_res.so\
           $(TPDIR)/ACULAB/lib/libTiNGshared.so\
           $(TPDIR)/ACULAB/lib/libacu_sw.so\
           $(TPDIR)/ACULAB/lib/libacu_cl.so\
           /usr/lib/libfreetype.so\
           /usr/lib/libfontconfig.so\
			  -lshpa3\
			  /usr/lib/libssl.so
LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = MediaPlayer.o MediaPlayerMain.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 =

# output file name.
OFILE1 = ACU_STUB_Player

# Program dependencies
$(OFILE1): $(BINDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# list of generated object files.
OBJS2 = MediaRecorderMain.o MediaRecorder.o

OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 =

# output file name.
OFILE2 = ACU_STUB_Recorder

# Program dependencies
$(OFILE2): $(BINDIR)/$(OFILE2)
$(BINDIR)/$(OFILE2): $(OBJS2) $(LIBS2)
#################################################################
# list of generated object files.
OBJS3 = MediaDtmfMain.o MediaDtmfDetection.o

OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
LIBS3 =

# output file name.
OFILE3 = ACU_STUB_Dtmf

# Program dependencies
$(OFILE3): $(BINDIR)/$(OFILE3)
$(BINDIR)/$(OFILE3): $(OBJS3) $(LIBS3)

#################################################################
# list of generated object files.
OBJS4 = MediaAsyncDtmfMain.o MediaAsyncDtmf.o

OBJS4 := $(OBJS4:%=$(OBJDIR)/%)

# list of libraries.
LIBS4 =

# output file name.
OFILE4 = ACU_STUB_AsyncDtmf

# Program dependencies
$(OFILE4): $(BINDIR)/$(OFILE4)
$(BINDIR)/$(OFILE4): $(OBJS4) $(LIBS4)

#################################################################
# list of generated object files.
OBJS5 = MediaAsyncPlayerMain.o MediaAsyncPlayer.o

OBJS5 := $(OBJS5:%=$(OBJDIR)/%)

# list of libraries.
LIBS5 =

# output file name.
OFILE5 = ACU_STUB_AsyncPlayer

# Program dependencies
$(OFILE5): $(BINDIR)/$(OFILE5)
$(BINDIR)/$(OFILE5): $(OBJS5) $(LIBS5)

#################################################################
# list of generated object files.
OBJS6 = MediaAsyncRecorderMain.o MediaAsyncRecorder.o

OBJS6 := $(OBJS6:%=$(OBJDIR)/%)

# list of libraries.
LIBS6 =

# output file name.
OFILE6 = ACU_STUB_AsyncRecorder

# Program dependencies
$(OFILE6): $(BINDIR)/$(OFILE6)
$(BINDIR)/$(OFILE6): $(OBJS6) $(LIBS6)

#################################################################
# list of generated object files.
OBJS7 = MediaPlayDtmfMain.o MediaPlayDtmf.o

OBJS7 := $(OBJS7:%=$(OBJDIR)/%)

# list of libraries.
LIBS7 =

# output file name.
OFILE7 = ACU_STUB_PlayDtmf

# Program dependencies
$(OFILE7): $(BINDIR)/$(OFILE7)
$(BINDIR)/$(OFILE7): $(OBJS7) $(LIBS7)

#################################################################
# list of generated object files.
OBJS8 = MediaPlayRecDtmfMain.o MediaPlayRecDtmf.o

OBJS8 := $(OBJS8:%=$(OBJDIR)/%)

# list of libraries.
LIBS8 =

# output file name.
OFILE8 = ACU_STUB_PlayRecDtmf

# Program dependencies
$(OFILE8): $(BINDIR)/$(OFILE8)
$(BINDIR)/$(OFILE8): $(OBJS8) $(LIBS8)

#################################################################
# list of generated object files.
OBJS9 = MediaRecDtmfMain.o MediaRecDtmf.o

OBJS9 := $(OBJS9:%=$(OBJDIR)/%)

# list of libraries.
LIBS9 =

# output file name.
OFILE9 = ACU_STUB_RecDtmf

# Program dependencies
$(OFILE9): $(BINDIR)/$(OFILE9)
$(BINDIR)/$(OFILE9): $(OBJS9) $(LIBS9)

#################################################################
# list of generated object files.
OBJS10 = StubWriteMpReq.o

OBJS10 := $(OBJS10:%=$(OBJDIR)/%)

# list of libraries.
LIBS10 =

# output file name.
OFILE10 = ACU_STUB_WriteMpReq

# Program dependencies
$(OFILE10): $(BINDIR)/$(OFILE10)
$(BINDIR)/$(OFILE10): $(OBJS10) $(LIBS10)

#################################################################
# list of generated object files.
OBJS11 = MediaSendFaxMain.o MediaSendFax.o

OBJS11 := $(OBJS11:%=$(OBJDIR)/%)

# list of libraries.
LIBS11 =

# output file name.
OFILE11 = ACU_STUB_SendFax

# Program dependencies
$(OFILE11): $(BINDIR)/$(OFILE11)
$(BINDIR)/$(OFILE11): $(OBJS11) $(LIBS11)

#################################################################
# list of generated object files.
OBJS12 = MediaRcvFaxMain.o MediaRcvFax.o

OBJS12 := $(OBJS12:%=$(OBJDIR)/%)

# list of libraries.
LIBS12 =

# output file name.
OFILE12 = ACU_STUB_RcvFax

# Program dependencies
$(OFILE12): $(BINDIR)/$(OFILE12)
$(BINDIR)/$(OFILE12): $(OBJS12) $(LIBS12)

#################################################################
# list of generated object files.
OBJS13 = ResetSwitch.o

OBJS13 := $(OBJS13:%=$(OBJDIR)/%)

# list of libraries.
LIBS13 =

# output file name.
OFILE13 = ACU_STUB_ResetSwitch

# Program dependencies
$(OFILE13): $(BINDIR)/$(OFILE13)
$(BINDIR)/$(OFILE13): $(OBJS13) $(LIBS13)

#################################################################
# list of generated object files.
OBJS14 = StubWriteLoadMpReq.o

OBJS14 := $(OBJS14:%=$(OBJDIR)/%)

# list of libraries.
LIBS14 =

# output file name.
OFILE14 = ACU_STUB_AcuMediaLoad

# Program dependencies
$(OFILE14): $(BINDIR)/$(OFILE14)
$(BINDIR)/$(OFILE14): $(OBJS14) $(LIBS14)

#################################################################
# list of generated object files.
OBJS15 = AcuPlayer.o

OBJS15 := $(OBJS15:%=$(OBJDIR)/%)

# list of libraries.
LIBS15 =

# output file name.
OFILE15 = ACU_STUB_PLAYER

# Program dependencies
$(OFILE15): $(BINDIR)/$(OFILE15)
$(BINDIR)/$(OFILE15): $(OBJS15) $(LIBS15)

#################################################################
# list of generated object files.
OBJS16 = StubGenerateFaxFile.o

OBJS16 := $(OBJS16:%=$(OBJDIR)/%)

# list of libraries.
LIBS16 =

# output file name.
OFILE16 = ACU_STUB_GenActiffFile

# Program dependencies
$(OFILE16): $(BINDIR)/$(OFILE16)
$(BINDIR)/$(OFILE16): $(OBJS16) $(LIBS16)

#################################################################
# list of generated object files.
OBJS17 = StubReadModReqMsg.o

OBJS17 := $(OBJS17:%=$(OBJDIR)/%)

# list of libraries.
LIBS17 =

# output file name.
OFILE17 = ACU_STUB_ReadModReqWithType

# Program dependencies
$(OFILE17): $(BINDIR)/$(OFILE17)
$(BINDIR)/$(OFILE17): $(OBJS17) $(LIBS17)

#################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) $(OBJS7)\
 $(OBJS8) $(OBJS9) $(OBJS10) $(OBJS11) $(OBJS12) $(OBJS13) $(OBJS14)\
 $(OBJS15) $(OBJS16) $(OBJS17)

# List of all executables to be created
#PROG =  $(OFILE1) $(OFILE2) $(OFILE3) $(OFILE4) $(OFILE5) $(OFILE6)\
 $(OFILE7) $(OFILE8) $(OFILE9) $(OFILE10) $(OFILE11) $(OFILE12) $(OFILE13)\
 $(OFILE14) $(OFILE15) $(OFILE16) $(OFILE17)

#PROG := $(PROG:%=$(BINDIR)/%)

# List of all libraries to be created
OLIB = #$(OFILE1)
OLIB := $(OLIB:%=$(LIBDIR)/%)

# List of all stubs to be created
STUBS = $(OFILE1) $(OFILE2) $(OFILE3) $(OFILE4) $(OFILE5) $(OFILE6)\
 $(OFILE7) $(OFILE8) $(OFILE9) $(OFILE10) $(OFILE11) $(OFILE12) $(OFILE13)\
 $(OFILE14) $(OFILE15) $(OFILE16) $(OFILE17)

STUBS := $(STUBS:%=$(BINDIR)/%)

#################################################################

include $(ROOTDIR)/media/makeutil/footer.mod
