// @(#) $Id: SipStructs.h,v 1.1.4.1.2.4 2017/11/14 02:52:39 mram Exp $
//----------------------------------------------------------------------
// NAME
//       SipStructs.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_SIP_STRUCTS_DEF_H
#define INCL_SIP_STRUCTS_DEF_H

#include "SipConstDef.h"
#include "MediaInterface.h"
enum EnumAcuPlaystatus
{
   ACU_PLAY_NEXT_NOT_REQD            = 0, //Request came when channel status is idle
   ACU_PLAY_NEXT_REQD                = 1, //Request came when channel status is stopping/getdtmf
   ACU_PLAY_NEXT_IMMEDIATE           = 2, //Request came when channel status is playing
   ACU_PLAY_MULTIPLE_FILES           = 3,
   ACU_PLAY_MULTIPLE_FILES_COMPLETED = 4,
   ACU_PLAY_STOP_CMD                 = 5,
   ACU_PLAY_MULTIPLE_FILES_INTERUPTED = 6
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

struct acuStatus
{
   INT32 index;
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
   //time_t recStartTime;
   time_t startRecTime;
   MpRecord record;
   SM_FILE_RECORD_PARMS rp;
   BOOLEAN  dtmf;
};

struct acuDtmf
{
   MpGetDigit dtmf;
   UINT16     detectedDigitsLen;
   TEXT       detectedDigits[MAX_DTMF_RSP_DIGITS];
   SM_LISTEN_FOR_PARMS lp;
};

enum EnumCallState
{
   CALL_NONE = 0,
   CALL_DISCONNECTED,
   CALL_SCHDL_STOP
};

enum EnumPlayControlState
{
   CONTROL_NONE = 0,
   CONTROL_USER_ABORT,
   CONTROL_SCHDL_STOP,
   CONTROL_CALL_DISCONNECTED,
};

struct SipCallHandlers
{
   ACU_CALL_HANDLE   handle;
   tSMVMPrxId        vmp_rx;
   tSMVMPtxId        vmp_tx;
   tSMChannelId      channel;
   INT32             timerTag;
   INT16             index;
   INT16             callType;
   unsigned int      seqNum;
   BOOLEAN           listenDtmfFlag;
   BOOLEAN           timerFlag;

   TEXT key[SIP_MAX_REQ_KEY_LEN + 1];
   TEXT origAddr[SIP_MAX_URI_LEN + 1];
   TEXT destAddr[SIP_MAX_URI_LEN + 1];

   EnumCallState     callState;
   EnumChannelStatus chnlStatus;
   EnumAcuPlaystatus playStatus;
   EnumPlayControlState playControlStatus;

   acuPlay     acuCurPlay;
   acuRecord   acuCurRecord;
   acuDtmf     acuCurDtmf;
   acuPlay     acuNxtPlay;

   int         playCount;
};

struct SipCallDetails
{
   SipCallHandlers sipCallHdls[2500];
};

struct SipChannel
{
   tSMChannelId      channel;
   
   INT16             index;
   
   TEXT key[SIP_MAX_REQ_KEY_LEN + 1];
};

#endif
