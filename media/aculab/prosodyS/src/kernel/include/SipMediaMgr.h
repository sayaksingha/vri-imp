// @(#) $Id: SipMediaMgr.h,v 1.1.4.1.2.5 2018/02/26 23:17:35 mram Exp $
//----------------------------------------------------------------------
// NAME
//       SipMediaMgr.h
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
#ifndef INCL_SIP_1xProc_DEF_H
#define INCL_SIP_1xProc_DEF_H

#define TING_TRACE_MIN 0
#define TING_TRACE_MAX 20

#include "SipStructs.h"
#include "SipUtil.h"
#include "SipQueApi.h"
#include "SipHandleApi.h"
#include "SipChannelApi.h"
//#include "Timer.h"
#include "MultiLevelHashTimer.h"

enum EnumSipMessage
{
   SIP_NONE,
   SIP_INVITE,
   SIP_TRYING_100,
   SIP_RINGING_180,
   SIP_OK_200,
   SIP_ACK,
   SIP_BYE
};


class SipMediaMgr
{
   public:
      SipMediaMgr();

      ~SipMediaMgr();

      BOOLEAN Init();
      
      BOOLEAN DeInit();
      BOOLEAN DeInitializeInCall(INT16 index);
      
      BOOLEAN ReInitialize(int);
      
      BOOLEAN ReloadConfig();

      BOOLEAN ReadApplnMsg(GenIsupMsg&);

      BOOLEAN ProcessApplnMsg(GenIsupMsg&);

      BOOLEAN ProcessEvents();

      BOOLEAN ReadMediaMsg();

      BOOLEAN ProcessMediaMsg();

      BOOLEAN HandleDtmfDetection(UINT16, SM_RECOGNISED_PARMS &, MpResponseDetails & );

      BOOLEAN HandlePlayFinish(UINT16 , MpResponseDetails & );

      BOOLEAN HandleRecordFinish(UINT16, MpResponseDetails&);

      BOOLEAN StartDtmfTimer(UINT32 index, INT16 timeout = 10);

      //BOOLEAN CancelDtmfTimer(tag  responseTimerTag);
      BOOLEAN CancelDtmfTimer(UINT32 index);

      BOOLEAN StopRecord(INT16 index);

      static UINT32 HandleDtmfTimeout(void* obj, UINT32 index);

      //MLHTimer mTimer;
      BOOLEAN ChnlLock(int semNum, TEXT * function);
      
      BOOLEAN ChnlUnlock(int semNum, TEXT * function);
   private:


      BOOLEAN ReadConfig();

      BOOLEAN InitializeCard();

      BOOLEAN InitializeInCall(INT16 index);

      BOOLEAN SendEventToAppln(EnumSipMessage lEvent);

      BOOLEAN FormApplnInvite();

      BOOLEAN FormApplnRinging();

      BOOLEAN FormApplnConnected();

      BOOLEAN FormApplnSipAck();

      BOOLEAN FormApplnBye();

      BOOLEAN FormAculabRingingMsg(UINT16);

      BOOLEAN FormAculabAcceptMsg(UINT16);
      
      BOOLEAN FormAculabDisconnectMsg(UINT16);

      TEXT * DecodeAddress(UINT16, TEXT *lUri);

      BOOLEAN CreateProsodyEvents();

      BOOLEAN CreateMediaControlThread();

      EnumVoiceProcessErr HandlePlay(const MpPlay playMsg);

      BOOLEAN StartPlay(const MpPlay playMsg);

      EnumVoiceProcessErr HandleStartRecord(const MpRecord recMsg);

      BOOLEAN StartRecord(const MpRecord recMsg);

      EnumVoiceProcessErr HandleDtmf(const MpGetDigit dtmfMsg);

      BOOLEAN ListenForDtmf(const MpGetDigit dtmfMsg);

      BOOLEAN IssuePlayStop();
   
      EnumVoiceProcessErr HandleStop(const MpStop stopMsg);

      BOOLEAN StopPlay(INT16);

      BOOLEAN IssueGetDigitStop();

      EnumVoiceProcessErr CancelGetDtmfDigits(const MpStop stopMsg);

      BOOLEAN CancelDtmf(INT16 index);

      BOOLEAN InitiateAculabInviteMsg(GenIsupMsg & lTxIsupMsg);

      BOOLEAN EncodeAddress(UINT16 index, TEXT *lUser, TEXT *lUri,
                  BOOLEAN lDomainFlag);

      EnumVoiceProcessErr HandleStopRecord(const MpStop stopMsg);


   private:
      ACU_SNAPSHOT_PARMS      mSnapshotParms;
      ACU_OPEN_CARD_PARMS     mOpenCardParms;
      DETAIL_XPARMS           mDetailXparms;
      INCOMING_RINGING_XPARMS  mRingingXparams;

      SipCallHandlers         mSipCallHandlers;
      //SipChannel              mSipChannel;
      GenIsupMsg              mRxIsupMsg;
      GenIsupMsg              mTxIsupMsg;


      TEXT                    mLogText[TSS_MAX_LOG_TEXT_LEN + 1];
      TEXT                    m1xstr[TSS_MAX_LOG_TEXT_LEN + 1];
      TEXT                    mOrigDomain[SS7_MAX_IP_LEN + 1];
      TEXT                    mDestDomain[SS7_MAX_IP_LEN + 1];

      int                     mTingTrace;
      INT16                   mSipLicenseIncoming;
      INT16                   mSipLicenseOutgoing;
      INT16                   mSipApplicationType;
      INT16                   mOutCalls;
      INT16                   mRetryTimerRandomTimeOut;
      INT32                   mDataFormat;
      INT32                   mTimerTimeOutQueLimit;
      UINT32                  mTimerSize;

      key_t                   mTimerSemKey;
      key_t                   mTimerHashShmKey;
      key_t                   mTimerHashSemKey;
      key_t                   mTimerBfrShmKey; 
      key_t                   mTimerBfrSemKey;
      key_t                   mTimerTimeOutQueKey;
      key_t                   mChnlStSemKey;
      int                     mSemChnlMgmtId;
      int                     mMsgQueFlushCnt;


   public:
      SipQueApi               mSipQueApi;
      MpRequestDetails        mMpReq;
      tSMEventId              mPlayEvent;
      tSMEventId              mRecEvent;
      tSMEventId              mDtmfEvent;
};
#endif
