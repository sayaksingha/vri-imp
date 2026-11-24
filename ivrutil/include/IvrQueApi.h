// @(#) $Id: IvrQueApi.h,v 1.1.1.1.2.5.2.1 2016/07/05 00:07:52 mram Exp $
//-----------------------------------------------------------------------
// NAME
// IvrQueApi.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2010
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated:   													DATE:25-OCT-2010 
//-----------------------------------------------------------------------
#ifndef INCL_IVR_QUE_API_DEF_H
#define INCL_IVR_QUE_API_DEF_H

#include "IvrUtil.h"
#include "MediaInterface.h"
#include "MediaUtil.h"
#include "BillingApi.h"

class IvrQueApi
{
   public:
      IvrQueApi ();

      ~IvrQueApi ();

      BOOLEAN Init ();

      BOOLEAN ReloadConfig ();

      BOOLEAN ReadFromCcxmlReqQue (CcxmlRequest &, INT16 msgType = 0);

		BOOLEAN ReadFromCcxmlRspQue (CcxmlResponse &, INT16 msgType = 0);
      
      BOOLEAN ClearVxmlReqQue (INT16 msgType = 0);
      
      BOOLEAN ReadFromVxmlReqQue (VxmlRequest &,  INT16 msgType = 0);
      
      BOOLEAN ReadFromVxmlChildReqQue (VxmlRequest &,  INT16 msgType = 0);
      
		BOOLEAN ReadFromVxmlRspQue (VxmlResponse &,  INT16 msgType = 0);

		BOOLEAN ReadFromCallAlertQue (KernelMsg & , INT16 msgType = 0);
		
		BOOLEAN ReadFromEventChildQue (KernelMsg & , INT16 msgType = 0);
		
		BOOLEAN ClearEventChildQue (INT16 msgType = 0);

      BOOLEAN WriteToCcxmlReqQue (CcxmlRequest & );

      BOOLEAN WriteToCcxmlRspQue (CcxmlResponse &, INT16 msgType = 1);
      //BOOLEAN WriteToCcxmlRspQue (CcxmlResponse &);

      BOOLEAN WriteToVxmlReqQue (VxmlRequest &);
      
		BOOLEAN WriteToVxmlChildReqQue (VxmlRequest &);

      BOOLEAN WriteToVxmlRspQue (VxmlResponse &, INT16 msgType = 1);

      BOOLEAN WriteToExtVxmlRspQue (ExtResponse &, INT16 msgType = 1);

		BOOLEAN WriteToEventChildQue (VxmlResponse & lVxmlRsp, INT16 msgType);

      BOOLEAN WriteToEventChildQue (KernelMsg &, UINT16 lMsgType);
		
		BOOLEAN WriteToEventChildQue (CcxmlResponse & lCcxmlRsp, INT16 msgType);
      
		BOOLEAN WriteToIsupEncQue (GenIsupMsg &);

		BOOLEAN WriteToCcxmlEventQue (CcxmlRequest & lCcxmlReq);

		BOOLEAN ReadFromCcxmlEventQue (CcxmlRequest & lCcxmlReq, INT16 msgType);

		BOOLEAN WriteToCallAlertQue (KernelMsg & lKernelMsg);

		BOOLEAN WriteToChannelStatusQueue(ShmIvrChannelDetails &channelDetail, LONG msgType = 1);

		BOOLEAN ReadFromChannelStatusQueue(ShmIvrChannelDetails &channelDetail, LONG msgType = 0);

//      BOOLEAN WriteToIvrPegQueue(INT16 EventCode, INT16 Action = INCREMENT, UINT32 Count=0);

      BOOLEAN WriteToIvrPegQueue(INT16 EventCode, INT16 Action = 1, UINT32 Count=0);

      BOOLEAN ReadFromIvrPegQueue(IvrPeg &ivrPeg, LONG msgType = 0);

      BOOLEAN ReadFromSchRspQ(ScheduleRsp& lScheduleRsp);

      BOOLEAN WriteToSchRspQ(ScheduleRsp& lScheduleRsp);

      BOOLEAN WriteToCktCmd(CktCmdReq&);

      BOOLEAN ReadCktCmdRsp(CktCmdRsp&);

      BOOLEAN    ReadFromBillingRequestQue (BillingRequest & billingRequest, INT16 msgType=0);

      BOOLEAN    WriteToBillingRequestQue (BillingRequest & billingRequest);

      BOOLEAN    ReadFromBillingResponseQue (BillingResponse & rBillingResponse, int msgType=0);

      BOOLEAN    WriteToBillingResponseQue (BillingResponse & rBillingResponse);

      BOOLEAN    ReadFromReqQue(MpRequestDetails & lMpReqDetails, INT16 lMsgType);

      BOOLEAN    WriteToRspQue (MpResponseDetails & lMpRspDetails, INT16 lMsgType);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateQueues ();

      key_t mCcxmlReqQueKey;
      MsgQueue mCcxmlReqQue;

      key_t mCcxmlRspQueKey;
      MsgQueue mCcxmlRspQue;

      key_t mVxmlReqQueKey;
      MsgQueue mVxmlReqQue;

      key_t mVxmlChildReqQueKey;
      MsgQueue mVxmlChildReqQue;

      key_t mVxmlRspQueKey;
      MsgQueue mVxmlRspQue;
      
		key_t mCallAlertQueKey;
      MsgQueue mCallAlertQue;

		key_t mEventChildQueKey;
      MsgQueue mEventChildQue;

		key_t mCcxmlEventQueKey;
      MsgQueue mCcxmlEventQue;

		key_t mIsupEncQueKey;
      MsgQueue mIsupEncQue;

		key_t mExtVxmlRspQueKey;
      MsgQueue mExtVxmlRspQue;

		key_t    mChannelStatusQueKey;
      MsgQueue mChannelStatusQue;

		key_t    mIvrPegQueKey;
      MsgQueue mIvrPegQue;

		key_t    mScheduleRspQueKey;
      MsgQueue mScheduleRspQue;

		key_t    mCktCmdReqQueKey;
      MsgQueue mCktCmdReqQue;
		
		key_t    mCktCmdRspQueKey;
      MsgQueue mCktCmdRspQue;

      key_t      mBillingReqQKey;
      MsgQueue   mBillingReqQue;

      key_t      mBillingRspQKey;
      MsgQueue   mBillingRspQue;

       key_t      mMpRspQueKey;
       MsgQueue   mMpRspQue;

       key_t      mMpReqQueKey;
       MsgQueue   mMpReqQue;

       int        mPegRequired;
};

#endif
