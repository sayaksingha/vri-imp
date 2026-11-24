static const char *id = "@(#) $Id: BillingServer.cc,v 1.1.4.3 2016/04/12 21:59:00 shijuc Exp $";
//----------------------------------------------------------------------
// NAME:
// BillingServer.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright (C) 2006, 
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd.
//
// DESCRIPTION
// Common lirary to be used by application to read and write billing 
// request to Online Charging module.
//
// Originated : KOTHARI KOMAL                                21-APR-2012
//----------------------------------------------------------------------
#include <stdio.h>
#include "BillingServer.h"

CTrace gBillingTrace (TRACE_ONLINE_CHARGING_ENV);

extern TEXT gProcessName[];
extern Log  gLog;
extern unsigned int       gReferenceNum;

//----------------------------------------------------------------------
// METHOD      : GetVersion
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None 
//----------------------------------------------------------------------
const char* BillingServer::GetVersion()
{
   return id;
}
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None 
//----------------------------------------------------------------------
BillingServer::BillingServer ()
{
}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BillingServer::~BillingServer ()
{
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::Init ()
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	// Initialize the QueAPI
	if (false == mQueApi.Init())
	{
	   sprintf (lLogText, "Initilising QueAPI - failed");
	   TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
	   gLog.GenerateLog (GSYS04, lLogText);
	   return false;
	}
	T(gTrace, printf ("%s: QueAPI Initilization - success..\n", gProcessName););
	
   // Read the Configuration...
   if (false == mCfgObj.Init(CONFIG_IVR_IPC))
   {
      sprintf (lLogText, "Initilising the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Configuration Initilization success..\n", gProcessName););

   if(DB_NO_ERR != mDbManager.Init(mCfgObj.mDbClient))  // 1 - MySql DB Client , 0 - Oracle DB Client
   {
      sprintf(lLogText, "DataBase Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }


   // Read the Configuration...
   if (false == mCfgObj.Init(mDbManager, CONFIG_IVR_BILLING))
   {
      sprintf (lLogText, "Initilising the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Configuration Initilization success..\n", gProcessName););


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
	// ShmKey startDlg	EndDlg
	if (false == mBillingRecordShm.Init(mCfgObj.mShmBillingRecordKey, IVR_MIN_CHANNEL_ID, IVR_MAX_CHANNEL_ID))
	{
	   sprintf (lLogText, "Initilising the Billing Record Detail Table failed");
	   TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
	   gLog.GenerateLog (GSYS04, lLogText);
	   return false;
	}
	T(gTrace, printf ("%s: Billing Record Detail Table Initilization success..\n", gProcessName););
	
	// store the Shm Pointer
	// mpBillingRecord= mBillingRecordShm.GetShmPointer();

   if(mCfgObj.mIsMmpEnabled)
   {
      if(false == mPackageBilling.Init())
      {
         sprintf (lLogText, "MMP Package Billing Object initialisation - failed");
         TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
         gLog.GenerateLog (GSYS04, lLogText);
         return false;
      }
   }

	return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::ReadConfig ()
{
	//TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	return true;
}

//----------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::ReloadConfig ()
{
	//TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   return mCfgObj.ReloadConfig(mDbManager);
}

//---------------------------------------------------------------------
// METHOD      : ProcessBillingRequest
// DESCRIPTION : Common API for Applications to Write and Read IN request and response
// PARAMETER   : BillingRequest &rBillingRequest, RspDetails &rspDetails, UINT16 timeOut
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::ReadBillingRequest(BillingRequest & billingRequest)
{
	//TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   return mQueApi.ReadFromBillingRequestQue(billingRequest);
}

//---------------------------------------------------------------------
// METHOD      : ProcessBillingRequest
// DESCRIPTION : Common API for Applications to Write and Read IN request and response
// PARAMETER   : BillingRequest &rBillingRequest, RspDetails &rspDetails, UINT16 timeOut
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::ProcessBillingRequest (BillingRequest & rBillingRequest)
{
	TEXT                 lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BillingRecordDetails lRecord;
   BillingResponse      lBillingResponse;

   gReferenceNum = rBillingRequest.referenceNum;
   // 1. Log the Billing Request Details
	snprintf (lLogText, MAX_LOG_TEXT_LEN,
			"RefNum:%08X CID:%05d Msisdn:%s Event:%d(%s) SubsType:%d(%s) ContentId:%d Expiry:%d",
			rBillingRequest.referenceNum, rBillingRequest.channelId, rBillingRequest.msisdn, rBillingRequest.event,
			BillingUtil::ConvertBillingEventTypeToStr (rBillingRequest.event),
			rBillingRequest.subsType,
			BillingUtil::ConvertSubsTypeToStr (rBillingRequest.subsType),
			rBillingRequest.contentId, rBillingRequest.expiryTime);

	T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
	gLog.GenerateLog (OCM01, 1, lLogText);

   // 2. Cleanup the Old responses, if any for the same reference number
/*	memset (&lBillingResponse, 0, sizeof (BillingResponse));

	if(true == this->ReceiveBillingResponse (lBillingResponse, rBillingRequest.referenceNum))
	{
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "DELAY RSP: Msisdn:%s Operation:%d(%s) RefNo:%08X RspStatus:%d(%s) RelCause:%ld",
            lBillingResponse.msisdn, lBillingResponse.event, 
            BillingUtil::ConvertBillingEventTypeToStr (lBillingResponse.event),
            lBillingResponse.referenceNum, lBillingResponse.status,
            BillingUtil::ConvertBillingStatusToStr (lBillingResponse.status),
            lBillingResponse.relCause);

      TERR (gBillingTrace,
            printf ("%s: %s Previous Response Received from Billing Module GW\n", gProcessName, lLogText););
      gLog.GenerateLog (OCM02, lLogText);
   }
*/

   // 3. Populate the expiry time for the billing request
   memset (&lBillingResponse, 0, sizeof (lBillingResponse));

	if (0 == rBillingRequest.expiryTime)
		rBillingRequest.expiryTime = time (NULL) + BILLING_DEFAULT_EXPIRY_TIME;
	else if (rBillingRequest.expiryTime < BILLING_MAX_EXPIRY_TIME)
		rBillingRequest.expiryTime = time (NULL) + rBillingRequest.expiryTime;

   lBillingResponse.appId = rBillingRequest.appId;
   lBillingResponse.event = rBillingRequest.event;
   strcpy(lBillingResponse.msisdn, rBillingRequest.msisdn);
   lBillingResponse.referenceNum = rBillingRequest.referenceNum;
   lBillingResponse.refId = rBillingRequest.channelId;
   lBillingResponse.mode = CHARGE_MODE_UNKNOWN;
   lBillingResponse.relCause = 0;
   lBillingResponse.status = BILLING_UNKNOWN;

	//time_t lExpiryTime = rBillingRequest.expiryTime;

   // 4. Populate or retrieve the Billing Record details from request
   if(false == this->GetBillingRecord(rBillingRequest, lRecord))
   {
      return this->SendBillingResponse(lBillingResponse); 
   }
  
   if (false == this->IsChargeFunctionEnabled(lRecord))
   {
      //Charge Function disabled
      // Send Billng Response to Application
      T(gTrace, printf("%s: %s Charging Function Disabled\n", gProcessName, lLogText););
      lBillingResponse.status = BILLING_DISABLED;
      return this->SendBillingResponse(lBillingResponse); 
   }

   lBillingResponse.mode = lRecord.chargeMode;
   T(gTrace, printf("%s: %s Charging Mode: %d\n", gProcessName, lLogText, lBillingResponse.mode););

   switch(lRecord.chargeMode)
   {
      case CHARGE_MODE_PACKAGE:
         if(false == mPackageBilling.ProcessBillingEvent(lRecord, lBillingResponse))
         {
            // Event Processing failed - Generate Log
            // Send proper BillingResponse
         }

         break;
      case CHARGE_MODE_SCP:
         break;
      default:
         break;
   }

   return this->SendBillingResponse(lBillingResponse); 
}

//----------------------------------------------------------------------
// METHOD      : SendBillingResponse
// DESCRIPTION : common process to receiveresponse for charging gateway by IN
// PARAMETER   : BillingResponse &rBillingResponse
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::SendBillingResponse (BillingResponse & rBillingResponse)
{
   return mQueApi.WriteToBillingResponseQue(rBillingResponse);
}

//----------------------------------------------------------------------
// METHOD      : ReceiveBillingResponse
// DESCRIPTION : common process to receiveresponse for charging gateway by IN
// PARAMETER   : BillingResponse &rBillingResponse
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::ReceiveBillingResponse (BillingResponse & rBillingResponse, int msgType)
{
   return mQueApi.ReadFromBillingResponseQue(rBillingResponse, msgType);
}

//----------------------------------------------------------------------
// METHOD      : StoreBillingRecord
// DESCRIPTION : Store Billing Record details in to shared memory
// PARAMETER   : BillingRecordDetails &record, INT32 index
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::StoreBillingRecord(BillingRecordDetails &record, INT32 index)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

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

   return lRetVal;
}

//----------------------------------------------------------------------
// METHOD      : GetBillingRecord
// DESCRIPTION : Retrieve Billing Record details from shared memory
// PARAMETER   : BillingRecordDetails &record, INT32 index
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::GetBillingRecord(INT32 index, BillingRecordDetails &record)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

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


   return lRetVal;
}

//----------------------------------------------------------------------
// METHOD      : GetBillingRecord
// DESCRIPTION : Retrieve Billing Record details from shared memory or 
//                populate BillingRecord details from billing request.
// PARAMETER   : BillingRequest req, BillingRecordDetails &record
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::GetBillingRecord(BillingRequest req, BillingRecordDetails &record)
{
	TEXT    lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //BOOLEAN lRetVal = false;

   if((BILLING_CALL_RECEIVED == req.event) ||( BILLING_CONTENT_PLAY == req.event) || (BILLING_OFFLINE_DEDICATION == req.event))
   {
      // New Billing Request, need to populate Billing Record Details.
      T(gTrace, printf("%s: New Billing Request, need to populate Billing Record Details.\n", gProcessName););

      if(record.currentEvent != BILLING_UNKNOWN_EVENT)
      {
         //Dump the BillingRecord Details info in discrepency file
         this->DumpToDiscrepencyFile(record);
      }

      memset(&record, 0, sizeof(BillingRecordDetails));

      record.appId = req.appId;
      record.channelId = req.channelId;
      record.contentId = req.contentId;
      record.initialEvent = req.event;
      record.lastEvent = BILLING_UNKNOWN_EVENT;
      record.currentEvent = req.event;
      record.referenceNum = req.referenceNum;

      strncpy(record.msisdn, req.msisdn, MAX_MSISDN_LEN);
      strncpy(record.shortCode, req.shortCode, MAX_MSISDN_LEN);
      strncpy(record.imsi, req.imsi, MAX_MSISDN_LEN);

      record.subsType = req.subsType;
      
      record.chargeMode = req.mode;
      record.chargeId = req.chargeId;

      strncpy(record.purpose, req.purpose, MAX_PURPOSE_LEN);
      record.expiryTime = req.expiryTime;

      // Get the Charging Function
      record.chargeFunction = this->GetChargeFunction(req);

      //if(false == this->IsChargeFunctionEnabled(record))
      //{
      //   // Charge function disabled
      //   return false;
      //}

      // Retrieve & override the charge mode (SCP/Package)
      record.chargeMode = this->GetChargeMode(req);
   }
   else
   {
      if(false == this->GetBillingRecord(req.channelId, record))
      {  
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%5d", gReferenceNum, req.channelId);
         TERR(gTrace, printf("%s: %s Invalid Channel Id\n", gProcessName, lLogText););
         gLog.GenerateLog(OCM16, lLogText);
         return false;
      }

      record.lastEvent = record.currentEvent;
      record.currentEvent = req.event;
   }

   if(BILLING_OFFLINE_DEDICATION != req.event)
      return this->StoreBillingRecord(record, req.channelId);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : IsChargeFunctionEnabled
// DESCRIPTION : Check whether ChargeFunction is enabled for received 
//                event or not.
// PARAMETER   : BillingRequest req
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::IsChargeFunctionEnabled(BillingRecordDetails record)
{
   if((CHARGE_FUNCTION_DURATION == record.chargeFunction)||
         (CHARGE_FUNCTION_CONTENT == record.chargeFunction))
      return true;
   else
      return false;
}

//----------------------------------------------------------------------
// METHOD      : GetChargeFunction
// DESCRIPTION : Check whether ChargeFunction is enabled for received 
//                event or not.
// PARAMETER   : BillingRequest req
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
EnumChargeFunction BillingServer::GetChargeFunction(BillingRequest req)
{
   EnumChargeFunction lChargeFunction = CHARGE_FUNCTION_NONE;

   if(CHARGE_FUNCTION_DURATION == (EnumChargeFunction)mCfgObj.mChargeFunction)
   {
      switch(req.event)
      {
         case BILLING_CALL_RECEIVED:
         case BILLING_CALL_ANSEWERED:
         case BILLING_CALL_DISCONNECTED:
         case BILLING_OFFLINE_DEDICATION:
         case BILLING_OFFLINE:
            lChargeFunction = CHARGE_FUNCTION_DURATION;
            break;
         default:
            lChargeFunction = CHARGE_FUNCTION_NONE;
      }
   }
   else if (CHARGE_FUNCTION_CONTENT == (EnumChargeFunction)mCfgObj.mChargeFunction)
   {
      switch(req.event)
      {
         case BILLING_CONTENT_PLAY:
         case BILLING_CONTENT_START:
         case BILLING_CONTENT_STOP:
         case BILLING_OFFLINE_DEDICATION:
         case BILLING_OFFLINE:
            lChargeFunction = CHARGE_FUNCTION_CONTENT;
            break;
         default:
            lChargeFunction = CHARGE_FUNCTION_NONE;
      }
   }

   return lChargeFunction;
}

//----------------------------------------------------------------------
// METHOD      : GetChargeMode
// DESCRIPTION : Retrieve the ChargeMode the request 
// PARAMETER   : BillingRequest req
// RETURN      : EnumChargeMode
//----------------------------------------------------------------------
EnumChargeMode BillingServer::GetChargeMode(BillingRequest req)
{
   EnumChargeMode lChargeMode = CHARGE_MODE_SCP;

   if(mCfgObj.mIsMmpEnabled)
   {
      if(CHARGE_MODE_SCP == req.mode)
         lChargeMode = CHARGE_MODE_SCP;
      else 
         lChargeMode = CHARGE_MODE_PACKAGE;
   }

   return lChargeMode;
}

//----------------------------------------------------------------------
// METHOD      : DumpToDiscrepencyFile
// DESCRIPTION : Dump the record to discrepency file for further analysis
// PARAMETER   : BillingRecordDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingServer::DumpToDiscrepencyFile(BillingRecordDetails &record)
{
   return true;
}
