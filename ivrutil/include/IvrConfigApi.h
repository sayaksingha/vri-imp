// @(#) $Id: IvrConfigApi.h,v 1.1.4.3 2016/04/12 21:59:16 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// IvrConfigApi.h
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
// Originated : KOTHARI KOMAL                                17-JAN-2012
//----------------------------------------------------------------------
#ifndef INCL_IVR_CFG_DEF_H
#define INCL_IVR_CFG_DEF_H

#include "IvrUtil.h"
#include "IvrDbManager.h"
#include "IvrStructs.h"

enum EnumIvrConfigType
{
   CONFIG_IVR_IPC    = 0x01,
   CONFIG_IVR_KERNEL = 0x02,
   CONFIG_IVR_MMP    = 0x04,
   CONFIG_IVR_BILLING= 0x08,
   CONFIG_IVR_ALL    = 0xFF
};

class IvrConfigApi
{
   public:
      IvrConfigApi();

      IvrConfigApi(EnumIvrConfigType configType);

      ~IvrConfigApi();

      const char* GetVersion();

      BOOLEAN Init(EnumIvrConfigType cfgType = CONFIG_IVR_ALL);

      BOOLEAN Init(IvrDbManager &dbObj, EnumIvrConfigType cfgType = CONFIG_IVR_ALL);

      BOOLEAN ReloadConfig();

      BOOLEAN ReloadConfig(IvrDbManager &dbObj);

      //Message Queue Keys
      key_t mCcxmlReqQueKey;
      key_t mCcxmlRspQueKey;
      key_t mVxmlReqQueKey;
      key_t mVxmlChildReqQueKey;
      key_t mVxmlRspQueKey;
      key_t mCallAlertQueKey;
      key_t mEventChildQueKey;
      key_t mCcxmlEventQueKey;
      key_t mIsupEncQueKey;
      key_t mExtVxmlRspQueKey;
      key_t mChannelStatusQueKey;
      key_t mIvrPegQueKey;

      //Session Manager Configuration
      INT16 mRingDuration;
      INT16 mScheduleReTryTime;
      INT16 mScheduleReTryAttempts;

      // Billing Server Configuration
      INT32 mSemBillingRecordKey;
      INT32 mShmBillingRecordKey;
      INT32 mChargeFunction;
      BOOLEAN mIsMmpEnabled;
      BOOLEAN mAllLogsRequired;

      int                mDbClient;
      TEXT               mDbName[IVR_MAX_DB_NAME_LEN];                  // Database Name
      TEXT               mDbUser[MAX_DB_USER];                  // User Name
      TEXT               mDbPass[MAX_DB_PASS];                  // Password

   protected:
      BOOLEAN ReadConfig(BOOLEAN initial=true);

      BOOLEAN ReadConfigFromDb(IvrDbManager &dbObj, BOOLEAN initial=true);

      BOOLEAN mIsInitFile;
      BOOLEAN mIsInitDb;

   private:
      EnumIvrConfigType mConfigType;
};

#endif

