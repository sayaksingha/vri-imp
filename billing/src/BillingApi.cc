static const char *id = "@(#) $Id: BillingApi.cc,v 1.1.4.3.2.1 2016/05/13 02:14:33 mram Exp $";
//----------------------------------------------------------------------
// NAME:
// BillingApi.cc
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
// Originated : KOTHARI KOMAL                                16-JAN-2012
//----------------------------------------------------------------------
#include <stdio.h>
#include "BillingApi.h"
#include "BillingUtil.h"
#include "IvrConfigApi.h"

CTrace gBillingTrace (TRACE_ONLINE_CHARGING_ENV, false);

extern TEXT gProcessName[];
extern Log  gLog;

IvrDbManager gBillingApiDbManager;
IvrConfigApi gBillingApiCfgObj;

//----------------------------------------------------------------------
// METHOD      : GetVersion
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None 
//----------------------------------------------------------------------
const char* BillingApi::GetVersion()
{
   return id;
}
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None 
//----------------------------------------------------------------------
BillingApi::BillingApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BillingApi::~BillingApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingApi::Init (UINT8 appId)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	if (0 == appId)
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN, "Invalid Application Id.");
		TERR (gBillingTrace,
				printf ("%s:BillingApi: %s. Exiting...\n", gProcessName, lLogText););
		gLog.GenerateLog (GSYS04, lLogText);
		return false;
	}

	mAppId = appId;   

	// Read the configuration
	if (false == this->ReadConfig ())
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN, "BillingApi ReadConfig failed.");
		TERR (gBillingTrace,
				printf ("%s:BillingApi: %s. Exiting...\n", gProcessName, lLogText););
		gLog.GenerateLog (GSYS04, lLogText);
		return false;
	}
   // Read the Configuration...
   if (false == gBillingApiCfgObj.Init(CONFIG_IVR_IPC))
   {
      sprintf (lLogText, "Initilising the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: IPC Configuration Read - success..\n", gProcessName););

   if(DB_NO_ERR != gBillingApiDbManager.Init(gBillingApiCfgObj.mDbClient))  // 1 - MySql DB Client , 0 - Oracle DB Client
   {
      sprintf(lLogText, "DataBase Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   // Read the Configuration...
   if (false == gBillingApiCfgObj.Init(gBillingApiDbManager, CONFIG_IVR_BILLING))
   {
      sprintf (lLogText, "Initilising the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Billing Configuration Read - success..\n", gProcessName););

   gBillingApiDbManager.Disconnect();

   // Create IPC
   if (false == this->CreateIPC ())
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "BillingApi CreateIPC failed");
      TERR (gBillingTrace,
            printf ("%s:BillingApi: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingApi::ReadConfig ()
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	TEXT lCfgKey[MAX_LOG_TEXT_LEN + 1] = "";
	CfgRead lCfgRead (gProcessName, "PRODUCT_HOME", "PRODUCT_CFG_PATH");

	if (true != lCfgRead.CfgInit (IVR_IPC_CFG))
	{
		TERR (gBillingTrace, printf ("%s:BillingApi: Could not open file %s\n",
					gProcessName, IVR_IPC_CFG););
		gLog.GenerateLog (GSYS09,(TEXT*) IVR_IPC_CFG);
		return false;
	}

	//snprintf (lCfgKey, MAX_LOG_TEXT_LEN, "MSG_BILLING_APP_%d_RSP_QUE", mAppId);
	snprintf (lCfgKey, MAX_LOG_TEXT_LEN, "MSG_BILLING_RSP_QUE");

	if (CFG_OK != lCfgRead.GetConfigNum (lCfgKey, mBillingRspQKey,
				IVR_MIN_MSG_Q_KEY, BILLING_MAX_MSG_Q_KEY))
	{
		sprintf (lLogText, "%s. File:%s", lCfgKey, IVR_IPC_CFG);
		TERR (gBillingTrace, printf ("%s:BillingApi: Config read failed for %s \n",
					gProcessName, lLogText););
		lCfgRead.CfgDeInit ();
		return false;
	}
	else
	{
		T (gBillingTrace, printf ("%s:BillingApi: %s = %d\n",
					gProcessName, lCfgKey, mBillingRspQKey););
	}

	// Read Request Queue key
	if (CFG_OK !=
			lCfgRead.GetConfigNum ("MSG_BILLING_REQ_QUE", mBillingReqQKey,
				IVR_MIN_MSG_Q_KEY, BILLING_MAX_MSG_Q_KEY))
	{
		sprintf (lLogText, "MSG_BILLING_REQ_QUE. File:%s", IVR_IPC_CFG);
		TERR (gBillingTrace, printf ("%s:BillingApi: Config read failed for %s \n",
					gProcessName, lLogText););
		lCfgRead.CfgDeInit ();
		return false;
	}
	else
	{
		T (gBillingTrace, printf ("%s:BillingApi: MSG_BILLING_REQ_QUE = %d\n",
					gProcessName, mBillingReqQKey););
	}

	return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateMsgQueue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
msgQueueError BillingApi::CreateMsgQueue (MsgQueue & queue, const key_t key)
{
	return queue.Create (key, IPC_CREAT | IPC_PERM);
}

//----------------------------------------------------------------------
// METHOD      : CreateIPC
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingApi::CreateIPC ()
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	msgQueueError lQueueError;

	lQueueError = this->CreateMsgQueue (mBillingReqQue, mBillingReqQKey);
	if (Q_SUCCESS != lQueueError)
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN,
				"Error creating MSG_BILLING_REQ_QUE. QueKey:%d Error:%s", 
				mBillingReqQKey, gLog.ConvertQueErrToStr (lQueueError));
		TERR (gBillingTrace, printf ("%s:BillingApi: %s\n", gProcessName, lLogText););
		gLog.GenerateLog (GSYS06, lLogText);
		return false;
	}

	lQueueError = this->CreateMsgQueue (mBillingRspQue, mBillingRspQKey);
	if (Q_SUCCESS != lQueueError)
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN,
				"Error creating MSG_BILLING_RSP_QUE. QueKey:%d Error:%s", 
				 mBillingRspQKey, gLog.ConvertQueErrToStr (lQueueError));
				//"Error creating MSG_BILLING_APP_%d_RSP_QUE. QueKey:%d Error:%s", 
		TERR (gBillingTrace, printf ("%s:BillingApi: %s\n", gProcessName, lLogText););
		gLog.GenerateLog (GSYS06, lLogText);
		return false;
	}

	return true;
}

//------------------------------------------------------------------------
// METHOD      : HandleQueueReadError
// DESCRIPTION : Handle Queue Read Error
// PARAMETER   :
// RETURN      : BOOLEAN
//------------------------------------------------------------------------
BOOLEAN BillingApi::HandleQueueReadError (msgQueueError err, MsgQueue & queue)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	key_t key = queue.GetQueueKey ();

	switch (err)
	{
		case Q_MSG_LEN_ERR:
			snprintf (lLogText, MAX_LOG_TEXT_LEN,
					"Q_MSG_LEN_ERR occured while reading from queue:%d", key);
			break;

		case Q_READ_PERM_ERR:
			snprintf (lLogText, MAX_LOG_TEXT_LEN,
					"Q_READ_PERM_ERR occured while reading from queue:%d", key);
			break;

		case Q_READ_SIGNAL_ERR:
			return false;

		case Q_READ_BUF_ADDR_ERR:
			snprintf (lLogText, MAX_LOG_TEXT_LEN,
					"Q_READ_BUF_ADDR_ERR occured while reading from queue:%d",
					key);
			break;

		case Q_REMOVED_ERR:
		case Q_READ_INVALID_ID:
			if (Q_SUCCESS != this->CreateMsgQueue (queue, key))
				snprintf (lLogText, MAX_LOG_TEXT_LEN,
						"Q_READ_INVALID_ID: ReCreateMsgQueue Failed. QueKey:%d", key);
			else
				snprintf (lLogText, MAX_LOG_TEXT_LEN,
						"Q_READ_INVALID_ID: ReCreateMsgQueue Success. QueKey:%d", key);
			break;

		case Q_NO_MSG_WITH_TYPE:
			return false;

		case Q_READ_UNKNOWN_ERR:
		default:
			snprintf (lLogText, MAX_LOG_TEXT_LEN,
					"Q_READ_UNKNOWN_ERR(%d) occured while reading from queue:%d",
					err, key);
			break;
	}

	TERR (gBillingTrace, printf ("%s: %s\n", gProcessName, lLogText););
	gLog.GenerateLog (GSYS07, lLogText);

	return false;
}

//---------------------------------------------------------------------
// METHOD      : ProcessBillingRequest
// DESCRIPTION : Common API for Applications to Write and Read IN request and response
// PARAMETER   : BillingRequest &rBillingRequest, RspDetails &rspDetails, UINT16 timeOut
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingApi::ProcessBillingRequest (BillingRequest & rBillingRequest,
		BillingResponse & rBillingResponse)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	snprintf (lLogText, MAX_LOG_TEXT_LEN,
			"RefNum:%08X Msisdn:%s Event:%d(%s) SubsType:%d(%s) ChargeId:%d Expiry:%d",
         rBillingRequest.referenceNum,
			rBillingRequest.msisdn, rBillingRequest.event,
			BillingUtil::ConvertBillingEventTypeToStr (rBillingRequest.event),
			rBillingRequest.subsType,
			BillingUtil::ConvertSubsTypeToStr (rBillingRequest.subsType),
			rBillingRequest.chargeId, rBillingRequest.expiryTime);

	T (gBillingTrace,
			printf ("%s:BillingApi: %s\n", gProcessName, lLogText););
	gLog.GenerateLog (OCM21, 1, lLogText);

	memset (&rBillingResponse, 0, sizeof (rBillingResponse));

   if (0 == gBillingApiCfgObj.mChargeFunction)
   {
      rBillingResponse.event = rBillingRequest.event;
      rBillingResponse.referenceNum = rBillingRequest.referenceNum;
      rBillingResponse.status = BILLING_DISABLED;
      strcpy (rBillingResponse.msisdn, rBillingRequest.msisdn);

      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X Msisdn:%s Operation:%d(%s) RspStatus:%d(%s)",
            rBillingResponse.referenceNum,
            rBillingResponse.msisdn, rBillingResponse.event, 
            BillingUtil::ConvertBillingEventTypeToStr (rBillingResponse.event),
            rBillingResponse.status,
            BillingUtil::ConvertBillingStatusToStr (rBillingResponse.status));
      T (gBillingTrace, printf ("%s:BillingApi: %s\n",
               gProcessName, lLogText););
      gLog.GenerateLog (OCM19, lLogText);
      return true;
   }

	if(true == this->ReceiveBillingResponse (rBillingResponse, rBillingRequest.referenceNum))
	{
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X DELAY RSP: Msisdn:%s Operation:%d(%s) RspStatus:%d(%s) RelCause:%d",
            rBillingResponse.referenceNum,
            rBillingResponse.msisdn, rBillingResponse.event, 
            BillingUtil::ConvertBillingEventTypeToStr (rBillingResponse.event),
            rBillingResponse.status,
            BillingUtil::ConvertBillingStatusToStr (rBillingResponse.status),
            rBillingResponse.relCause);

      TERR (gBillingTrace,
            printf ("%s: %s Previous Response Received from Billing Modulegw\n", gProcessName, lLogText););
      gLog.GenerateLog (OCM20, lLogText);
   }

   memset (&rBillingResponse, 0, sizeof (rBillingResponse));

	if (0 == rBillingRequest.expiryTime)
		rBillingRequest.expiryTime = time (NULL) + BILLING_DEFAULT_EXPIRY_TIME;
	else if (rBillingRequest.expiryTime < BILLING_MAX_EXPIRY_TIME)
		rBillingRequest.expiryTime = time (NULL) + rBillingRequest.expiryTime;

	time_t lExpiryTime = rBillingRequest.expiryTime;

   if (this->SubmitBillingRequest (rBillingRequest))
   {
      while ((time (NULL) <= lExpiryTime))
      {
         usleep (100);

         if (this->ReceiveBillingResponse (rBillingResponse, rBillingRequest.referenceNum))
         {
            if ((BILLING_SUCCESS == rBillingResponse.status) || (BILLING_DISABLED == rBillingResponse.status))
               return true;
            else
               return false;
         }
      }
   }

   rBillingResponse.event = rBillingRequest.event;
   rBillingResponse.referenceNum = rBillingRequest.referenceNum;
   rBillingResponse.status = BILLING_TIMEOUT;

   strcpy (rBillingResponse.msisdn, rBillingRequest.msisdn);

   TERR (gBillingTrace, printf ("%s: %s Request Timed Out\n",
            gProcessName, lLogText););
   gLog.GenerateLog (OCM22, lLogText);

   return false;
}

//----------------------------------------------------------------------
// METHOD      : SubmitBillingRequest
// DESCRIPTION : common process to write request for charging gateway by application
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingApi::SubmitBillingRequest (BillingRequest & rBillingRequest)
{
   msgQueueError lQueueErr;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (rBillingRequest);
   int   lMsgType = rBillingRequest.referenceNum;

   rBillingRequest.appId = mAppId;

	if (0 == rBillingRequest.expiryTime)
		rBillingRequest.expiryTime = time (NULL) + BILLING_DEFAULT_EXPIRY_TIME;
	else if (rBillingRequest.expiryTime < BILLING_MAX_EXPIRY_TIME)
		rBillingRequest.expiryTime = time (NULL) + rBillingRequest.expiryTime;

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X Msisdn:%s Event:%d(%s) SubsType:%d(%s) ChargeId:%d Expiry:%d SysTime:%d",
         rBillingRequest.referenceNum,
         rBillingRequest.msisdn, rBillingRequest.event, 
         BillingUtil::ConvertBillingEventTypeToStr (rBillingRequest.event),
         rBillingRequest.subsType,
         BillingUtil::ConvertSubsTypeToStr (rBillingRequest.subsType),
         rBillingRequest.chargeId, rBillingRequest.expiryTime, time (NULL));

   lQueueErr = mBillingReqQue.WriteMsg (&rBillingRequest, lMsgLen, lMsgType, BLOCKING);
   if (Q_SUCCESS != lQueueErr)
   {
      lMsgType = rBillingRequest.referenceNum;
      if (false ==IvrUtil::HandleQueueWriteError (lQueueErr,
               mBillingReqQue, &rBillingRequest, lMsgLen, lMsgType, BLOCKING))
      {
         TERR (gBillingTrace, printf ("%s:BillingApi: %s Writing Request to Billing Queue Failed\n",
                  gProcessName, lLogText););
         gLog.GenerateLog (OCM24, lLogText);
         return false;
      }
   }

   T (gBillingTrace,
         printf ("%s:BillingApi: %s Writing Request to Billing Queue Success\n",
            gProcessName, lLogText););
   gLog.GenerateLog (OCM23, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReceiveBillingResponse
// DESCRIPTION : common process to receiveresponse for charging gateway by IN
// PARAMETER   : BillingResponse &rBillingResponse
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingApi::ReceiveBillingResponse (BillingResponse & rBillingResponse, int msgType)
{
   INT16 lMsgLen = sizeof (BillingResponse);
   msgQueueError lQueueErr;


   if (Q_SUCCESS != (lQueueErr = mBillingRspQue.ReadMsg (&rBillingResponse,
               lMsgLen, msgType, NON_BLOCKING)))
   {
      this->HandleQueueReadError (lQueueErr, mBillingRspQue);
      return false;
   }

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X Msisdn:%s Event:%d(%s) RspStatus:%d(%s) RelCause:%d AppId:%d",
         rBillingResponse.referenceNum,
         rBillingResponse.msisdn, rBillingResponse.event, 
         BillingUtil::ConvertBillingEventTypeToStr(rBillingResponse.event),
         rBillingResponse.status,
         BillingUtil::ConvertBillingStatusToStr (rBillingResponse.status),
         rBillingResponse.relCause, mAppId);

   TERR (gBillingTrace,
         printf ("%s: %s Response Received from Billing Server\n", gProcessName, lLogText););
   gLog.GenerateLog (OCM25, lLogText);

   return true;
}
