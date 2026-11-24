// @(#) $Id: SynwayMPManager.h,v 1.1.4.3 2016/04/12 21:59:39 shijuc Exp $
//-----------------------------------------------------------------------------
// NAME           : SynwayMPManager.h
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Chethan V M           Date:
//----------------------------------------------------------------------------

#ifndef INCL_SYNWAY_VOICE_PROC_H
#define INCL_SYNWAY_VOICE_PROC_H

#include "MediaInterface.h"
#include "SynwayVoiceMgrApi.h"

#define  INV_CHANNEL_NUMBER   -1
#define  MAX_CHANNEL_PER_PRS   10000

class SynwayMPManager
{
   public :

   SynwayMPManager();

   ~SynwayMPManager();

   BOOLEAN Init();
	
   BOOLEAN DeInit();

   BOOLEAN ReloadConfig();

   BOOLEAN ReadReq();

   BOOLEAN ProcessReq();

   BOOLEAN WriteRsp(MpResponseDetails &, TEXT *lType =(TEXT*) "MAIN");

   BOOLEAN ReadEventRsp(MESSAGE_INFO &lEvent);

   BOOLEAN ProcessEventRsp(SynwayMPManager * lpTsPikaMPManager, MESSAGE_INFO &);

	BOOLEAN ProcessSigEventRsp(SynwayMPManager * lpSynwayMPManager, MESSAGE_INFO &);

   BOOLEAN ProcessChnlMgmtrsp(SynwayMPManager * lpSynwayMPManager, MESSAGE_INFO &);

	BOOLEAN IssuePlayStop();
	BOOLEAN IssueGetDigitStop();

	BOOLEAN ReadIsupMsg(GenIsupMsg &lGenIsupMsg);

   BOOLEAN ReadChnlMgmtMsg(CktCmdReq &lCktCmdReq);

	BOOLEAN WriteIsupMsg(GenIsupMsg &lGenIsupMsg, TEXT *lType =(TEXT*) "MAIN");

   BOOLEAN WriteChnlMgmtMsg(CktCmdRsp &lCktCmdRsp, TEXT *lType =(TEXT*) "MAIN");

	BOOLEAN ProcessIsupMsg(GenIsupMsg &lGenIsupMsg);

   BOOLEAN ProcessChnlMgmtMsg(CktCmdReq &lCktCmdReq); 

	BOOLEAN CreateSignalingThread();

   BOOLEAN CreateChnlMgmtThread();

   BOOLEAN CreateChnlTimerThread();

   BOOLEAN CheckChnlStatus();

	INT32	  mBoardId;
	TEXT	  mInPutShCfgFile[100];
	TEXT	  mInPutShIdxFile[100];

	private:
	INT32	  mMinChannel;
	INT32	  mMaxChannel;

	TEXT* ConvtCmdToStr(EnumMediaCmdTypes lCmd, unsigned int &lseq, UINT16 &lCkt, MpData&);

	long SetChannelId(EnumMediaCmdTypes lCmd, MpData &lMpData);

	long GetChannelId(EnumMediaCmdTypes lCmd, MpData &lMpData);


   UINT8                 mTrunkId;
   MpResponseDetails     mMpRsp;        // Response struture to be filled by Library
   MpRequestDetails      mMpReq;         // Request structure to pe passed to Library
   pthread_t             mEventMsgThread;
   pthread_t             mSignalingMsgThread;
   pthread_t             mChnlMgmtMsgThread;
   pthread_t             mChnlTimerMsgThread;

   BOOLEAN ReadConfig();
   
   BOOLEAN CreateQueues();

   BOOLEAN CreateReqThreads(UINT8 count);

   BOOLEAN CreateEventThread();

   BOOLEAN InitChannel();

	INT16						mRunSigThread;
   INT16                mRunChnlMgmtThread;

   MsgQueue             mReqMsgQue;
   INT32                mReqMsgQKey;

	MsgQueue					mIsupMsgReqQ;
	MsgQueue					mIsupMsgRspQ;

   MsgQueue             mChnlMgmtMsgReqQ;
   MsgQueue             mChnlMgmtMsgRspQ;

	INT32						mIsupMsgRspKey;
	INT32						mIsupMsgReqKey;

   INT32                mChnlMgmtMsgReqKey;
   INT32                mChnlMgmtRspKey;
   
   MsgQueue             mRspMsgQue;
   INT32                mRspMsgQKey;

   UINT32               mEventThread;
   UINT32               mSignalingThread;
   UINT32               mChnlMgmtThread;
   UINT32               mChnlTimerThread;

   BOOLEAN              mSendRspFlag;              // Flag to send back response to listener
};
#endif
