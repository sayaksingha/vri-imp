// @(#) $Id: Billing.h,v 1.1.4.3 2016/04/12 21:58:59 shijuc Exp $
//----------------------------------------------------------------------------
// NAME 
// Billing.h 
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright (C) 2006, 
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd.
//
// CLASS TYPE
//
// DESCRIPTION
// This is a API(library class) for applications to invoke real-time billing
// (IN-Interactin) with subscribers account.
//
// Originated: KOTHARI KOMAL                                       16-JAN-2012
//----------------------------------------------------------------------------
#ifndef INCL_MMP_BILLING_H
#define INCL_MMP_BILLING_H

#include "BillingUtil.h"

class Billing
{
   public:
      Billing();

      ~Billing();

      BOOLEAN Init(UINT8 appId=0);

      BOOLEAN ReadBillingRequest(BillingRequest & billingRequest);
      // Process for Application to send request and receive response
      BOOLEAN ProcessBillingRequest(BillingRequest & billingRequest,
           BillingResponse & billingResponse);

      const char* GetVersion();
   private:
      msgQueueError CreateMsgQueue(MsgQueue & queue, const key_t key);

      BOOLEAN HandleQueueReadError(msgQueueError err, MsgQueue & queue);

      // Read responce and give it to common process for application
      BOOLEAN ReceiveBillingResponse(BillingResponse & mAppRsp, int msgType);

      // Reads request from commonn process and inserts for charging gateway
      BOOLEAN SubmitBillingRequest(BillingRequest & mAppReq);
      
   protected:
      BOOLEAN ReadConfig();

      BOOLEAN CreateIPC();

      MsgQueue mBillingReqQue;
      key_t mBillingReqQKey;

      MsgQueue mBillingRspQue;
      key_t mBillingRspQKey;

      UINT8 mAppId;
};

#endif
