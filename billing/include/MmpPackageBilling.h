// @(#) $Id: MmpPackageBilling.h,v 1.1.4.3 2016/04/12 21:59:00 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// MmpPackageBilling.h
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated : KOTHARI KOMAL                                12-APR-2012
//----------------------------------------------------------------------
#ifndef INCL_MMP_PACKAGE_BILLING_DEF_H
#define INCL_MMP_PACKAGE_BILLING_DEF_H

#include "BillingUtil.h"
#include "MmpDbManager.h"
#include "MultiLevelHashTimer.h"
#include "BillingShmManager.h"
#include "BillingSem.h"
#include "MmpConfigApi.h"
#include "IvrQueApi.h"
/*
enum EnumPackageType
{
   PACKAGE_TYPE_UNKNOWN      = 0,
   PACKAGE_TYPE_MINUTE       = 1,
   PACKAGE_TYPE_CONTENT      = 2,
   PACKAGE_TYPE_DEDICATION   = 4,
   PACKAGE_TYPE_SUBSCRIPTION = 8
};

struct BillingLimits
{
   UINT32            subscriptionId;
   UINT32            packageId;
   EnumPackageType   packageType;
   UINT32            usage;
};

struct SubsPackageInfo
{
   EnumPackageType   packageType;
   TEXT              msisdn[MAX_MSISDN_LEN+1];
   UINT32            subscriptionId;
   UINT32            packageId;
   UINT32            min;
   UINT32            content;
   UINT32            dedication;
   UINT32            subscription;
   UINT8             status;
   tm                startDate;
   tm                endDate;
   tm                startTime;
   tm                endTime;
};
*/
class MmpPackageBilling
{
   public:
   MmpPackageBilling();

   ~MmpPackageBilling();

   const char*    GetVersion();

   BOOLEAN        ResetBillingRecord(INT32 channelId, BillingRecordDetails &);

   BOOLEAN        GetBillingRecord(INT32 channelId, BillingRecordDetails &);

   BOOLEAN        SetBillingRecord(INT32, BillingRecordDetails&);

   BillingLimits  GetSubsPackage(TEXT* subsNo, EnumMmpPkgType packageType, SubsPackageInfo &, UINT32 usage = 0);

   BOOLEAN        ChargeFromPackage(UINT32 subscriptionId, EnumMmpPkgType , UINT32 usage, SubsPackageInfo &);

   BOOLEAN        ProcessBillingEvent(BillingRecordDetails record, BillingResponse &response);

   BOOLEAN        ProcessBillingEvent(UINT32 channelId, EnumBillingEvents event, BillingResponse &response);
   
   BOOLEAN        Init ();

   BOOLEAN        ProcessTimerExpiry(UINT32 channelId, EnumBillingEvents event);
   
   private:
   BOOLEAN        StartNomTimer(BillingRecordDetails &record);

   BOOLEAN        StopNomTimer(BillingRecordDetails &record);

   BOOLEAN        GenerateCdr(BillingRecordDetails &record);

   BOOLEAN        SendBillingResponse(BillingResponse &respone);
   
   EnumMmpPkgType GetPackageType(EnumBillingEvents event);
   
   BillingLimits  GetSubsPackageForContent(BillingRecordDetails &record);
   
   BOOLEAN        ProcessCallReceived(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessCallAnswered(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessCallDisconnected(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessContentPlay(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessContentStart(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessContentStop(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessOfflineDedication(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessOffline(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        ProcessUnknownEvent(BillingRecordDetails &record, BillingResponse &response);
   
   BOOLEAN        SendReleaseCallSignal(INT32 channelId);

   private:
   BillingSem           mSemObj;
   MmpConfigApi         mCfgObj;

   IvrQueApi            mQueApi;
   BillingShmManager <BillingRecordDetails> mBillingRecordShm;
   BillingRecordDetails billingRecord;
   SubsPackageInfo      subsPackage;
   MmpDbManager         mDbManager;
   MLHTimer            *mTimer;
};
   
#endif   
