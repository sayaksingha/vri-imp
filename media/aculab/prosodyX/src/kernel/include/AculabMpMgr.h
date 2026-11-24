// @(#) $Id: AculabMpMgr.h,v 1.1.4.3 2016/04/12 21:59:28 shijuc Exp $
//-----------------------------------------------------------------------------
// NAME           : AculabMpMgr.h
//
// COPYRIGHT      : TSS -- Copyright (C) 2012 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Roopa A M                                Date: 30-JUL-2012
//----------------------------------------------------------------------------

#ifndef INCL_ACULAB_MP_MGR_H
#define INCL_ACULAB_MP_MGR_H

#include "AcuModuleInfo.h"
#include "AcuCardDetails.h"
#include "AcuMsgQueApi.h"
#include "AcuEventApi.h"
#include "AcuMediaStructs.h"

class AculabMPMgr
{
   public :

      AculabMPMgr();

      ~AculabMPMgr();

      //-------------------------------------------------------------------------
      BOOLEAN Init();

      BOOLEAN DeInit(INT16 lModuleIndx);

      BOOLEAN ReloadConfig();

      //----------------------------------------
      BOOLEAN ReadReq();

      INT16   GetMsgType(UINT16 lCktId);

      BOOLEAN DistributeReq();

      BOOLEAN ReadModuleReq(INT16 lModuleIndex);

      BOOLEAN ProcessModuleReq();

      BOOLEAN ProcessReq();

      INT16   SetMsgType(EnumMediaCmdTypes lCmd, MpData &lMpData);

      BOOLEAN WriteRsp(MpResponseDetails &);

      //-------------- Functions Cld by Event Thread  ----------------
      BOOLEAN HandlePlayFinish(UINT16 lChannlIndex, MpResponseDetails & rAcuRsp);

      BOOLEAN HandleRecordFinish(UINT16 lChannlIndex, MpResponseDetails & rAcuRsp);

      BOOLEAN HandleDtmfDetection(SM_RECOGNISED_PARMS & rp, MpResponseDetails & rAcuRsp);

      BOOLEAN HandlePlayCtrlFinish(UINT16 lMediaIndx, TEXT * lProcessName);

      UINT16  GetCircuitId(UINT16 lCktId);

      //----------------------------------------
      int                  mModules;
      int                  mMediaTrace;
      int                  mMaxChannels;

      INT32                mCardId;
      INT32                mDataFormat;
      INT32                mSkipTime;
      UINT32               mSkipOffset;
      AcuEventApi          mAcuEventApi;

   private:
      //----------------------------------------
      BOOLEAN ReadConfig();

      //----------------------------------------
      BOOLEAN InitializeCard();

      INT16 InitialiseChannel(acuStream & rNetStream, acuStream & rMediaStream, INT16 lModuleIndex);

      //----------------------------------------
      BOOLEAN DeinitChannel(UINT32 lTempCardId);

      BOOLEAN ReleaseChannel(tSMChannelId & rChannel);

      //----------------------------------------
      BOOLEAN CheckCircuitId(EnumMediaCmdTypes lCmd, MpData &lMpData);

      EnumChannelStatus  GetMediaChannelStatus(INT16 lMediaChn, TEXT * lTxtStat);

      //----------------------------------------
      EnumVoiceProcessErr HandlePlay(const MpPlay playMsg);

      BOOLEAN StartPlay(const MpPlay playMsg);

      //----------------------------------------
      EnumVoiceProcessErr HandleStartRecord(const MpRecord);

      BOOLEAN StartRecord(const MpRecord);

      //----------------------------------------
      EnumVoiceProcessErr HandleStopRecord(const MpStop);

      BOOLEAN StopRecord(INT16 lCktId);

      //----------------------------------------
      EnumVoiceProcessErr HandleStop(const MpStop);

      BOOLEAN IssuePlayStop();

      BOOLEAN StopPlay(INT16 lCktId);

      //----------------------------------------
      BOOLEAN IssueGetDigitStop();

      EnumVoiceProcessErr CancelGetDtmfDigits(const MpStop, TEXT *);

      BOOLEAN CancelDtmf(INT16 lCktId);

      //----------------------------------------
      EnumVoiceProcessErr HandleDtmf(const MpGetDigit, MpGetDigit &);

      BOOLEAN ListenForDtmf(MpGetDigit, INT16 lCktId);

      //------------- PLAY CONTROLS ------------
      BOOLEAN HandlePlayStopOnDtmf (UINT16 lCktId);

      BOOLEAN HandlePlayCtrls (UINT16 lCktId, int lParam);

      EnumVoiceProcessErr HandlePause(const MpPause pauseMsg);

      EnumVoiceProcessErr HandleResume(const MpResume resumeMsg);

      EnumVoiceProcessErr HandleForward(const MpForward forwardMsg);

      EnumVoiceProcessErr HandleRewind(const MpRewind rewindMsg);

      //----------------------------------------
      EnumVoiceProcessErr ConnectSpeech(const MpSwitch);

      EnumVoiceProcessErr DisconnectSpeech(const MpSwitch);

      //----------------------------------------
      //EnumVoiceProcessErr HandleSendFax(const MpFax faxMsg);

      //EnumVoiceProcessErr HandleRcvFax(const MpFax faxMsg);

      //---------------------------------------
      BOOLEAN CreateEventThread();

      //-------------------------------------------------------------------------
      MpRequestDetails     mMpReq;        
      MpRequestDetails     mMpAcuReq;        
      MpResponseDetails    mMpAcuRsp;   

      AcuMsgQueApi         mMsgQueApi;
      AcuModuleInfo        mAcuModuleInfo;
      AcuCardDetails       mAcuCardDetails;

      //----- Play & Rec Thread ---------------
      pthread_t            mEventThread;
      pthread_t            mEventMsgThread;

      TEXT                 mFileName[MAX_FILE_NAME_LEN + 1];
      BOOLEAN              mSendRspFlag;          
      INT32                mCompType;
      int                  mTingTrace;

      CardDetails          mCardDetails[ACULAB_MAX_NUM_CARDS];
      ModuleInfo           mModuleInfo[ACULAB_MAX_NO_MODULES];

      ACU_SNAPSHOT_PARMS            mSnapshotParms;
      ACU_OPEN_CARD_PARMS           mOpenCardParms;
      SM_OPEN_MODULE_PARMS          mOpenModuleParms;
};
#endif
