// @(#) $Id: SynwayVoiceMgrApi.h,v 1.1.4.3 2016/04/12 21:59:39 shijuc Exp $
//-----------------------------------------------------------------------------
// NAME           : SynwayVoiceMgrApi.h
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Chethan V M           Date:
//----------------------------------------------------------------------------

#ifndef INCL_TS_SYNWAY_API_LIB_H
#define INCL_TS_SYNWAY_API_LIB_H

#include "SynwayUtil.h"

#define SYNWAY_MEDIA_TIMEDOUT_BUFFER 20
#define SYNWAY_GETDIG_TIMEDOUT_BUFFER 20
#define SYNWAY_SIGNALLING_TIMEDOUT_BUFFER 20
#define SYNWAY_STOP_EVENT_TIMEDOUT 60

class SynwayVoiceMgrApi
{
	public :

		SynwayVoiceMgrApi();

		~SynwayVoiceMgrApi();

		BOOLEAN ReadConfig();

		BOOLEAN ReloadConfig();

		EnumVoiceProcessErr Init(INT16, INT16, INT16, TEXT *, TEXT *);

		BOOLEAN InitSynway (TEXT *, TEXT *);

		EnumVoiceProcessErr Deinit();

		EnumVoiceProcessErr Play(const MpPlay , TEXT *);

		EnumVoiceProcessErr Stop(const MpStop, TEXT *);

		BOOLEAN	StopPlaying(UINT16, unsigned int, BOOLEAN, TEXT *);

		BOOLEAN	StartPlay(MpPlay, TEXT *);

		EnumVoiceProcessErr AdjSpdVol(const MpSetVolSpd);

		INT16	  HandleEvent(MpResponseDetails &, MESSAGE_INFO&);

		BOOLEAN WaitForEvt(MESSAGE_INFO &);

		BOOLEAN SetEvents();

		INT16	HandleGetDtmfStopped(MESSAGE_INFO &, MpResponseDetails &);

		INT16	HandleRecordStopped(MESSAGE_INFO &, MpResponseDetails &);

		INT16	HandlePlayStopped(MESSAGE_INFO &, MpResponseDetails &);

		void ShowChnlStatus();

		BOOLEAN	RecToFile(MpRecord, TEXT *);

		EnumVoiceProcessErr  StartRecord(const MpRecord, TEXT *);

		BOOLEAN	StopRecToFile(UINT16, unsigned int, TEXT *);

		EnumVoiceProcessErr	StopRecord(const MpStop, TEXT *);

		EnumVoiceProcessErr GetDtmfDigits(const MpGetDigit, MpGetDigit &, TEXT *);

		BOOLEAN	CancelWaitDtmf(UINT16, unsigned int, TEXT *, TEXT *, BOOLEAN);

		EnumVoiceProcessErr CancelGetDtmfDigits(const MpStop, TEXT *, TEXT *);

		EnumVoiceProcessErr ConnectSpeech(const MpSwitch, TEXT *);

		EnumVoiceProcessErr DisconnectSpeech(const MpSwitch, TEXT *);

		BOOLEAN  WaitDtmf(MpGetDigit, TEXT *);

		INT16 CollectDtmfBuf(MpGetDigit, TEXT *, TEXT *);

		EnumChannelStatus GetMediaChannelStatus(INT16 , TEXT *);

		BOOLEAN HandleMediaEvent(MpResponseDetails &rsp, MESSAGE_INFO &lEvent);

		BOOLEAN HandleSigEvent(GenIsupMsg &lGenIsupMsg, MESSAGE_INFO &lEvent);

      BOOLEAN HandleChnlMgmtEvent(CktCmdRsp &lCktCmdRsp, MESSAGE_INFO &lEvent);

		INT16 SendIsupMsgToNw(GenIsupMsg &lGenIsupMsg);

      BOOLEAN SendChnlMgmtMsg(CktCmdReq &lCktCmdReq);

		INT16 SignalEvents(GenIsupMsg &lGenIsupMsg, MESSAGE_INFO &lEvent);

      INT16 ChnlMgmtEvents(CktCmdRsp &lCktCmdRsp, MESSAGE_INFO &lEvent);

		static TEXT* ConvertStatusToStr(EnumChannelStatus);

      void GetSignalingLinkStatus();

      static TEXT* ConvertLinkStateToStr(int linkState);

      EnumVoiceProcessErr Pause(MpPause lpause, TEXT *lType);
      EnumVoiceProcessErr Resume(MpResume lresume, TEXT *lType);
      EnumVoiceProcessErr Forward(MpForward lforward, TEXT *lType);
      EnumVoiceProcessErr Rewind(MpRewind lRewind, TEXT *lType);

      INT16 HandlePlayControls(MESSAGE_INFO &lEvent);

      INT16 DialNum(GenIsupMsg &lGenIsupMsg);

      void FillRelAndClearScheduled(int cic, GenIsupMsg &genIsupMsg, int cause);

      BOOLEAN CheckMediaChannelStatus(int chnl, MpResponseDetails &rsp);

      BOOLEAN CheckSignalStatus(int chnl, GenIsupMsg &lGenIsupMsg);
	protected :

      BOOLEAN                   InitChnlMgmt();
      BOOLEAN                   ChnlUnlock(int);
      BOOLEAN                   ChnlLock(int);

		//synwaySystem              mSynwaySystem;
		INT32         				  mPlayFileFormat_Cfg;
		INT32         				  mRecordFileFormat_Cfg;
		UINT32                    mTimeOutEvt_Cfg;
		UINT32                    mPlayFileTime_Cfg;
      TEXT                      mLinkStatusFile[SYNWAY_MAX_FILE_PATH_LEN+1];      

      key_t                     mChnlStShmKey;
      int                       mShmChnlMgmtId;

      key_t                     mChnlStSemKey;
      int                       mSemChnlMgmtId;

      INT32                     mFilePlayPrfChkIntr;
      INT32                     mFileRecordPrfChkIntr;

      INT32                     mMaxCallDur;
      synwaySystem              *mpSynwaySystem;


};

#endif
