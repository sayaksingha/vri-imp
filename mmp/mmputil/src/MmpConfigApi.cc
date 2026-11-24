static const char *id = "@(#) $Id: MmpConfigApi.cc,v 1.1.4.3 2016/04/12 21:59:45 shijuc Exp $";
//----------------------------------------------------------------------
// NAME : MmpConfigApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2008
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION:
//              This libreary used for reading all configurations.
// message.

// Originated: Divya K                           Date: 31-JAN-2012
//----------------------------------------------------------------------
#include "MmpConfigApi.h"

//----------------------------------------------------------------------
// METHOD      : GetVersion
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None 
//----------------------------------------------------------------------
const char* MmpConfigApi::GetVersion()
{
   return id;
}

//----------------------------------------------------------------------
// METHOD      : MmpConfigApi
// DESCRIPTION : Constructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
MmpConfigApi::MmpConfigApi ()
{
   mConfigType = CONFIG_ALL;
}

//----------------------------------------------------------------------
// METHOD      : ~MmpConfigApi
// DESCRIPTION : Destructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
MmpConfigApi::~MmpConfigApi ()
{
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
//BOOLEAN MmpConfigApi::Init (EnumMmpConfigType cfgType=CONFIG_ALL)
BOOLEAN MmpConfigApi::Init (UINT32 cfgType)
{
   mConfigType = cfgType;
   mIsMmpEnabled = false;
   if(false == this->ReadConfig ())
   {
      return false;
   }

   return true;
}


//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
//BOOLEAN MmpConfigApi::Init (MmpDbManager & dbObj, EnumMmpConfigType cfgType)
BOOLEAN MmpConfigApi::Init (MmpDbManager & dbObj, UINT32 cfgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   mConfigType = cfgType;
   mIsMmpEnabled = false;

   if(false == this->ReadConfig ())
      return false;

   if(DB_NO_ERR != dbObj.Init(mDbClient))  // 1 - MySql DB Client , 0 - Oracle DB Client
   {
      sprintf(lLogText, "DataBase Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == this->ReadConfigFromDb (dbObj))
      return false;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpConfigApi::ReadConfig(BOOLEAN initial)
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (lCfgRead.CfgInit (MMP_KERNEL_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, MMP_KERNEL_CFG););
      gLog.GenerateLog (GSYS09,(TEXT*) MMP_KERNEL_CFG);
      return false;
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MMP_DB_CLIENT", mDbClient,
            0, 1))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MMP_DB_CLIENT", MMP_KERNEL_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MMP_DB_CLIENT = %d\n",
               gProcessName, mDbClient););
   }

   if( CONFIG_MMP_IPC & mConfigType)
   {
      if (lCfgRead.CfgInit (MMP_IPC_CFG) != true)
      {
         TERR (gTrace, printf ("%s: Could not open file %s\n",
                  gProcessName, MMP_IPC_CFG););
         gLog.GenerateLog (GSYS09,(TEXT*) MMP_IPC_CFG);
         return false;
      }

      if (CFG_OK !=
            lCfgRead.GetConfigNum ("SHM_DEDICATION_KEY", mShmDedicateKey,
               MIN_SHM_KEY, MAX_SHM_KEY))
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Config read failed for %s. File:%s.",
               "SHM_DEDICATION_KEY", MMP_IPC_CFG);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         lCfgRead.CfgDeInit ();
         return false;
      }
      else
      {
         T (gTrace, printf ("%s: SHM_DEDICATION_KEY = %d\n",
                  gProcessName, mShmDedicateKey););
      }


      if (CFG_OK !=
            lCfgRead.GetConfigNum ("SHM_BILLING_RECORD_KEY", mShmBillingRecordKey,
               MIN_SHM_KEY, MAX_SHM_KEY))
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Config read failed for %s. File:%s.",
               "SHM_BILLING_RECORD_KEY", MMP_IPC_CFG);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         lCfgRead.CfgDeInit ();
         return false;
      }
      else
      {
         T (gTrace, printf ("%s: SHM_BILLING_RECORD_KEY = %d\n",
                  gProcessName, mShmBillingRecordKey););
      }

      if (CFG_OK !=
            lCfgRead.GetConfigNum ("SEM_BILLING_RECORD_KEY", mSemBillingRecordKey,
               MIN_SEM_KEY, MAX_SEM_KEY))
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Config read failed for %s. File:%s.",
               "SEM_BILLING_RECORD_KEY", MMP_IPC_CFG);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         lCfgRead.CfgDeInit ();
         return false;
      }
      else
      {
         T (gTrace, printf ("%s: SEM_BILLING_RECORD_KEY = %d\n",
                  gProcessName, mSemBillingRecordKey););
      }

      lCfgRead.CfgDeInit ();
   }

   return true;
}

////////////////////////////////////////////////////////////////////////
//---------------------- Read Config From Database -------------------//
////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------
// METHOD NAME : ReadConfigFromDb
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpConfigApi::ReadConfigFromDb (MmpDbManager & dbObj, BOOLEAN initial)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT            lKey[50] = "";
   DbErrorCodes    lDbErr;

   INT32 lKeyVal=0;

   // Commom supporting configuration items for all modules

   // 1. All Logs Required
   strcpy(lKey, "SYS_ALL_LOGS_REQUIRED");
   lDbErr = dbObj.ReadCfgNum(lKey, lKeyVal, 0, 1);

   if(lDbErr != DB_NO_ERR)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Db Error in reading configuration SYS_ALL_LOGS_REQUIRED Err:%d",
            lDbErr);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      return false;
   }
   if(lKeyVal)
      mAllLogsRequired = true;

   T(gTrace, printf("%s: SYS_ALL_LOGS_REQUIRED = %d\n",gProcessName, mAllLogsRequired););

   // 2. MMP License enabled or not
   strcpy(lKey, "KER_MMP_LICENSE");
   lDbErr = dbObj.ReadCfgNum(lKey, lKeyVal, 0, 1);

   if(lDbErr != DB_NO_ERR)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Db Error in reading configuration KER_MMP_LICENSE Err:%d",
            lDbErr);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      return false;
   }
   if(lKeyVal)
      mIsMmpEnabled = true;

   T(gTrace, printf("%s: KER_MMP_LICENSE = %d\n",
            gProcessName, mIsMmpEnabled););

   // MMP Kernel Configuration parameters
   if(mConfigType & CONFIG_MMP_KERNEL)
   {
      strcpy(lKey, "SYS_MMP_DEDICATION_APPL_CODE");

      lDbErr = dbObj.ReadCfgStr(lKey, mDedicationAppl, 4,20);
      
      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_DEDICATION_APPL_CODE Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_DEDICATION_APPL_CODE = %s\n",
               gProcessName, mDedicationAppl););

      strcpy(lKey, "SYS_MMP_DEDICATION_START_TIME");
      lDbErr = dbObj.ReadCfgNum(lKey, mSysDedicationStartTime, 0,23);
      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_DEDICATION_START_TIME Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_DEDICATION_START_TIME = %d\n",
               gProcessName, mSysDedicationStartTime););

      strcpy(lKey, "SYS_MMP_DEDICATION_END_TIME");
      lDbErr = dbObj.ReadCfgNum(lKey, mSysDedicationEndTime, 0,23);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_DEDICATION_END_TIME Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_DEDICATION_END_TIME = %d\n",
               gProcessName, mSysDedicationEndTime););

      if(mSysDedicationStartTime == mSysDedicationEndTime)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "SYS_MMP_DEDICATION_START_TIME and SYS_MMP_DEDICATION_END_TIME cannot be same");
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }

      strcpy(lKey, "SYS_MMP_DEDICATION_DEL_ATTEMPTS");
      lDbErr = dbObj.ReadCfgNum(lKey, mSysDedicationDelAttempts, 1,5);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_DEDICATION_DEL_ATTEMPTS Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_DEDICATION_DEL_ATTEMPTS = %d\n",
               gProcessName, mSysDedicationDelAttempts););
    
/*      strcpy(lKey, "SYS_NOTI_VALIDITY_PERIOD");
      lDbErr = dbObj.ReadCfgNum(lKey, mSysNotiVP, 0, 172800);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "Db Error in reading configuration SYS_NOTI_VALIDITY_PERIOD Err:%d",
                  lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_NOTI_VALIDITY_PERIOD = %ld\n",
               gProcessName, mSysNotiVP););
*/      
      strcpy(lKey, "SYS_MMP_OP_SMS_RSP");
      lDbErr = dbObj.ReadCfgNum(lKey, mSysOpSmsRsp, 0, 1);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "Db Error in reading configuration SYS_MMP_OP_SMS_RSP Err:%d",
                  lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_OP_SMS_RSP = %d\n",
               gProcessName, mSysOpSmsRsp););

   
      strcpy(lKey, "SYS_MMP_DAILY_BILL_FLAG");
      lDbErr = dbObj.ReadCfgNum(lKey, mDailyBillFlg, 0, 1);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_DAILY_BILL_FLAG, Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_DAILY_BILL_FLAG = %d\n",
               gProcessName, mDailyBillFlg););

      strcpy(lKey, "SYS_MMP_OP_SENDER_ID");
      lDbErr = dbObj.ReadCfgStr(lKey, mOpSenderId, 1, SS7_MAX_ADDRESS_LEN);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_OP_SENDER_ID, Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_OP_SENDER_ID = %s\n",
               gProcessName, mOpSenderId););
   }

   // MMP Charging Configuration parameters
   if(mConfigType & CONFIG_MMP_CHARGING)
   {
      INT32 lKeyVal=0;

      strcpy(lKey, "SYS_CHARGING_FUNCTION_TYPE");
      lDbErr = dbObj.ReadCfgNum(lKey, lKeyVal, (INT32)CHARGE_FUNCTION_NONE, (INT32)CHARGE_FUNCTION_MAX);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "Db Error in reading configuration SYS_CHARGING_FUNCTION_TYPE Err:%d",
                  lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      mChargeFunction = (EnumChargeFunction)lKeyVal;
      T(gTrace, printf("%s: SYS_CHARGING_FUNCTION_TYPE = %d\n",
               gProcessName, mChargeFunction););
      
   }

   if(mConfigType & CONFIG_MMP_SYSTEM)
   {
      strcpy(lKey, "SYS_IMSI_FOR_CHARGE");
      lDbErr = dbObj.ReadCfgNum(lKey, mGetImsiFlg, 0, 1);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_IMSI_FOR_CHARGE Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_IMSI_FOR_CHARGE = %d\n",
               gProcessName, mGetImsiFlg););

      strcpy(lKey, "SYS_HOME_OP_CC");
      lDbErr = dbObj.ReadCfgStr(lKey, mHomeCc, 1, 6);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_HOME_OP_CC:%d", lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_HOME_OP_CC = %s\n",
               gProcessName, mHomeCc););
      
      strcpy(lKey, "SYS_NDC_SN_LENGTH");
      lDbErr = dbObj.ReadCfgNum(lKey, mNdcSnLength, 1, 16);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_NDC_SN_LENGTH:%d", lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_NDC_SN_LENGTH = %d\n",
               gProcessName, mNdcSnLength););
      
      strcpy(lKey, "SYS_MMP_CGW_ERR_LMT");
      lDbErr = dbObj.ReadCfgNum(lKey, mCgwErrLmt, 1, 30);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_CGW_ERR_LMT:%d", lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_CGW_ERR_LMT = %d\n",
               gProcessName, mCgwErrLmt););
      
      strcpy(lKey, "SYS_MMP_DEREG_ON_REQ");
      lDbErr = dbObj.ReadCfgNum(lKey, mSendUnreg, 0, 1);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_DEREG_ON_REQ:%d", lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_DEREG_ON_REQ = %d\n",
               gProcessName, mSendUnreg););
      
      strcpy(lKey, "SYS_MMP_IN_RETRY");
      lDbErr = dbObj.ReadCfgNum(lKey, mInRetryAttempts, 1, 10);

      if(lDbErr != DB_NO_ERR)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_MMP_IN_RETRY:%d", lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      T(gTrace, printf("%s: SYS_MMP_IN_RETRY = %d\n",
               gProcessName, mInRetryAttempts););
   }  

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpConfigApi::ReloadConfig()
{
   return this->ReadConfig(false);
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpConfigApi::ReloadConfig(MmpDbManager &dbObj)
{
   return this->ReadConfigFromDb(dbObj, false);
}

