// @(#) $Id: AcuMediaStructs.h,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $
//----------------------------------------------------------------------
// NAME 
// AcuMediaStructs.h
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2012
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated :                                             21-JUL--2012
//----------------------------------------------------------------------
#ifndef INCL_ACU_MEDIA_STRUCT_DEF_H
#define INCL_ACU_MEDIA_STRUCT_DEF_H

enum EnumAcuPlaystatus
{
   ACU_PLAY_NEXT_NOT_REQD            = 0, //Request came when channel status is idle
   ACU_PLAY_NEXT_REQD                = 1, //Request came when channel status is stopping/getdtmf
   ACU_PLAY_NEXT_IMMEDIATE           = 2, //Request came when channel status is playing
   ACU_PLAY_MULTIPLE_FILES           = 3,
   ACU_PLAY_MULTIPLE_FILES_COMPLETED = 4,
   ACU_PLAY_STOP_CMD                 = 5
};

enum EnumAcuFaxStatus
{
   ACU_FAX_PROGRESS,
   ACU_FAX_MULTIPLE_FILES,
   ACU_FAX_MULTIPLE_FILES_COMPLETED
};

enum EnumAcuPlayCtrl
{
   ACU_PLAY_NONE,
   ACU_PLAY_PAUSE,
   ACU_PLAY_RESUME,
   ACU_PLAY_FORWARD,
   ACU_PLAY_REWIND
};

enum EnumAcuTingTrace
{
   ACU_TING_TRACE_NONE = 1,
   ACU_TING_TRACE_ENABLE  = 3
};

enum EnumAcuMediaTrace
{
   ACU_MEDIA_TRACE_NONE = 0,
   ACU_MEDIA_TRACE_INIT = 1,
   ACU_MEDIA_TRACE_PLAY = 2,
   ACU_MEDIA_TRACE_RECORD = 3,
   ACU_MEDIA_TRACE_DTMF = 4,
   ACU_MEDIA_TRACE_ALL  = 5
};

enum EnumAcuDsp
{
   ACU_TWO_DSP_PER_CARD   = 2,
   ACU_FOUR_DSP_PER_CARD  = 4,
   ACU_EIGHT_DSP_PER_CARD = 8
};

struct acuStream
{
   int st;
   int ts;
   int type;
};

struct acuStatus
{
   INT32 mediaIndx;
};

struct acuPlay
{
   MpPlay  play;
   UINT16  fileCnt;
   INT16   curFileIndx;
   ULONG   playCtrlPos;
   EnumAcuPlayCtrl playCtrl;
   SM_FILE_REPLAY_PARMS fp;
};

struct acuRecord
{
   MpRecord record;
   SM_FILE_RECORD_PARMS rp;
};

struct acuDtmf
{
   MpGetDigit dtmf;
   UINT16     detectedDigitsLen;
   TEXT       detectedDigits[MAX_DTMF_RSP_DIGITS];
   SM_LISTEN_FOR_PARMS lp;
};

struct acuSpeech
{
   int          pId;
   tSMConference confId;
};

#if 0
struct acuFax 
{
   MpFax             faxMsg;
   EnumAcuFaxStatus  faxStatus;
};
#endif

struct acuChannel
{
   acuStream mediaSt;  //out
   acuStream netSt;   //in

   UINT16   cardId;
   UINT16   circuitId;
   tSMModuleId moduleId;
   tSMChannelId channelId;

   EnumChannelStatus status;
   EnumAcuPlaystatus playStatus;

   acuPlay     acuCurPlay;
   acuRecord   acuCurRecord;
   acuDtmf     acuCurDtmf;
   acuPlay     acuNxtPlay;
   acuSpeech   acuConf;
   //acuFax      acuCurFax;
};

#endif
