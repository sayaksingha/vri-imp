static const char *id = "@(#) $Id: MmpPackageBilling.cc,v 1.1.4.3.2.1 2016/04/12 23:18:05 mram Exp $";
//----------------------------------------------------------------------
// NAME:
// MmpPackageBilling.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright (C) 2006,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd.
//
// DESCRIPTION
// MMP Package Billing library to enable Online Charging module to 
//   charge the subscriber from his subscribed package.
//
// Originated : KOTHARI KOMAL                                16-APR-2012
//----------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include "BillingUtil.h"
#include "MmpPackageBilling.h"

extern CTrace gTrace;

extern TEXT gProcessName[];
extern Log  gLog;
extern unsigned int       gReferenceNum;

UINT32 timerExpiry(void* ptr, UINT32 channelId)
{
   MmpPackageBilling  *lpMmpPackageBilling = (MmpPackageBilling  *) ptr;
  
   T(gTrace, printf("%s: Session Timer Expired. CID:%05d\n",gProcessName, channelId););
   lpMmpPackageBilling->ProcessTimerExpiry(channelId, BILLING_TIMER_EXPIRY);

   return channelId;
}

//----------------------------------------------------------------------
// METHOD      : GetVersion
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
const char* MmpPackageBilling::GetVersion()
{
   return id;
}

//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
MmpPackageBilling::MmpPackageBilling ()
{
}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
MmpPackageBilling::~MmpPackageBilling ()
{
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::Init ()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   
   // Read the Configuration...
   if (false == mCfgObj.Init(CONFIG_MMP_CHARGING|CONFIG_MMP_IPC))
   {
      sprintf (lLogText, "Initilising the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   //T(gTrace, printf ("%s: Configuration Initilization success..\n", gProcessName););

   if(DB_NO_ERR != mDbManager.Init(mCfgObj.mDbClient))  // 1 - MySql DB Client , 0 - Oracle DB Client
   {
      sprintf(lLogText, "DataBase Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   // Read the Configuration...
   if (false == mCfgObj.Init(mDbManager, CONFIG_MMP_CHARGING))
   {
      sprintf (lLogText, "Initilising the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   //T(gTrace, printf ("%s: Configuration Initilization success..\n", gProcessName););

   // Initialize the QueAPI
   if (false == mQueApi.Init())
   {
      sprintf (lLogText, "Initilising QueAPI - failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: QueAPI Initilization - success..\n", gProcessName););

   // Create Semaphore which will be used as Lock for O/G Dialog Allocation...
   // SemKey and Semno
   if (false == mSemObj.Init(mCfgObj.mSemBillingRecordKey, 1))
   {
      sprintf (lLogText, "Initilising the Semaphore failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Semaphore Initilization success..\n", gProcessName););


   // Create Dilogue Details Shared Memory...
   // ShmKey startDlg   EndDlg
   if (false == mBillingRecordShm.Init(mCfgObj.mShmBillingRecordKey, IVR_MIN_CHANNEL_ID, IVR_MAX_CHANNEL_ID))
   {
      sprintf (lLogText, "Initilising the Billing Record Detail Table failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Billing Record Detail Table Initilization success..\n", gProcessName););

   // store the Shm Pointer
   //mpBillingRecord= mBillingRecordShm.GetShmPointer();

   mTimer = new MLHTimer();
//   if (mTimer->InitTimeoutTable(10000, 19001, 19002, 19003, 19004, 19005, 19006, timerExpiry, this) != MLH_TIMER_SUCCESS)
   {
      sprintf(lLogText, "Timer Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM09, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : SetBillingRecord
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::SetBillingRecord(INT32 index, BillingRecordDetails &record)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BOOLEAN lRetVal = mBillingRecordShm.InsertData(&record, index);

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d AppId:%d EVT:%d MSISDN:%s",
         record.referenceNum, record.channelId, record.appId, record.currentEvent, record.msisdn);

   if(false == lRetVal)
   {
      TERR(gTrace, printf("%s: %s Failed to store the Billing Record detail\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM17, lLogText);
      return false;
   }

   T(gTrace, printf("%s: %s Store Billing Record detail - Success\n", gProcessName, lLogText););
   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM18, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ResetBillingRecord
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ResetBillingRecord(INT32 index, BillingRecordDetails &record)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BOOLEAN lRetVal = mBillingRecordShm.ResetSlot(index);

   if(!lRetVal)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d AppId:%d EVT:%d MSISDN:%s",
            record.referenceNum, record.channelId, record.appId, record.currentEvent, record.msisdn);
      TERR(gTrace, printf("%s: %s Reset Billing Record detail - Failed\n", gProcessName, lLogText););
      if(mCfgObj.mAllLogsRequired)
         gLog.GenerateLog(OCM18, lLogText);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : GetBillingRecord
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::GetBillingRecord(INT32 index, BillingRecordDetails &record)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   memset(&record, 0, sizeof(BillingRecordDetails));

   BOOLEAN lRetVal = mBillingRecordShm.GetData(&record, index);

   if(lRetVal)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d AppId:%d EVT:%d MSISDN:%s",
            record.referenceNum, record.channelId, record.appId, record.currentEvent, record.msisdn);
      T(gTrace, printf("%s: %s Get Billing Record detail - Success\n", gProcessName, lLogText););
      if(mCfgObj.mAllLogsRequired)
         gLog.GenerateLog(OCM18, lLogText);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : StartNomTimer
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::StartNomTimer(BillingRecordDetails &record)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d", record.referenceNum, record.channelId, record.currentEvent);

   if(record.timerTag != 0)
   {
      TERR(gTrace, printf("%s: %s Timer already started.\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM15, lLogText);
      return false;
   }

   //if( MLH_TIMER_SUCCESS != mTimer->SetTimeout(timerExpiry, record.channelId, record.timerTag, (record.limits.usage * 60)))
  // if( MLH_TIMER_SUCCESS != mTimer->SetTimeout(record.channelId, record.timerTag, (record.limits.usage * 60)))
   {
      TERR(gTrace, printf("%s: %s Start Timer Failed\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM11, lLogText);
      return false;
   }

   T(gTrace, printf("%s: %s Timer Started : CID:%05d Duration:%dSec Tag:%d\n", gProcessName, lLogText, 
            record.channelId, (record.limits.usage * 60), record.timerTag););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM12, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : StopNomTimer
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::StopNomTimer(BillingRecordDetails &record)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d", record.referenceNum, record.channelId, record.currentEvent);
   if(record.timerTag == 0)
   {
      TERR(gTrace, printf("%s: %s Timer Not Enabled.\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM10, lLogText);
      return true;
   }
   if( MLH_TIMER_SUCCESS != mTimer->CancelTimer(record.timerTag))
   {
      TERR(gTrace, printf("%s: %s Cancel Timer Failed\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM13, lLogText);
      return false;
   }
   T(gTrace, printf("%s: %s Timer Canceled\n", gProcessName, lLogText););
   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM14, lLogText);
   return true;
}

//----------------------------------------------------------------------
// METHOD      : GenerateCdr
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::GenerateCdr(BillingRecordDetails &record)
{
   //TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   return true;
}

//----------------------------------------------------------------------
// METHOD      : SendBillingResponse
// DESCRIPTION :
// PARAMETER   : BillingResponse
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::SendReleaseCallSignal(INT32 channelId)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   KernelMsg   lKernelMsg;

   memset(&lKernelMsg, 0, sizeof(KernelMsg));

   lKernelMsg.kernelMsgType = ISUP_MSG;
   lKernelMsg.isupMsg.isupMsgType      = GEN_ISUP_REL;
   lKernelMsg.isupMsg.circuitId        = channelId;
   lKernelMsg.isupMsg.isupRel.optLen   = 0 ;

   sprintf(lLogText, "RefNum: %08X CID:%05d", gReferenceNum, channelId);

   if(false == mQueApi.WriteToCallAlertQue(lKernelMsg))
   {
      TERR(gTrace, printf("%s: %s Sending ISUP_REL to SessionMgr failed\n", gProcessName, lLogText););
      gLog.GenerateLog(CHMG02, lLogText);
      return false;
   }
   T(gTrace, printf("%s: %s ISUP_REL message Sent to SessionMgr\n", gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : SendBillingResponse
// DESCRIPTION :
// PARAMETER   : BillingResponse
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::SendBillingResponse(BillingResponse &respone)
{
   //TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   return true;
}

//----------------------------------------------------------------------
// METHOD      : GetPackageType
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
EnumMmpPkgType MmpPackageBilling::GetPackageType(EnumBillingEvents event)
{
   EnumMmpPkgType lPackageType=MMP_PACKAGE_UNKNOWN;
   switch(event)
   {
      case BILLING_CALL_RECEIVED :
      case BILLING_CALL_ANSEWERED :
      case BILLING_CALL_DISCONNECTED :
         lPackageType=MMP_PACKAGE_MINUTES; 
         break;
      case BILLING_CONTENT_PLAY :
      case BILLING_CONTENT_START :
      case BILLING_CONTENT_STOP :
         lPackageType=MMP_PACKAGE_MCS;
         break;
      case BILLING_OFFLINE_DEDICATION :
         lPackageType=MMP_PACKAGE_DEDICATION;
         break;
      case BILLING_OFFLINE :
         lPackageType=MMP_PACKAGE_ALL;
         break;
      case BILLING_TIMER_EXPIRY :
         break;
      case BILLING_UNKNOWN_EVENT: 
      default:
         break;
   }

   return lPackageType;
}

//----------------------------------------------------------------------
// METHOD      : GetSubsPackage
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BillingLimits MmpPackageBilling::GetSubsPackage(TEXT* subsNo, EnumMmpPkgType packageType, SubsPackageInfo &packageInfo, UINT32 usage)
{
   //TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingLimits lBillingLimits;
   DbErrorCodes  lDbErr;

   memset(&packageInfo, 0, sizeof(SubsPackageInfo));
   memset(&lBillingLimits, 0, sizeof(BillingLimits));

   lDbErr = mDbManager.GetSubsPackage(subsNo, packageType, packageInfo, usage);

   T(gTrace, printf("%s: RefNum:%08X PID:%d SUBSID:%d PKGT:%d USAGE:(%d,%d,%d,%d)\n", gProcessName, gReferenceNum, packageInfo.packageId, 
            packageInfo.subscriptionId, packageType, packageInfo.min, packageInfo.content, packageInfo.dedication, packageInfo.subscription););

   if(lDbErr == DB_NO_ERR)
   {
      lBillingLimits.subscriptionId = packageInfo.subscriptionId;
      lBillingLimits.packageId = packageInfo.packageId;
      lBillingLimits.packageType = packageType;
      switch(packageType)
      {
         case MMP_PACKAGE_MINUTES:
            lBillingLimits.usage = packageInfo.min;
         break;
         case MMP_PACKAGE_CONTENTS:
            lBillingLimits.usage = packageInfo.content;
         break;
         case MMP_PACKAGE_DEDICATION:
            lBillingLimits.usage = packageInfo.dedication;
         break;
         case MMP_PACKAGE_SUBSCRIPTION:
            lBillingLimits.usage = packageInfo.subscription;
         break;
         default:
            memset(&lBillingLimits, 0, sizeof(BillingLimits));
      }

      T(gTrace, printf("%s: RefNum:%08X PID:%d SUBSID:%d PKGT:%d USAGE:(%d)\n", gProcessName, gReferenceNum, lBillingLimits.packageId,
         lBillingLimits.subscriptionId, lBillingLimits.packageType, lBillingLimits.usage););
   }      
   return lBillingLimits;
}

//----------------------------------------------------------------------
// METHOD      : GetSubsPackageForContent
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BillingLimits MmpPackageBilling::GetSubsPackageForContent(BillingRecordDetails &record)
{
   //TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingLimits lBillingLimits;

   memset(&lBillingLimits, 0, sizeof(BillingLimits));

   lBillingLimits = this->GetSubsPackage(record.msisdn, MMP_PACKAGE_SUBSCRIPTION, record.packageInfo, record.contentId);

   if(lBillingLimits.subscriptionId == 0)
   {
      lBillingLimits = this->GetSubsPackage(record.msisdn, MMP_PACKAGE_CONTENTS, record.packageInfo);
      if(lBillingLimits.subscriptionId == 0)
      {
         lBillingLimits = this->GetSubsPackage(record.msisdn, MMP_PACKAGE_MINUTES, record.packageInfo);
      }
   }   

   return lBillingLimits;
}

//----------------------------------------------------------------------
// METHOD      : ChargeFromPackage
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ChargeFromPackage(UINT32 subscriptionId, EnumMmpPkgType packageType, UINT32 usage, SubsPackageInfo &packageInfo)
{
   //TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   DbErrorCodes  lDbErr;
   BOOLEAN       lChargeSuccess = false;

   switch(packageType)
   {
      case MMP_PACKAGE_SUBSCRIPTION:
      if((packageInfo.packageType & MMP_PACKAGE_SUBSCRIPTION) && (packageInfo.subscription == usage))
      {
         lChargeSuccess = true;
      }
      else
      {
         // Not subscribed to the required content
      }         
      break;
      case MMP_PACKAGE_MINUTES:
      case MMP_PACKAGE_CONTENTS:
      case MMP_PACKAGE_DEDICATION:
         lDbErr = mDbManager.ChargeFromPackage(subscriptionId, packageType, usage, packageInfo);
         if(lDbErr != DB_NO_ERR)
         {
            // Unable to charge from package - Generate Audit for failure

         }
         else
         {
            // Generate Success Log
            lChargeSuccess = true;
         }

      break;
      default:
         // Invalid Package Type
         // Generate Failure Logs
         break;
        
   }

   if(lChargeSuccess)
   {
      //Generate Audit for success
   }
   else
   {
      //Generate Audit for failure
   }
   return lChargeSuccess;
}

//----------------------------------------------------------------------
// METHOD      : ProcessBillingEvent
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessBillingEvent(BillingRecordDetails record, BillingResponse &response)
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d", gReferenceNum, record.channelId, record.currentEvent);
   T(gTrace, printf("%s: %s Process Billing Event.\n", gProcessName, lLogText););
   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM02, lLogText);
   //record.lastEvent = record.currentEvent;
   //record.currentEvent = event;
   switch(record.currentEvent)
   {
      case BILLING_CALL_RECEIVED :
         return this->ProcessCallReceived(record, response);      
      case BILLING_CALL_ANSEWERED :
         return this->ProcessCallAnswered(record, response);
      case BILLING_CALL_DISCONNECTED :
         return this->ProcessCallDisconnected(record, response);
      case BILLING_CONTENT_PLAY :
         return this->ProcessContentPlay(record, response);
      case BILLING_CONTENT_START :
         return this->ProcessContentStart(record, response);
      case BILLING_CONTENT_STOP :
         return this->ProcessContentStop(record, response);
      case BILLING_OFFLINE_DEDICATION :
         return this->ProcessOfflineDedication(record, response);
      case BILLING_OFFLINE :
         return this->ProcessOffline(record, response);
      case BILLING_TIMER_EXPIRY :
         return this->ProcessTimerExpiry(record.channelId, BILLING_TIMER_EXPIRY);
      case BILLING_UNKNOWN_EVENT: 
      default:
         return this->ProcessUnknownEvent(record, response);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ProcessBillingEvent
// DESCRIPTION :
// PARAMETER   : UINT32 channelId, EnumBillingEvents event
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessBillingEvent(UINT32 channelId, EnumBillingEvents event, BillingResponse &response)
{
   TEXT                 lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingRecordDetails lRecord;

   if(false == this->GetBillingRecord(channelId, lRecord))
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d", gReferenceNum, channelId, event);
      TERR(gTrace, printf("%s: %s Billing Record Retrieval Failed.\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM02, lLogText);
      return false;
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d", gReferenceNum, channelId, event);
   T(gTrace, printf("%s: %s Billing Record Retrieved.\n", gProcessName, lLogText););
   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM02, lLogText);
   //lRecord.lastEvent = lRecord.currentEvent;
   //lRecord.currentEvent = event;
   switch(event)
   {
      case BILLING_CALL_RECEIVED :
         return this->ProcessCallReceived(lRecord, response);      
      case BILLING_CALL_ANSEWERED :
         return this->ProcessCallAnswered(lRecord, response);
      case BILLING_CALL_DISCONNECTED :
         return this->ProcessCallDisconnected(lRecord, response);
      case BILLING_CONTENT_PLAY :
         return this->ProcessContentPlay(lRecord, response);
      case BILLING_CONTENT_START :
         return this->ProcessContentStart(lRecord, response);
      case BILLING_CONTENT_STOP :
         return this->ProcessContentStop(lRecord, response);
      case BILLING_OFFLINE_DEDICATION :
         return this->ProcessOfflineDedication(lRecord, response);
      case BILLING_OFFLINE :
         return this->ProcessOffline(lRecord, response);
      case BILLING_TIMER_EXPIRY :
         return this->ProcessTimerExpiry(channelId, event);
      case BILLING_UNKNOWN_EVENT: 
      default:
         return this->ProcessUnknownEvent(lRecord, response);
   }

   return true;
}
//----------------------------------------------------------------------
// METHOD      : ProcessCallReceived
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessCallReceived(BillingRecordDetails &record, BillingResponse &response)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingLimits   lLimits;
   //BillingResponse response;

   //memset(&response, 0, sizeof(BillingResponse));

   response.event = record.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", record.msisdn);
   response.refId = record.channelId;
   response.referenceNum = record.referenceNum;

   lLimits = this->GetSubsPackage(record.msisdn, MMP_PACKAGE_MINUTES, record.packageInfo);

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, MMP_PACKAGE_MINUTES);

   record.limits = lLimits;

   if((record.limits.subscriptionId == 0) || (record.limits.usage == 0)) //(lLimits.usage == 0)
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessCallReceived\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);

      response.status = BILLING_NO_PACKAGE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, lLimits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);

   //Set the CDR parameters
   snprintf(record.cdr.cldParty, SS7_MAX_ADDRESS_LEN, "%s", record.shortCode);
   snprintf(record.cdr.clgParty, SS7_MAX_ADDRESS_LEN, "%s", record.msisdn);
   snprintf(record.cdr.imsi, SS7_MAX_ADDRESS_LEN, "%s", record.imsi);
   sprintf(record.cdr.referenceNumber, "%08X", record.referenceNum);
   record.cdr.connectTime = time(NULL);

   if(false == this->SetBillingRecord(record.channelId, record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }

   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   response.status = BILLING_SUCCESS;
   return this->SendBillingResponse(response);
}

//----------------------------------------------------------------------
// METHOD      : ProcessCallAnswered
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessCallAnswered(BillingRecordDetails &record, BillingResponse &response)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //BillingResponse response;

   //memset(&response, 0, sizeof(BillingResponse));

   response.event = record.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", record.msisdn);
   response.refId = record.channelId;
   response.referenceNum = record.referenceNum;

   snprintf(lLogText, MAX_LOG_TEXT_LEN,  "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType);

   //Set the CDR parameters
   record.cdr.answerTime = time(NULL);
   record.cdr.disconnectTime = time(NULL);
   record.cdr.stopTime = time(NULL);

   if((record.limits.subscriptionId == 0) || (record.limits.usage == 0))
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessCallAnswered\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);

      response.status = BILLING_NO_PACKAGE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, record.limits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);
   
   if(false == this->StartNomTimer(record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - Timer Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM07, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }
   
   if(false == this->SetBillingRecord(record.channelId, record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }

   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   response.status = BILLING_SUCCESS;
   return this->SendBillingResponse(response);
}
//----------------------------------------------------------------------
// METHOD      : ProcessCallDisconnected
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessCallDisconnected(BillingRecordDetails &record, BillingResponse &response)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //BillingResponse response;

   //memset(&response, 0, sizeof(BillingResponse));

   response.event = record.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", record.msisdn);
   response.refId = record.channelId;
   response.referenceNum = record.referenceNum;

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType);

   //Set the CDR parameters
   record.cdr.disconnectTime = time(NULL);
   record.cdr.stopTime = time(NULL); 

   if(record.cdr.answerTime > 0)
   {
      record.cdr.duration = (UINT32)difftime(record.cdr.disconnectTime, record.cdr.answerTime);
      if(false == this->StopNomTimer(record))
      {
         T(gTrace, printf("%s: %s Event Processing Failed - Stop Timer Error\n", gProcessName, lLogText););
         gLog.GenerateLog(OCM07, lLogText);

         response.status = BILLING_SYSTEM_FAILURE;
         return this->SendBillingResponse(response);
      }
   }
   else
      record.cdr.duration = 0;

   record.cdr.chargeUnit = CHARGE_UNIT_NOM;
   record.cdr.unitCharged = CEILING(record.cdr.duration/60.0);

   if((record.limits.subscriptionId == 0) || (record.limits.usage == 0))
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessCallDisconnected\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);

      response.status = BILLING_NO_PACKAGE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,"RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, record.limits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);
   
   if(false == this->ResetBillingRecord(record.channelId, record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,"RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) UNIT_CHARGED:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, record.cdr.unitCharged);

   if(false == this->ChargeFromPackage(record.packageInfo.subscriptionId, record.limits.packageType, record.cdr.unitCharged, record.packageInfo))
   {
      TERR(gTrace, printf("%s: %s Charging Failed\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM08, lLogText);
      response.status = BILLING_SYSTEM_FAILURE;
   }
   else
   {
      T(gTrace, printf("%s: %s Charging Success\n", gProcessName, lLogText););
      response.status = BILLING_SUCCESS;
   }
   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   // Generate Final CDR
   this->GenerateCdr(record);

   return this->SendBillingResponse(response);
}
//----------------------------------------------------------------------
// METHOD      : ProcessContentPlay
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessContentPlay(BillingRecordDetails &record, BillingResponse &response)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingLimits   lLimits;
   //BillingResponse response;

   //memset(&response, 0, sizeof(BillingResponse));

   response.event = record.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", record.msisdn);
   response.refId = record.channelId;
   response.referenceNum = record.referenceNum;

   lLimits = this->GetSubsPackageForContent(record);

   record.limits = lLimits;

   snprintf(lLogText, MAX_LOG_TEXT_LEN,"RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, MMP_PACKAGE_MINUTES);
   if((record.limits.subscriptionId == 0) || (record.limits.usage == 0)) //(lLimits.usage == 0)
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessContentPlay\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);

      response.status = BILLING_NO_PACKAGE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, lLimits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);

   //Set the CDR parameters
   snprintf(record.cdr.cldParty, SS7_MAX_ADDRESS_LEN, "%s", record.shortCode);
   snprintf(record.cdr.clgParty, SS7_MAX_ADDRESS_LEN, "%s", record.msisdn);
   snprintf(record.cdr.imsi, SS7_MAX_ADDRESS_LEN, "%s", record.imsi);
   sprintf(record.cdr.referenceNumber, "%08X", record.referenceNum);
   record.cdr.connectTime = time(NULL);

   if(false == this->SetBillingRecord(record.channelId, record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }

   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   response.status = BILLING_SUCCESS;
   return this->SendBillingResponse(response);

   return true;
}
//----------------------------------------------------------------------
// METHOD      : ProcessContentStart

// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessContentStart(BillingRecordDetails &record, BillingResponse &response)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //BillingResponse response;

   //memset(&response, 0, sizeof(BillingResponse));

   response.event = record.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", record.msisdn);
   response.refId = record.channelId;
   response.referenceNum = record.referenceNum;

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType);

   //Set the CDR parameters
   record.cdr.answerTime = time(NULL);
   record.cdr.disconnectTime = time(NULL);
   record.cdr.stopTime = time(NULL);

   if((record.limits.subscriptionId == 0) || (record.limits.usage == 0))
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessContentStart \n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);

      response.status = BILLING_NO_PACKAGE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, record.limits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);
  
   if(MMP_PACKAGE_MINUTES == record.limits.packageType)
   {
      if(false == this->StartNomTimer(record))
      {
         T(gTrace, printf("%s: %s Event Processing Failed - Timer Error\n", gProcessName, lLogText););
         gLog.GenerateLog(OCM07, lLogText);

         response.status = BILLING_SYSTEM_FAILURE;
         return this->SendBillingResponse(response);
      }
   }

   if(false == this->SetBillingRecord(record.channelId, record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }

   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   response.status = BILLING_SUCCESS;
   return this->SendBillingResponse(response);
}

//----------------------------------------------------------------------
// METHOD      : ProcessContentStop
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessContentStop(BillingRecordDetails &record, BillingResponse &response)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //BillingResponse response;

   //memset(&response, 0, sizeof(BillingResponse));

   response.event = record.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", record.msisdn);
   response.refId = record.channelId;
   response.referenceNum = record.referenceNum;

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType);

   //Set the CDR parameters
   record.cdr.disconnectTime = time(NULL);
   record.cdr.stopTime = time(NULL);
   if(record.cdr.answerTime > 0)
      record.cdr.duration = (UINT32)difftime(record.cdr.disconnectTime, record.cdr.answerTime);
   else
      record.cdr.duration = 0;

   switch(record.limits.packageType)
   {
      case MMP_PACKAGE_MINUTES:
         record.cdr.chargeUnit = CHARGE_UNIT_NOM;
         record.cdr.unitCharged = CEILING(record.cdr.duration/60.0);
         if(false == this->StopNomTimer(record))
         {
            T(gTrace, printf("%s: %s Event Processing Failed - Stop Timer Error\n", gProcessName, lLogText););
            gLog.GenerateLog(OCM07, lLogText);

            response.status = BILLING_SYSTEM_FAILURE;
            return this->SendBillingResponse(response);
         }
         break;
      case MMP_PACKAGE_CONTENTS:
         record.cdr.chargeUnit = CHARGE_UNIT_CONTENT;
         record.cdr.unitCharged = 1;
         break;
      case MMP_PACKAGE_SUBSCRIPTION:
         record.cdr.chargeUnit = CHARGE_UNIT_SUBSCRIPTION;
         record.cdr.unitCharged = 0;
         break;
      default:
         record.cdr.chargeUnit = CHARGE_UNIT_OTHER_FLAT;
         record.cdr.unitCharged = 0;
   }

   if((record.limits.subscriptionId == 0) || (record.limits.usage == 0))
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessContentStop\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);

      response.status = BILLING_NO_PACKAGE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, record.limits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);
   
   if(false == this->ResetBillingRecord(record.channelId, record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,"RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) UNIT_CHARGED:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, record.cdr.unitCharged);

   if(false == this->ChargeFromPackage(record.packageInfo.subscriptionId, record.limits.packageType, record.cdr.unitCharged, record.packageInfo))
   {
      TERR(gTrace, printf("%s: %s Charging Failed\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM08, lLogText);
      response.status = BILLING_SYSTEM_FAILURE;
   }
   else
   {
      T(gTrace, printf("%s: %s Charging Success\n", gProcessName, lLogText););
      response.status = BILLING_SUCCESS;
   }
   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   // Generate Final CDR
   this->GenerateCdr(record);

   return this->SendBillingResponse(response);
}

//----------------------------------------------------------------------
// METHOD      : ProcessOfflineDedication
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessOfflineDedication(BillingRecordDetails &record, BillingResponse &response)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingLimits   lLimits;
   //BillingResponse response;

   //memset(&response, 0, sizeof(BillingResponse));

   response.event = record.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", record.msisdn);
   response.refId = record.channelId;
   response.referenceNum = record.referenceNum;

   lLimits = this->GetSubsPackage(record.msisdn, MMP_PACKAGE_DEDICATION, record.packageInfo);

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType);
   record.limits = lLimits;

   if((record.limits.subscriptionId == 0) || (record.limits.usage == 0)) //(lLimits.usage == 0)
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessOfflineDedication\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);

      response.status = BILLING_NO_PACKAGE;
      return this->SendBillingResponse(response);
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, lLimits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);

   //Set the CDR parameters
   snprintf(record.cdr.cldParty, SS7_MAX_ADDRESS_LEN, "%s", record.shortCode);
   snprintf(record.cdr.clgParty, SS7_MAX_ADDRESS_LEN, "%s", record.msisdn);
   snprintf(record.cdr.imsi, SS7_MAX_ADDRESS_LEN, "%s", record.imsi);
   sprintf(record.cdr.referenceNumber, "%08X", record.referenceNum);
   record.cdr.chargeUnit = CHARGE_UNIT_DEDICATION;
   record.cdr.unitCharged = 1;
   record.cdr.connectTime = time(NULL);
   record.cdr.disconnectTime = record.cdr.connectTime;
   record.cdr.stopTime = record.cdr.connectTime;
   record.cdr.duration = 0;

   /*if(false == this->ResetBillingRecord(record.channelId, record))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      return this->SendBillingResponse(response);
   }*/

   snprintf(lLogText, MAX_LOG_TEXT_LEN,"RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) UNIT_CHARGED:%d", record.referenceNum, 
      record.channelId, record.currentEvent, record.msisdn, record.limits.packageType,
      record.packageInfo.packageId, record.packageInfo.subscriptionId, record.cdr.unitCharged);

   if(false == this->ChargeFromPackage(record.packageInfo.subscriptionId, record.limits.packageType, record.cdr.unitCharged, record.packageInfo))
   {
      T(gTrace, printf("%s: %s Charging From Package Failed - DB Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM08, lLogText);
      response.status = BILLING_SYSTEM_FAILURE;
   }
   else
   {
      T(gTrace, printf("%s: %s Charging Success\n", gProcessName, lLogText););
      response.status = BILLING_SUCCESS;
   }
   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   // Generate Final CDR
   this->GenerateCdr(record);

   return this->SendBillingResponse(response);
}

//----------------------------------------------------------------------
// METHOD      : ProcessOffline
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessOffline(BillingRecordDetails &record, BillingResponse &response)
{
   //TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //BillingLimits lLimits;

   return false;
}
//----------------------------------------------------------------------
// METHOD      : ProcessTimerExpiry
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessTimerExpiry(UINT32 channelId, EnumBillingEvents event)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingResponse response;
   BillingRecordDetails lRecord;

   if(false == this->GetBillingRecord(channelId, lRecord))
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d", gReferenceNum, channelId, event);
      TERR(gTrace, printf("%s: %s Billing Record Retrieval Failed.\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM02, lLogText);
      return false;
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d", gReferenceNum, channelId, event);
   T(gTrace, printf("%s: %s Billing Record Retrieved.\n", gProcessName, lLogText););
   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM02, lLogText);

   memset(&response, 0, sizeof(BillingResponse));
   
   response.event = lRecord.currentEvent;
   response.mode = CHARGE_MODE_PACKAGE;
   snprintf(response.msisdn, MAX_MSISDN_LEN,"%s", lRecord.msisdn);
   response.refId = lRecord.channelId;
   response.referenceNum = lRecord.referenceNum;
   response.status = BILLING_TIMEOUT;

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d", lRecord.referenceNum, 
      lRecord.channelId, lRecord.currentEvent, lRecord.msisdn, lRecord.limits.packageType);

   //Set the CDR parameters
   lRecord.cdr.disconnectTime = time(NULL);
   lRecord.cdr.stopTime = time(NULL); 
   if(lRecord.cdr.answerTime > 0)
      lRecord.cdr.duration = (UINT32)difftime(lRecord.cdr.disconnectTime, lRecord.cdr.answerTime);
   else
      lRecord.cdr.duration = 0;
   lRecord.cdr.chargeUnit = CHARGE_UNIT_NOM;
   lRecord.cdr.unitCharged = lRecord.limits.usage;

   // Reset Timer Tag
   lRecord.timerTag = 0;

   if((lRecord.limits.subscriptionId == 0) || (lRecord.limits.usage == 0))
   {
      TERR(gTrace, printf("%s: %s No Valid Package Subscription Found - ProcessTimerExpiry\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM03, lLogText);
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) USAGE:%d", lRecord.referenceNum, 
      lRecord.channelId, lRecord.currentEvent, lRecord.msisdn, lRecord.limits.packageType,
      lRecord.packageInfo.packageId, lRecord.packageInfo.subscriptionId, lRecord.limits.usage);
   T(gTrace, printf("%s: %s Package Subscription Found\n", gProcessName, lLogText););

   if(mCfgObj.mAllLogsRequired)
      gLog.GenerateLog(OCM04, lLogText);
   
   if(false == this->ResetBillingRecord(lRecord.channelId, lRecord))
   {
      T(gTrace, printf("%s: %s Event Processing Failed - SHM Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM06, lLogText);

      response.status = BILLING_SYSTEM_FAILURE;
      //return this->SendBillingResponse(response);
   }

   //First send the Billing Response to application, so call can be released immediately.
   //this->SendBillingResponse(response);
   this->SendReleaseCallSignal(lRecord.channelId);

   snprintf(lLogText, MAX_LOG_TEXT_LEN,"RefNum:%08X CID:%05d BEVT:%d MSISDN:%s PKGT:%d PID:%d(%d) UNIT_CHARGED:%d", lRecord.referenceNum, 
      lRecord.channelId, lRecord.currentEvent, lRecord.msisdn, lRecord.limits.packageType,
      lRecord.packageInfo.packageId, lRecord.packageInfo.subscriptionId, lRecord.cdr.unitCharged);

   if(false == this->ChargeFromPackage(lRecord.packageInfo.subscriptionId, lRecord.limits.packageType, lRecord.cdr.unitCharged, lRecord.packageInfo))
   {
      T(gTrace, printf("%s: %s Charging From Package Failed - DB Error\n", gProcessName, lLogText););
      gLog.GenerateLog(OCM08, lLogText);
      response.status = BILLING_SYSTEM_FAILURE;
   }
   else
   {
      T(gTrace, printf("%s: %s Charging Success\n", gProcessName, lLogText););
      response.status = BILLING_TIMEOUT;
   }
   T(gTrace, printf("%s: %s Event Processed\n", gProcessName, lLogText););
   gLog.GenerateLog(OCM05, lLogText);

   // Generate Final CDR
   return this->GenerateCdr(lRecord);

   //return this->SendBillingResponse(response);
}
//----------------------------------------------------------------------
// METHOD      : ProcessUnknownEvent
// DESCRIPTION :
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpPackageBilling::ProcessUnknownEvent(BillingRecordDetails &record, BillingResponse &response)
{
   //TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //BillingLimits lLimits;

   return false;
}
