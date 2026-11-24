static const char *id = "@(#) $Id: IvrConfigApi.cc,v 1.1.4.3 2016/04/12 21:59:16 shijuc Exp $";
//----------------------------------------------------------------------
// NAME : IvrConfigApi.cc
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

// Originated: KOTHARI KOMAL                           Date: 17-JAN-2012
//----------------------------------------------------------------------
#include "IvrConfigApi.h"

//----------------------------------------------------------------------
// METHOD      : GetVersion
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None 
//----------------------------------------------------------------------
const char* IvrConfigApi::GetVersion()
{
   return id;
}

//----------------------------------------------------------------------
// METHOD      : IvrConfigApi
// DESCRIPTION : Constructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
IvrConfigApi::IvrConfigApi ()
{
   mIsInitFile = false;
   mIsInitDb = false;
   mIsMmpEnabled = false;
   mConfigType = CONFIG_IVR_ALL;
   mScheduleReTryAttempts = IVR_DEFAULT_SCHEDULE_RETRY_ATTEMPTS;
}

//----------------------------------------------------------------------
// METHOD      : IvrConfigApi
// DESCRIPTION : Constructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
IvrConfigApi::IvrConfigApi (EnumIvrConfigType configType)
{
   mIsInitFile = false;
   mIsInitDb = false;
   mConfigType = configType;
}
//----------------------------------------------------------------------
// METHOD      : ~IvrConfigApi
// DESCRIPTION : Destructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
IvrConfigApi::~IvrConfigApi ()
{
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrConfigApi::Init (EnumIvrConfigType cfgType)
{
   mConfigType = cfgType;
   if (mIsInitFile)
      return true;

   mIsInitFile = this->ReadConfig ();

   return mIsInitFile;
}


//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrConfigApi::Init (IvrDbManager & dbObj, EnumIvrConfigType cfgType)
{
   mConfigType = cfgType;
   if (mIsInitDb)
      return true;

   if (false == mIsInitFile)
      mIsInitFile = this->ReadConfig ();

   mIsInitDb = this->ReadConfigFromDb (dbObj);

   if (mIsInitDb && mIsInitFile)
      T (gTrace,
            printf ("%s:\33[32m IvrConfigApi::Initialization ok...\33[0m\n",
               gProcessName););

   return (mIsInitDb && mIsInitFile);
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrConfigApi::ReadConfig(BOOLEAN initial)
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(initial)
   { // All the configration values which needs not be reloaded will go here

      // ***************************** Read ipc Params ********************
      // Read message queue key values
      if(mConfigType & CONFIG_IVR_IPC)
      {
         if (lCfgRead.CfgInit (IVR_IPC_CFG) != true)
         {
            TERR (gTrace, printf ("%s: Could not open file %s\n",
                     gProcessName, IVR_IPC_CFG););
            gLog.GenerateLog (GSYS09,(TEXT*) IVR_IPC_CFG);
            return false;
         }

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_CCXML_REQ_Q_RCV", mCcxmlReqQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_CCXML_REQ_Q_RCV", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }
         else
         {
            T (gTrace, printf ("%s: MSG_CCXML_REQ_Q_RCV = %d\n",
                     gProcessName, mCcxmlReqQueKey););
         }

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_VXML_REQ_Q_RCV", mVxmlReqQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_VXML_REQ_Q_RCV", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }
         else
         {
            T (gTrace, printf ("%s: MSG_VXML_REQ_Q_RCV = %d\n",
                     gProcessName, mVxmlReqQueKey););
         }

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_VXML_CHILD_REQ_Q_RCV", mVxmlChildReqQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_VXML_CHILD_REQ_Q_RCV", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }
         else
         {
            T (gTrace, printf ("%s: MSG_VXML_CHILD_REQ_Q_RCV = %d\n",
                     gProcessName, mVxmlChildReqQueKey););
         }

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_CALL_ALERT_Q_RCV", mCallAlertQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_CALL_ALERT_Q_RCV", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }
         else
         {
            mCcxmlRspQueKey = mCallAlertQueKey;
            mVxmlRspQueKey  = mCallAlertQueKey;

            T (gTrace, printf ("%s: MSG_CALL_ALERT_Q_RCV = %d\n",
                     gProcessName, mCallAlertQueKey););
         }

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_EVENT_CHILD_Q_RCV", mEventChildQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_EVENT_CHILD_Q_RCV ", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }

         T (gTrace, printf ("%s: MSG_EVENT_CHILD_Q_RCV = %d\n",
                  gProcessName, mEventChildQueKey););

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_CCXML_EVENT_Q_RCV", mCcxmlEventQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_CCXML_EVENT_Q_RCV ", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }

         T (gTrace, printf ("%s: MSG_CCXML_EVENT_Q_RCV = %d\n",
                  gProcessName, mCcxmlEventQueKey););

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_ISUP_ENC_Q_RCV", mIsupEncQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_ISUP_ENC_Q_RCV ", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }

         T (gTrace, printf ("%s: MSG_ISUP_ENC_Q_RCV = %d\n",
                  gProcessName, mIsupEncQueKey););

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_EXT_VXML_RSP_Q_RCV", mExtVxmlRspQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_EXT_VXML_RSP_Q_RCV ", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }

         T (gTrace, printf ("%s: MSG_EXT_VXML_RSP_Q_RCV = %d\n",
                  gProcessName, mExtVxmlRspQueKey););

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_CHANNEL_STATUS_Q_RCV", mChannelStatusQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_CHANNEL_STATUS_Q_RCV ", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }

         T (gTrace, printf ("%s: MSG_IVR_PEG_Q_RCV = %d\n",
                  gProcessName, mIvrPegQueKey););

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("MSG_IVR_PEG_Q_RCV", mIvrPegQueKey,
                  IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "MSG_IVR_PEG_Q_RCV ", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }

         T (gTrace, printf ("%s: MSG_IVR_PEG_Q_RCV = %d\n",
                  gProcessName, mIvrPegQueKey););

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("SEM_BILLING_RECORD_KEY", mSemBillingRecordKey,
                  IVR_MIN_SEM_KEY, IVR_MAX_SEM_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "SEM_BILLING_RECORD_KEY", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }
         else
         {
            T (gTrace, printf ("%s: SEM_BILLING_RECORD_KEY = %d\n",
                     gProcessName, mSemBillingRecordKey););
         }

         if (CFG_OK !=
               lCfgRead.GetConfigNum ("SHM_BILLING_RECORD_KEY", mShmBillingRecordKey,
                  IVR_MIN_SHM_KEY, IVR_MAX_SHM_KEY))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Config read failed for %s. File:%s.",
                  "SHM_BILLING_RECORD_KEY", IVR_IPC_CFG);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            lCfgRead.CfgDeInit ();
            return false;
         }
         else
         {
            T (gTrace, printf ("%s: SHM_BILLING_RECORD_KEY = %d\n",
                     gProcessName, mShmBillingRecordKey););
         }

      }
      lCfgRead.CfgDeInit ();

      if (lCfgRead.CfgInit (IVR_KER_CFG) != true)
      {
         TERR (gTrace, printf ("%s: Could not open file %s\n",
                  gProcessName, IVR_KER_CFG););
         gLog.GenerateLog (GSYS09,(TEXT*) IVR_KER_CFG);
         return false;
      }

      if (CFG_OK !=
            lCfgRead.GetConfigNum ("IVR_DB_CLIENT",mDbClient,
               0,10))
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Config read failed for %s. File:%s.",
               "IVR_DB_CLIENT", IVR_KER_CFG);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         lCfgRead.CfgDeInit ();
         return false;
      }
      else
      {
         T (gTrace, printf ("%s: IVR_DB_CLIENT = %d\n",
                  gProcessName, mDbClient););
      }
      lCfgRead.CfgDeInit ();

   }

   // All the configration values which needs to be reloaded will go here
   {
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
BOOLEAN IvrConfigApi::ReadConfigFromDb (IvrDbManager & dbObj, BOOLEAN initial)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT32 lCfgValue = 0;
   TEXT            lKey[50] = "";
   DbErrorCodes    lDbErr;

   if(initial)
   { // All the configration values which needs not be reloaded will go here
      // CONFIG_IVR_KERNEL : IVR Kernel related Read-ONCE Configurations
      if(mConfigType & CONFIG_IVR_KERNEL)
      {
      }
      // CONFIG_IVR_MMP : MMP application related Read-ONCE Configurations
      if(mConfigType & CONFIG_IVR_MMP)
      {
      }
   }

   // 1. All Logs Required
   strcpy(lKey, "SYS_ALL_LOGS_REQUIRED");
   lDbErr = dbObj.ReadCfgNum(lKey, lCfgValue, 0, 1);

   if(lDbErr != DB_NO_ERR)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Db Error in reading configuration SYS_ALL_LOGS_REQUIRED Err:%d",
            lDbErr);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      return false;
   }
   if(lCfgValue)
      mAllLogsRequired = true;

   T(gTrace, printf("%s: SYS_ALL_LOGS_REQUIRED = %d\n",gProcessName, mAllLogsRequired););

   // 2. MMP License enabled or not
   strcpy(lKey, "KER_MMP_LICENSE");
   lDbErr = dbObj.ReadCfgNum(lKey, lCfgValue, 0, 1);

   if(lDbErr != DB_NO_ERR)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Db Error in reading configuration KER_MMP_LICENSE Err:%d",
            lDbErr);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      return false;
   }
   if(lCfgValue)
      mIsMmpEnabled = true;

   T(gTrace, printf("%s: KER_MMP_LICENSE = %d\n",
            gProcessName, mIsMmpEnabled););

   // All the configration values which needs to be reloaded will go here
   // CONFIG_IVR_KERNEL : IVR Kernel related Reloadable Configurations
   if(mConfigType & CONFIG_IVR_KERNEL)
   {
      if (DB_NO_ERR != dbObj.ReadCfgNum ("KER_RING_DURATION", lCfgValue,
               IVR_MIN_RING_DURATION, IVR_MAX_RING_DURATION))
      {
         mRingDuration = IVR_DEFAULT_RING_DURATION;
      }
      else
      {
         mRingDuration = (INT16)lCfgValue;
      }

      T (gTrace, printf ("%s: KER_RING_DURATION = %d\n", gProcessName, mRingDuration););
      
      if (DB_NO_ERR != dbObj.ReadCfgNum ("SYS_SCHEDULE_RETRY_TIME", lCfgValue,
               IVR_MIN_SCHEDULE_RETRY_TIME, IVR_MAX_SCHEDULE_RETRY_TIME))
      {
         mScheduleReTryTime = IVR_DEFAULT_SCHEDULE_RETRY_TIME;
      }
      else
      {
         mScheduleReTryTime = (INT16)lCfgValue;
      }

      T (gTrace, printf ("%s: SYS_SCHEDULE_RETRY_TIME = %d\n", gProcessName, mScheduleReTryTime););

      if (DB_NO_ERR != dbObj.ReadCfgNum ("SYS_SCHEDULE_RETRY_ATTEMPTS", lCfgValue,
               IVR_MIN_SCHEDULE_RETRY_ATTEMPTS, IVR_MAX_SCHEDULE_RETRY_ATTEMPTS))
      {
         mScheduleReTryAttempts = IVR_DEFAULT_SCHEDULE_RETRY_ATTEMPTS;
      }
      else
      {
         mScheduleReTryAttempts = (INT16)lCfgValue;
      }

      T (gTrace, printf ("%s: SYS_SCHEDULE_RETRY_ATTEMPTS = %d\n", gProcessName, mScheduleReTryAttempts););
   }

   // MMP Charging Configuration parameters
   if(mConfigType & CONFIG_IVR_BILLING)
   {
      strcpy(lKey, "SYS_CHARGING_FUNCTION_TYPE");
      if( DB_NO_ERR != dbObj.ReadCfgNum(lKey, lCfgValue, 0, 2 )) //CHARGE_FUNCTION_NONE-0, CHARGE_FUNCTION_MAX-2);
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Db Error in reading configuration SYS_CHARGING_FUNCTION_TYPE Err:%d",
               lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS11, lLogText);
         return false;
      }
      mChargeFunction = lCfgValue;
      T(gTrace, printf("%s: SYS_CHARGING_FUNCTION_TYPE = %d\n",
               gProcessName, mChargeFunction););

   }

   // CONFIG_IVR_MMP : MMP application related Reloadable Configurations
   if(mConfigType & CONFIG_IVR_MMP)
   {
   }


   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrConfigApi::ReloadConfig()
{
   return this->ReadConfig(false);
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrConfigApi::ReloadConfig(IvrDbManager &dbObj)
{
   return this->ReadConfigFromDb(dbObj, false);
}

