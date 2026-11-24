// @(#) $Id: MmpConfigApi.h,v 1.1.4.3 2016/04/12 21:59:45 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// MmpConfigApi.h
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
// Originated : Divya K                                31-JAN-2012
//----------------------------------------------------------------------
#ifndef INCL_MMP_CFG_DEF_H
#define INCL_MMP_CFG_DEF_H

#include "MmpUtil.h"
#include "MmpDbManager.h"
#include "MmpStructs.h"
#include "BillingStructs.h"

enum EnumMmpConfigType
{
   CONFIG_MMP_IPC      = 0x01,
   CONFIG_MMP_KERNEL   = 0x02,
   CONFIG_MMP_CHARGING = 0x04,
   CONFIG_MMP_SYSTEM   = 0x08,
   CONFIG_ALL          = 0xFF
};

class MmpConfigApi
{
   public:
      MmpConfigApi();

      ~MmpConfigApi();

      const char* GetVersion();

      //BOOLEAN Init(EnumMmpConfigType cfgType);
      //BOOLEAN Init(MmpDbManager &dbObj, EnumMmpConfigType cfgType);
      BOOLEAN Init(UINT32 cfgType=CONFIG_ALL);

      BOOLEAN Init(MmpDbManager &dbObj, UINT32 cfgType=CONFIG_ALL);

      BOOLEAN ReloadConfig();

      BOOLEAN ReloadConfig(MmpDbManager &dbObj);

   protected:
      BOOLEAN ReadConfig(BOOLEAN initial=true);

      BOOLEAN ReadConfigFromDb(MmpDbManager &dbObj, BOOLEAN initial=true);

   /*   BOOLEAN mIsInitFile;
      BOOLEAN mIsInitDb;*/

   private:
      UINT32            mConfigType;
      //EnumMmpConfigType mConfigType;

   public:
      INT32              mSysDedicationStartTime;
      INT32              mSysDedicationEndTime;
      INT32              mSysDedicationDelAttempts;
      TEXT               mDedicationAppl[SS7_MAX_ADDRESS_LEN + 1];
      INT32              mDailyBillFlg;
      INT32              mGetImsiFlg;
      TEXT               mOpSenderId[SS7_MAX_ADDRESS_LEN + 1];

      INT32              mShmDedicateKey;
      UINT32             mSessionId;
//      INT32              mSysNotiVP;
      INT32              mSysOpSmsRsp;

      INT32              mSemBillingRecordKey;
      INT32              mShmBillingRecordKey;
      INT32              mNdcSnLength;
      INT32              mCgwErrLmt;
      INT32              mSendUnreg;
      INT32              mInRetryAttempts;
      TEXT               mHomeCc[MMP_MAX_CC_LEN + 1];

      BOOLEAN            mAllLogsRequired;
      EnumChargeFunction mChargeFunction;
      BOOLEAN            mIsMmpEnabled;

      int                mDbClient;
      TEXT               mDbName[IVR_MAX_DB_NAME_LEN];                  // Database Name
      TEXT               mDbUser[MAX_DB_USER];                  // User Name
      TEXT               mDbPass[MAX_DB_PASS];                  // Password
};

#endif

