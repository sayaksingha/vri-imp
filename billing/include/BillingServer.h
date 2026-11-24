// @(#) $Id: BillingServer.h,v 1.1.4.3 2016/04/12 21:59:00 shijuc Exp $
//----------------------------------------------------------------------------
// NAME
// BillingServer.h
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
#include "MmpPackageBilling.h"
#include "IvrQueApi.h"
#include "IvrConfigApi.h"
#include "BillingShmManager.h"
#include "BillingSem.h"

class BillingServer
{
   public:
      BillingServer();

      ~BillingServer();

      BOOLEAN Init();

      BOOLEAN ReadBillingRequest(BillingRequest & billingRequest);

      // Process for Application to send request and receive response
      BOOLEAN ProcessBillingRequest(BillingRequest & billingRequest);

      const char* GetVersion();

      BOOLEAN ReloadConfig();
   private:

      BOOLEAN GetBillingRecord(BillingRequest req, BillingRecordDetails &record);

      BOOLEAN StoreBillingRecord(BillingRecordDetails &record, INT32 index);

      BOOLEAN ResetBillingRecord(INT32 index, BillingRecordDetails &record);

      BOOLEAN GetBillingRecord(INT32 index, BillingRecordDetails &record);

      BOOLEAN ReceiveBillingResponse (BillingResponse & rBillingResponse, int msgType);

      // Read responce and give it to common process for application
      BOOLEAN SendBillingResponse(BillingResponse & billingResponse);

      // Reads request from commonn process and inserts for charging gateway
      BOOLEAN SubmitBillingRequest(BillingRequest & billingRequest);

      BOOLEAN IsChargeFunctionEnabled(BillingRecordDetails record);

      EnumChargeFunction GetChargeFunction(BillingRequest req);

      EnumChargeMode GetChargeMode(BillingRequest req);

      BOOLEAN DumpToDiscrepencyFile(BillingRecordDetails &record);

   protected:
      BOOLEAN ReadConfig();

      BOOLEAN CreateIPC();

      MsgQueue mBillingReqQue;
      key_t mBillingReqQKey;

      MsgQueue mBillingRspQue;
      key_t mBillingRspQKey;

      UINT8 mAppId;

      IvrQueApi    mQueApi;

      IvrConfigApi mCfgObj;

      BillingSem   mSemObj;

      BillingShmManager <BillingRecordDetails> mBillingRecordShm;

      // BillingRecordDetails *mpBillingRecord;

      // BillingRecordDetails  mRecord;

      MmpPackageBilling     mPackageBilling;

      IvrDbManager          mDbManager;
};

#endif

