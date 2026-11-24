// @(#) $Id: MmpQueApi.h,v 1.1.4.3 2016/04/12 21:59:45 shijuc Exp $
//-----------------------------------------------------------------------
// NAME
// MmpQueApi.h
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
#ifndef INCL_MMP_QUE_API_DEF_H
#define INCL_MMP_QUE_API_DEF_H

#include "MmpUtil.h"

class MmpQueApi
{
   public:
      MmpQueApi ();

      ~MmpQueApi ();

      BOOLEAN    Init ();

      BOOLEAN    ReloadConfig ();

      BOOLEAN    WriteToGwTxQue (AseRequest& lAseRequest );

      BOOLEAN    ReadGwTxRsp (AseResponse& lAseResponse, INT16 msgType);
      
      BOOLEAN    ReadFromProvHdlrQue (GgiReqMsg & reqMsg, INT16 msgType);

      BOOLEAN    WriteToSubsProvReqQue (MmpSubsProvReq& lMmpSubsProvReq );

      BOOLEAN    ReadSubsProvRspQue (MmpSubsProvRsp& lMmpSubsProvRsp, INT16 lMsgType);

      BOOLEAN    WriteToProvHdlrQue (GgiReqMsg & lProvReq);

      BOOLEAN    ReadFromBillingRequestQue (BillingRequest & billingRequest, INT16 msgType=0);

      BOOLEAN    WriteToBillingRequestQue (BillingRequest & billingRequest);

      BOOLEAN    ReadFromBillingResponseQue (BillingResponse & rBillingResponse, int msgType=0);

      BOOLEAN    WriteToBillingResponseQue (BillingResponse & rBillingResponse);

	private:
      BOOLEAN    ReadConfig ();

      BOOLEAN    CreateQueues ();

      key_t      mGwTxQueKey;
      MsgQueue   mGwTxQue;
		
      key_t      mGwTxRspQueKey;
      MsgQueue   mGwTxRspQue;
		
      key_t      mGwRxQueKey;
      MsgQueue   mGwRxQue;

      key_t      mProvHdlrQueKey;
      MsgQueue   mProvHdlrQue;

      key_t      mSubsProvReqQueKey;
      key_t      mSubsProvRspQueKey;

      MsgQueue   mSubsProvReqQue;
      MsgQueue   mSubsProvRspQue;

      key_t      mBillingReqQKey;
      MsgQueue   mBillingReqQue;

      key_t      mBillingRspQKey;
      MsgQueue   mBillingRspQue;
};

#endif
