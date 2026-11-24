static const char* id="@(#) $Id: IvrQueApi.cc,v 1.1.1.1.2.7.2.2 2016/07/05 00:07:21 mram Exp $" ;
//----------------------------------------------------------------------
// NAME
// IvrQueApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2010
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated :		  									    	  DATE:25-OCT-2010
//----------------------------------------------------------------------
#include "IvrQueApi.h"

extern unsigned int gReferenceNum;
extern TEXT  gLogText[];
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
IvrQueApi::IvrQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
IvrQueApi::~IvrQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::Init ()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Read Configuration
   if (false == this->ReadConfig ())
   {
      strncpy (lLogText, "ReadConfig failed, Exiting ....", MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   // Creating Queues
   if (false == this->CreateQueues ())
   {
      strncpy (lLogText, "CreateQueue failed, Exiting ....",
            MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Read message queue key values
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

	/*
	if (CFG_OK !=
			lCfgRead.GetConfigNum ("MSG_CCXML_RES_Q_RCV", mCcxmlRspQueKey,
				IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN, 
				"Config read failed for %s. File:%s.",
				"MSG_CCXML_RES_Q_RCV", IVR_IPC_CFG);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_CCXML_RES_Q_RCV = %d\n",
               gProcessName, mCcxmlRspQueKey););
   }
*/
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

	/*
   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_VXML_RES_Q_RCV", mVxmlRspQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {
		snprintf (lLogText, MAX_LOG_TEXT_LEN, 
				"Config read failed for %s. File:%s.",
				"MSG_VXML_RES_Q_RCV", IVR_IPC_CFG);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		lCfgRead.CfgDeInit ();
		return false;
   }
   
	T (gTrace, printf ("%s: MSG_VXML_RES_Q_RCV = %d\n",
            gProcessName, mVxmlRspQueKey););
*/
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
         lCfgRead.GetConfigNum ("MSG_SCHEDULE_RSP_Q_RCV", mScheduleRspQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {
		snprintf (lLogText, MAX_LOG_TEXT_LEN, 
				"Config read failed for %s. File:%s.",
				"MSG_SCHEDULE_RSP_Q_RCV", IVR_IPC_CFG);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		lCfgRead.CfgDeInit ();
		return false;
   }
   
	T (gTrace, printf ("%s: MSG_SCHEDULE_RSP_Q_RCV = %d\n",
            gProcessName, mScheduleRspQueKey););

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_ISUP_CKT_CMD_Q_RCV", mCktCmdReqQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {
		snprintf (lLogText, MAX_LOG_TEXT_LEN, 
				"Config read failed for %s. File:%s.",
				"MSG_ISUP_CKT_CMD_Q_RCV", IVR_IPC_CFG);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		lCfgRead.CfgDeInit ();
		return false;
   }
   
	T (gTrace, printf ("%s: MSG_ISUP_CKT_CMD_Q_RCV = %d\n",
            gProcessName, mCktCmdReqQueKey););

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_ISUP_CKT_CMD_RSP_Q_RCV", mCktCmdRspQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {
		snprintf (lLogText, MAX_LOG_TEXT_LEN, 
				"Config read failed for %s. File:%s.",
				"MSG_ISUP_CKT_CMD_RSP_Q_RCV", IVR_IPC_CFG);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		lCfgRead.CfgDeInit ();
		return false;
   }
   
	T (gTrace, printf ("%s: MSG_ISUP_CKT_CMD_RSP_Q_RCV = %d\n",
            gProcessName, mCktCmdRspQueKey););

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_BILLING_RSP_QUE", mBillingRspQKey,
            IVR_MIN_MSG_Q_KEY, BILLING_MAX_MSG_Q_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_BILLING_RSP_QUE", IVR_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_BILLING_RSP_QUE = %d\n",
               gProcessName, mBillingRspQKey););
   }

   // Read Request Queue key
   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_BILLING_REQ_QUE", mBillingReqQKey,
            IVR_MIN_MSG_Q_KEY, BILLING_MAX_MSG_Q_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_BILLING_REQ_QUE", IVR_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_BILLING_REQ_QUE = %d\n",
               gProcessName, mBillingReqQKey););
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_REQ_Q",
            mMpReqQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {  
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_VOICE_PROCESS_REQ_Q",IVR_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););       
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {

      T (gTrace, printf ("%s: MSG_VOICE_PROCESS_REQ_Q = %d\n",
               gProcessName, mMpReqQueKey););
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_RSP_Q",
            mMpRspQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {  
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_VOICE_PROCESS_RSP_Q",IVR_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););                          
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_VOICE_PROCESS_RSP_Q = %d\n",
               gProcessName, mMpRspQueKey););
   }

   lCfgRead.CfgDeInit ();

   if (lCfgRead.CfgInit (IVR_PRODUCT_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, IVR_PRODUCT_CFG););
      gLog.GenerateLog (GSYS09,(TEXT*) IVR_PRODUCT_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("PEGS_REQD",
            mPegRequired, 0, 1))
   {  
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "PEGS_REQD", IVR_PRODUCT_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););                          
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: PEGS_REQD = %d\n",
               gProcessName, mPegRequired););
   }

   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateQueues
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::CreateQueues ()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   msgQueueError lQueErr;

   // Creating CCXML Req Queue
   lQueErr = mCcxmlReqQue.Create (mCcxmlReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mCcxmlReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CCXML_REQ_Q_RCV",
               mCcxmlReqQueKey););
      return false;
   }

   // Creating CCXML Rsp Queue
   T(gTrace, printf("%s: CCXMLRSP_Q_RCV = %d\n", gProcessName, mCcxmlRspQueKey););
   lQueErr = mCcxmlRspQue.Create (mCcxmlRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mCcxmlRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CALL_ALERT_Q_RCV",
               mCcxmlRspQueKey););
      return false;
   }

   // Creating VXML Req Queue
   lQueErr = mVxmlReqQue.Create (mVxmlReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mVxmlReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VXML_REQ_Q_RCV",
               mVxmlReqQueKey););
      return false;
   }

   // Creating VXML Child Req Queue
   lQueErr = mVxmlChildReqQue.Create (mVxmlChildReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mVxmlChildReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VXML_CHILD_REQ_Q_RCV",
               mVxmlChildReqQueKey););
      return false;
   }

   // Creating VXML Rsp Queue
   lQueErr = mVxmlRspQue.Create (mVxmlRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mVxmlRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CALL_ALERT_Q_RCV",
               mVxmlRspQueKey););
      return false;
   }

   // Creating Call Alert Queue
   lQueErr = mCallAlertQue.Create (mCallAlertQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mCallAlertQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CALL_ALERT_Q_RCV",
               mCallAlertQueKey););
      return false;
   }

   // Creating EVENT Child Queue
   lQueErr = mEventChildQue.Create (mEventChildQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mEventChildQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_EVENT_CHILD_Q_RCV",
               mEventChildQueKey););
      return false;
   }

   // Creating EVENT Child Queue
   lQueErr = mCcxmlEventQue.Create (mCcxmlEventQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mCcxmlEventQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CCXML_EVENT_Q_RCV",
               mCcxmlEventQueKey););
      return false;
   }

   // Creating ISUPEncoder Queue
   lQueErr = mIsupEncQue.Create (mIsupEncQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mIsupEncQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_ISUP_ENC_Q_RCV",
               mIsupEncQueKey););
      return false;
   }

   // Creating External VXML Rsp Queue
   lQueErr = mExtVxmlRspQue.Create (mExtVxmlRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mExtVxmlRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_EXT_VXML_RSP_Q_RCV",
               mExtVxmlRspQueKey););
      return false;
   }

   // Creating Channel Status Queue
   lQueErr = mChannelStatusQue.Create (mChannelStatusQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mChannelStatusQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CHANNEL_STATUS_Q_RCV",
               mChannelStatusQueKey););
      return false;
   }

   // Creating Internal Peg Queue
   lQueErr = mIvrPegQue.Create (mIvrPegQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mIvrPegQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CHANNEL_STATUS_Q_RCV",
               mIvrPegQueKey););
      return false;
   }

   // Creating Scheduler Rsp Queue
   lQueErr = mScheduleRspQue.Create (mScheduleRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mScheduleRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_SCHEDULE_RSP_Q_RCV",
               mScheduleRspQueKey););
      return false;
   }

   lQueErr = mCktCmdReqQue.Create (mCktCmdReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mCktCmdReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_ISUP_CKT_CMD_Q_RCV",
               mCktCmdReqQueKey););
      return false;
   }

   lQueErr = mCktCmdRspQue.Create (mCktCmdRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mCktCmdRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_ISUP_CKT_CMD_RSP_Q_RCV",
               mCktCmdRspQueKey););
      return false;
   }

   lQueErr = mBillingReqQue.Create (mBillingReqQKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Error creating %s queue with QueKey:%d",
            "MSG_BILLING_REQ_QUE", mBillingReqQKey);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS06, lLogText);
      return false;
   }

   lQueErr = mBillingRspQue.Create(mBillingRspQKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Error creating %s queue with QueKey:%d",
            "MSG_BILLING_RSP_QUE", mBillingRspQKey);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS06, lLogText);
      return false;
   }

   lQueErr = mMpReqQue.Create (mMpReqQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mMpReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VOICE_PROCESS_REQ_Q", mMpReqQueKey););
      return false;
   }

   lQueErr = mMpRspQue.Create (mMpRspQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mMpRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VOICE_PROCESS_RSP_Q", mMpRspQueKey););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromCcxmlReqQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromCcxmlReqQue (CcxmlRequest & lCcxmlReq, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lCcxmlReq);
   msgQueueError lQueErr;

   memset (&lCcxmlReq, 0, sizeof (lCcxmlReq));

   // Read Messages from CCXML REQ Queue
   lQueErr = mCcxmlReqQue.ReadMsg ((void *) &lCcxmlReq, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mCcxmlReqQue);
   }

   if (0 == lCcxmlReq.channelId && '\0' == lCcxmlReq.ccxmlUrlRequest.url[0])
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Url:%s VarCnt:%d"
         " Var1:%s Var2:%s Var3:%s Var4:%s Var5:%s Rcvd CallCntrl Req",lCcxmlReq.refNum,
         lCcxmlReq.channelId, 
         lCcxmlReq.ccxmlUrlRequest.url, 
         lCcxmlReq.ccxmlUrlRequest.varCount,
         lCcxmlReq.ccxmlUrlRequest.var1, lCcxmlReq.ccxmlUrlRequest.var2,
         lCcxmlReq.ccxmlUrlRequest.var3, lCcxmlReq.ccxmlUrlRequest.var4,
         lCcxmlReq.ccxmlUrlRequest.var5);

   gLog.GenerateLog (API_IVR01, 1, lLogText);
   T(gTrace, 
         printf("%s: --------------------------------------------------------------\n",
            gProcessName););
   //IvrUtil::GenerateCcxmlReqTrace(lCcxmlReq, lLogText, true);
   T (gTrace, printf ("%s: %s QKey:%d\n",
            gProcessName, lLogText, mCcxmlReqQueKey););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromCcxmlRspQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromCcxmlRspQue (CcxmlResponse & lCcxmlRsp, INT16 msgType)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   KernelMsg       lKernelMsg;
   msgQueueError   lQueErr;

   memset (&lKernelMsg, 0, sizeof (lKernelMsg));
   memset(&lCcxmlRsp, 0, sizeof(CcxmlResponse));
   INT16 lMsgLen = sizeof (lKernelMsg);

   // Read Messages from CCXML Response Queue
   //lQueErr = mCcxmlRspQue.ReadMsg ((void *) &lKernelMsg, lMsgLen, msgType, BLOCKING);
   lQueErr = mCcxmlRspQue.ReadMsg ((void *) &lKernelMsg, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mCcxmlRspQue);
   }

   lCcxmlRsp = lKernelMsg.ccxmlResponse;
   if ('\0' == lCcxmlRsp.cmdName[0])
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CmdName:%s ConId:%s SessionId:%s DlgSrc:%s Duplex:%s"
         "Type:%s DialogId:%s Immediate:%s Rcvd Callcntrl Rsp", lCcxmlRsp.refNum,lCcxmlRsp.cmdName, 
         lCcxmlRsp.conId, lCcxmlRsp.sessionId, 
         lCcxmlRsp.dlgStart.dlgSrc, lCcxmlRsp.dlgStart.duplex, 
         lCcxmlRsp.dlgStart.type, 
         lCcxmlRsp.dlgTerminate.dialogId, 
         lCcxmlRsp.dlgTerminate.immediate);

   gLog.GenerateLog (API_IVR02, 1, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d\n",
            gProcessName, lLogText, mCcxmlRspQueKey););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ClearVxmlReqQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ClearVxmlReqQue (INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   VxmlRequest lVxmlReq;
   msgQueueError lQueErr;

   memset (&lVxmlReq, 0, sizeof (lVxmlReq));

   do
   {
      INT16 lMsgLen = sizeof (lVxmlReq);
      // Read Messages from the VXML Request Queue
      lQueErr = mVxmlReqQue.ReadMsg ((void *) &lVxmlReq, lMsgLen, msgType, NON_BLOCKING);
      if (Q_SUCCESS != lQueErr)
      {
         return IvrUtil::HandleQueueReadError (lQueErr, mVxmlReqQue);
      }

      if ('\0' == lVxmlReq.vxmlUrl[0] && 0 == lVxmlReq.channelId)
      {
         //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
         //return false;
      }

      snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d VxmlUrl:%s", 
            lVxmlReq.refNum, lVxmlReq.channelId, lVxmlReq.vxmlUrl);

      T (gTrace, printf ("%s: RefNum:%08X CID:%06d %s QueKey:%d Message Cleanup from Vxml Request Queue\n",
               gProcessName, lLogText, mVxmlReqQueKey););
   }while(lQueErr == Q_SUCCESS);
   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromVxmlReqQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromVxmlReqQue (VxmlRequest & lVxmlReq,  INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lVxmlReq);
   msgQueueError lQueErr;

   memset (&lVxmlReq, 0, sizeof (lVxmlReq));

   // Read Messages from the VXML Request Queue
   lQueErr = mVxmlReqQue.ReadMsg ((void *) &lVxmlReq, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mVxmlReqQue);
   }

   if ('\0' == lVxmlReq.vxmlUrl[0] && 0 == lVxmlReq.channelId)
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d VxmlUrl:%s", 
         lVxmlReq.refNum, lVxmlReq.channelId, lVxmlReq.vxmlUrl);

   gLog.GenerateLog (API_IVR03, 2, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d Message read from Vxml Request Queue\n",
            gProcessName, lLogText, mVxmlReqQueKey););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromVxmlChildReqQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromVxmlChildReqQue (VxmlRequest & lVxmlReq,  INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lVxmlReq);
   msgQueueError lQueErr;

   memset (&lVxmlReq, 0, sizeof (lVxmlReq));

   // Read Messages from the VXML Request Queue
   lQueErr = mVxmlChildReqQue.ReadMsg ((void *) &lVxmlReq, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mVxmlChildReqQue);
   }

   if ('\0' == lVxmlReq.vxmlUrl[0] && 0 == lVxmlReq.channelId)
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d VxmlUrl:%s", 
         lVxmlReq.refNum, lVxmlReq.channelId, lVxmlReq.vxmlUrl);

   gLog.GenerateLog (API_IVR14, 2, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d Message read from Vxml Child Request Queue\n",
            gProcessName, lLogText, mVxmlChildReqQueKey););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromVxmlRspQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromVxmlRspQue (VxmlResponse & lVxmlRsp,  INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lVxmlRsp);
   msgQueueError lQueErr;

   memset (&lVxmlRsp, 0, sizeof (lVxmlRsp));

   // Read Messages from the Vxml Response Queue 
   lQueErr = mVxmlRspQue.ReadMsg ((void *) &lVxmlRsp, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mVxmlRspQue);
   }

   if (0 == lVxmlRsp.result)
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "Result:%d",lVxmlRsp.result);

   gLog.GenerateLog (API_IVR04, 1, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Message read from Vxml Response Queue\n",
            gProcessName, lLogText, mVxmlRspQueKey, msgType););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromCallAlertQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromCallAlertQue (KernelMsg & lKernelMsg,  INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lKernelMsg);
   msgQueueError lQueErr;

   memset (&lKernelMsg, 0, sizeof (lKernelMsg));

   // Read Messages from the Call Alert Queue 
   lQueErr = mCallAlertQue.ReadMsg ((void *) &lKernelMsg, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mCallAlertQue);
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "KernelMsg:%d", lKernelMsg.kernelMsgType);

   gLog.GenerateLog (API_IVR04, 1, lLogText);
   T (gTrace, printf ("%s: %s QKey:%d MsgType:%d Message read from Call Alert Queue\n",
            gProcessName, lLogText, mCallAlertQueKey, msgType););

   if (0 == lKernelMsg.kernelMsgType)
   {
      T (gTrace, printf ("%s: Health Check Message Received, Writing to health check Queue\n", 
               gProcessName););
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromEventChildQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromEventChildQue (KernelMsg & lKernelMsg,  INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lKernelMsg);
   msgQueueError lQueErr;

   memset (&lKernelMsg, 0, sizeof (lKernelMsg));

   // Read Messages from the Call Alert Queue 
   lQueErr = mEventChildQue.ReadMsg ((void *) &lKernelMsg, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mEventChildQue);
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X ReqType:%d MsgType:%d", lKernelMsg.refNum,lKernelMsg.kernelMsgType, msgType);
   gLog.GenerateLog (API_IVR04, 1, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d Msg read from Event Child Q\n",
            gProcessName, lLogText, mEventChildQueKey););

   if (0 == lKernelMsg.kernelMsgType)
   {
      T (gTrace, printf ("%s: Health Check Msg Received, Writing to health check Q\n", 
               gProcessName););
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   return true;
}
//----------------------------------------------------------------------
// METHOD      : ClearEventChildQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ClearEventChildQue (INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   KernelMsg  lKernelMsg;
   msgQueueError lQueErr;

   memset (&lKernelMsg, 0, sizeof (lKernelMsg));

   // Read Messages from the Call Alert Queue 
   do
   {
      INT16 lMsgLen = sizeof (lKernelMsg);
      lQueErr = mEventChildQue.ReadMsg ((void *) &lKernelMsg, lMsgLen, msgType, NON_BLOCKING);
      if (Q_SUCCESS != lQueErr)
      {
         return IvrUtil::HandleQueueReadError (lQueErr, mEventChildQue);
      }

      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Cleanup: ReqType:%d",lKernelMsg.kernelMsgType);

      gLog.GenerateLog (API_IVR04, 1, lLogText);
      T (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Msg read from Event Child Q\n",
               gProcessName, lLogText, mEventChildQueKey, msgType););

      if (0 == lKernelMsg.kernelMsgType)
      {
         T (gTrace, printf ("%s: Health Check Msg Received, Writing to health check Q\n", 
                  gProcessName););
         //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
         //return false;
      }
   }while (lQueErr == Q_SUCCESS);
   return true;
}
//----------------------------------------------------------------------
// METHOD      : WriteToCcxmlReqQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToCcxmlReqQue (CcxmlRequest & lCcxmlReq )
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lCcxmlReq);
   msgQueueError lQueErr;
   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Url:%s ",
         lCcxmlReq.refNum, 
         lCcxmlReq.channelId,
         lCcxmlReq.ccxmlUrlRequest.url); 

   lQueErr = mCcxmlReqQue.WriteMsg ((void *) &lCcxmlReq, lMsgLen, lCcxmlReq.channelId, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lCcxmlReq);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mCcxmlReqQue,
               &lCcxmlReq, lMsgLen, lCcxmlReq.channelId, BLOCKING))
      {
         gLog.GenerateLog (API_IVR05, lLogText);
         TERR (gTrace, printf ("%s: %s Write to CCXML Reaquest Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR06, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to CCXML Request Q\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToCcxmlRspQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
//BOOLEAN IvrQueApi::WriteToCcxmlRspQue (CcxmlResponse & lCcxmlRsp)
BOOLEAN IvrQueApi::WriteToCcxmlRspQue (CcxmlResponse & lCcxmlRsp, INT16 msgType)
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   KernelMsg	lKernelMsg;
   msgQueueError lQueErr;

   memset(&lKernelMsg, 0, sizeof(KernelMsg));
   memcpy(&lKernelMsg.ccxmlResponse, &lCcxmlRsp, sizeof(lCcxmlRsp));
   INT16 lMsgLen = sizeof (lKernelMsg);
   INT16 lMType = msgType;
   lKernelMsg.refNum = lCcxmlRsp.refNum;

   switch(lCcxmlRsp.ccxmlMsgType)
   {
      case CCXML_CREATECALL:
         lKernelMsg.kernelMsgType = DIAL_OUT;
         break;
      default:
         lKernelMsg.kernelMsgType = CCXML_RSP;
         break;
   }

   //strcpy(lCcxmlRsp.sessionId, lCcxmlRsp.conId);
   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CmdName:%s ConId:%s(%d) SessionId:%s DlgSrc:%s Duplex:%s"
         " Type:%s DlgId:%s Immediate:%s Sent Command Rsp", lCcxmlRsp.refNum, lCcxmlRsp.cmdName, 
         lCcxmlRsp.conId, lKernelMsg.ccxmlResponse.channelId, lCcxmlRsp.sessionId,
         lCcxmlRsp.dlgStart.dlgSrc, lCcxmlRsp.dlgStart.duplex, 
         lCcxmlRsp.dlgStart.type, lCcxmlRsp.dlgTerminate.dialogId, 
         lCcxmlRsp.dlgTerminate.immediate);

   lQueErr = mCcxmlRspQue.WriteMsg ((void *) &lKernelMsg, lMsgLen, lMType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lKernelMsg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mCcxmlRspQue,
               &lKernelMsg, lMsgLen, lMType, BLOCKING))
      {
         gLog.GenerateLog (API_IVR07, lLogText);
         TERR (gTrace, printf ("%s: %s Write to CCXML Response Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR08, 1, lLogText);
   T (gTrace, printf ("%s: %s\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToVxmlReqQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToVxmlReqQue (VxmlRequest & lVxmlReq)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lVxmlReq);
   msgQueueError lQueErr;

   if(EVT_VX_TERM != lVxmlReq.vxmlEvent)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d VxmlUrl:%s", 
            gReferenceNum,
            lVxmlReq.channelId,
            lVxmlReq.vxmlUrl);
   }
   else
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d", 
            gReferenceNum, 
            lVxmlReq.channelId);
   }

   lQueErr = mVxmlReqQue.WriteMsg ((void *) &lVxmlReq, lMsgLen, lVxmlReq.channelId, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lVxmlReq);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mVxmlReqQue,
               &lVxmlReq, lMsgLen, lVxmlReq.channelId, BLOCKING))
      {
         gLog.GenerateLog (API_IVR09, lLogText);
         TERR (gTrace, printf ("%s: %s Write to VXML Request Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR10, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to VXML Request Q\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToVxmlChildReqQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToVxmlChildReqQue (VxmlRequest & lVxmlReq)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lVxmlReq);
   msgQueueError lQueErr;

   if(EVT_VX_TERM != lVxmlReq.vxmlEvent)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d VxmlUrl:%s", 
            gReferenceNum,
            lVxmlReq.channelId,
            lVxmlReq.vxmlUrl);
   }
   else
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d", 
            gReferenceNum, 
            lVxmlReq.channelId);
   }

   lQueErr = mVxmlChildReqQue.WriteMsg ((void *) &lVxmlReq, lMsgLen, lVxmlReq.channelId, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lVxmlReq);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mVxmlChildReqQue,
               &lVxmlReq, lMsgLen, lVxmlReq.channelId, BLOCKING))
      {
         gLog.GenerateLog (API_IVR15, lLogText);
         TERR (gTrace, printf ("%s: %s Write to VXML Child Request Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR16, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to VXML Child Request Q\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToVxmlRspQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToVxmlRspQue (VxmlResponse & lVxmlRsp, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   KernelMsg	lKernelMsg;
   memset(&lKernelMsg, 0, sizeof(lKernelMsg));
   memcpy(&lKernelMsg.vxmlResponse, &lVxmlRsp, sizeof(lVxmlRsp));
   lKernelMsg.kernelMsgType = VXML_RSP;
   lKernelMsg.refNum = lVxmlRsp.refNum;

   INT16 lMsgLen = sizeof (lKernelMsg);
   msgQueueError lQueErr;

   //lKernelMsg.refNum = gReferenceNumg;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Result:%d", gReferenceNum, lVxmlRsp.channelId, lVxmlRsp.result);

   lQueErr = mVxmlRspQue.WriteMsg ((void *) &lKernelMsg, lMsgLen, msgType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lKernelMsg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mVxmlRspQue,
               &lKernelMsg, lMsgLen, msgType, BLOCKING))
      {
         gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to VXML Response Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to VXML Response Q\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToEventChildQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToEventChildQue (VxmlResponse & lVxmlRsp, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   KernelMsg	lKernelMsg;
   memset(&lKernelMsg, 0, sizeof(lKernelMsg));
   memcpy(&lKernelMsg.vxmlResponse, &lVxmlRsp, sizeof(lVxmlRsp));
   lKernelMsg.kernelMsgType = VXML_RSP;
   lKernelMsg.refNum = lVxmlRsp.refNum;

   INT16 lMsgLen = sizeof (lKernelMsg);
   msgQueueError lQueErr;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Result:%d", gReferenceNum, lVxmlRsp.channelId, lVxmlRsp.result);

   lQueErr = mEventChildQue.WriteMsg ((void *) &lKernelMsg, lMsgLen, msgType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lKernelMsg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mEventChildQue,
               &lKernelMsg, lMsgLen, msgType, BLOCKING))
      {
         gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to Event Child Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Event Child Q\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToEventChildQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToEventChildQue (KernelMsg & lKernelMsg, UINT16 lMsgType)
{
   //TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lKernelMsg);
   msgQueueError lQueErr;


   //sprintf(lLogText, "MsgType:%d",lMsgType);

   lQueErr = mEventChildQue.WriteMsg ((void *) &lKernelMsg, lMsgLen, lMsgType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lKernelMsg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mEventChildQue,
               &lKernelMsg, lMsgLen, lMsgType, BLOCKING))
      {
         //gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: Write to Event Child Q Failed %s\n",
                  gProcessName, gLogText););
         return false;
      }
   }

   //gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Event Child Q\n",
            gProcessName, gLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromCcxmlEventQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromCcxmlEventQue (CcxmlRequest & lCcxmlReq, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   msgQueueError lQueErr;

   memset (&lCcxmlReq, 0, sizeof (lCcxmlReq));

   while(true)
   {
      INT16 lMsgLen = sizeof (lCcxmlReq);
      // Read Messages from the Call Alert Queue 
      lQueErr = mCcxmlEventQue.ReadMsg ((void *) &lCcxmlReq, lMsgLen, msgType, BLOCKING);
      if(Q_SUCCESS == lQueErr)
      {
         break;
      }
      if (Q_READ_SIGNAL_ERR != lQueErr)
      {
         return IvrUtil::HandleQueueReadError (lQueErr, mCcxmlEventQue);
      }
   }
   if ('\0' == lCcxmlReq.ccxmlEventRequest.event[0])
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Evt:%s SId:%d Rcvd an Evt",
         lCcxmlReq.refNum, lCcxmlReq.channelId,
         lCcxmlReq.ccxmlEventRequest.event, 
         lCcxmlReq.sessionId);

   gLog.GenerateLog (API_IVR13, 1, lLogText);
   T(gTrace, printf("%s: --------------------------------------------------------------\n", 
            gProcessName););
   T (gTrace, printf ("%s: %s\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToCcxmlEventQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToCcxmlEventQue (CcxmlRequest & lCcxmlReq)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lCcxmlReq);
   msgQueueError lQueErr;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Evt:%s SId:%d",
         lCcxmlReq.refNum, lCcxmlReq.channelId,
         lCcxmlReq.ccxmlEventRequest.event, lCcxmlReq.sessionId);

   lQueErr = mCcxmlEventQue.WriteMsg ((void *) &lCcxmlReq, lMsgLen, lCcxmlReq.channelId, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lCcxmlReq);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mCcxmlEventQue,
               &lCcxmlReq, lMsgLen, lCcxmlReq.channelId, BLOCKING))
      {
         //gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to Ccxml Event Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   //gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Ccxml Event Q\n",
            gProcessName, lLogText););

   return true;
}
//----------------------------------------------------------------------
// METHOD      : WriteToIsupEncQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToIsupEncQue (GenIsupMsg & lGenIsupMsg)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lGenIsupMsg);
   msgQueueError lQueErr;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "lGenIsupMsg Type:%d", lGenIsupMsg.isupMsgType);

   lQueErr = mIsupEncQue.WriteMsg ((void *) &lGenIsupMsg, lMsgLen, lGenIsupMsg.circuitId, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lGenIsupMsg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mIsupEncQue,
               &lGenIsupMsg, lMsgLen, lGenIsupMsg.circuitId, BLOCKING))
      {
         //gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to Isup Enc Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   //gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Isup Encoder Q\n",
            gProcessName, lLogText););

   return true;
}
//----------------------------------------------------------------------
// METHOD      : WriteToCallAlertQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToCallAlertQue (KernelMsg & lKernelMsg)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lKernelMsg);
   msgQueueError lQueErr;

   lQueErr = mCallAlertQue.WriteMsg ((void *) &lKernelMsg, lMsgLen, 1, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lKernelMsg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mCallAlertQue,
               &lKernelMsg, lMsgLen, 1, BLOCKING))
      {
         //gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to Event Child Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   //gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Call Alert Q\n",
            gProcessName, lLogText););

   return true;
}
//----------------------------------------------------------------------
// METHOD      : WriteToExtVxmlRspQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToExtVxmlRspQue (ExtResponse & lExtResponse, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   INT16 lMsgLen = sizeof (ExtResponse);
   msgQueueError lQueErr;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "%s Result:%d", gLogText, lExtResponse.result);

   lQueErr = mExtVxmlRspQue.WriteMsg ((void *) &lExtResponse, lMsgLen, msgType, NON_BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (ExtResponse);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mExtVxmlRspQue,
               &lExtResponse, lMsgLen, msgType, BLOCKING))
      {
         gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to External VXML Response Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to External VXML Response Q\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToChannelStatusQueue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToChannelStatusQueue(ShmIvrChannelDetails &channelDetail, LONG msgType)
{
   //TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   INT16 lMsgLen = sizeof (ShmIvrChannelDetails);
   msgQueueError lQueErr;

   // snprintf (lLogText, MAX_LOG_TEXT_LEN, "Result:%d", lVxmlRsp.result);

   //snprintf (lLogText, MAX_LOG_TEXT_LEN, "CID-%06d Status-%s", channelDetail.channelNum, 
   //		IvrUtil::ConvertChannelStatusToString(channelDetail.status));

   lQueErr = mChannelStatusQue.WriteMsg ((void *) &channelDetail, lMsgLen, msgType, NON_BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (ShmIvrChannelDetails);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mChannelStatusQue,
               &channelDetail, lMsgLen, msgType, NON_BLOCKING))
      {
         //gLog.GenerateLog (API_IVR17, 2, lLogText);
         //TERR (gTrace, printf ("%s: %s Write to Channel Status Queue Failed\n",
         //         gProcessName, lLogText););
         return false;
      }
   }

   //gLog.GenerateLog (API_IVR18, 2, lLogText);
   //T (gTrace, printf ("%s: %s Wrote to Channel Status Queue\n",
   //         gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromChannelStatusQueue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromChannelStatusQueue(ShmIvrChannelDetails &channelDetail, LONG msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (ShmIvrChannelDetails);
   msgQueueError lQueErr;

   memset (&channelDetail, 0, sizeof (ShmIvrChannelDetails));

   // Read Messages from the Channel Status Queue
   lQueErr = mChannelStatusQue.ReadMsg ((void *) &channelDetail, lMsgLen, msgType, NON_BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mChannelStatusQue);
   }

   if (0 == channelDetail.channelNum &&  IVR_CHANNEL_UNINITIALIZED == channelDetail.status && 0 == channelDetail.refNum)
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "CID-%06d Status-%s", channelDetail.channelNum, 
         IvrUtil::ConvertChannelStatusToString(channelDetail.status));

   gLog.GenerateLog (API_IVR19, 2, lLogText);
   T (gTrace, printf ("%s: %s Msge read from Channel Status Queue\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToEventChildQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToEventChildQue (CcxmlResponse & lCcxmlRsp, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   KernelMsg	lKernelMsg;
   memset(&lKernelMsg, 0, sizeof(lKernelMsg));
   memcpy(&lKernelMsg.ccxmlResponse, &lCcxmlRsp, sizeof(lCcxmlRsp));
   lKernelMsg.kernelMsgType = CCXML_RSP;
   lKernelMsg.refNum = lCcxmlRsp.refNum;


   INT16 lMsgLen = sizeof (lKernelMsg);
   msgQueueError lQueErr;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d CmdName:%s SessionId:%s DlgSrc:%s Duplex:%s"
         " Type:%s DlgId:%s Immediate:%s Sent Command Rsp", lCcxmlRsp.refNum, lCcxmlRsp.channelId, lCcxmlRsp.cmdName,
         lCcxmlRsp.sessionId,lCcxmlRsp.dlgStart.dlgSrc, lCcxmlRsp.dlgStart.duplex,
         lCcxmlRsp.dlgStart.type, lCcxmlRsp.dlgTerminate.dialogId,
         lCcxmlRsp.dlgTerminate.immediate);

   lQueErr = mEventChildQue.WriteMsg ((void *) &lKernelMsg, lMsgLen, msgType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (lKernelMsg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mEventChildQue,
               &lKernelMsg, lMsgLen, msgType, BLOCKING))
      {
         gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to Event Child Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Event Child Q\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToIvrPegQueue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToIvrPegQueue(INT16 EventCode, INT16 Action, UINT32 Count)
{
   //TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   if(!mPegRequired)
      return true;

   IvrPeg lIvrPeg;
   INT16  lMsgLen = sizeof (IvrPeg);
   msgQueueError lQueErr;

   lQueErr = mIvrPegQue.WriteMsg ((void *) &lIvrPeg, lMsgLen, 1, NON_BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (IvrPeg);
      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mIvrPegQue,
               &lIvrPeg, lMsgLen, 1, NON_BLOCKING))
      {
         //gLog.GenerateLog (API_IVR17, 2, lLogText);
         //TERR (gTrace, printf ("%s: %s Write to Channel Status Queue Failed\n",
         //         gProcessName, lLogText););
         return false;
      }
   }

   //gLog.GenerateLog (API_IVR18, 2, lLogText);
   //T (gTrace, printf ("%s: %s Wrote to Channel Status Queue\n",
   //         gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromIvrPegQueue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromIvrPegQueue(IvrPeg &ivrPeg, LONG msgType)
{
   //TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (IvrPeg);
   msgQueueError lQueErr;

   memset (&ivrPeg, 0, sizeof (IvrPeg));

   // Read Messages from the Ivr Peg Queue
   lQueErr = mIvrPegQue.ReadMsg ((void *) &ivrPeg, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mIvrPegQue);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromSchRspQ 
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromSchRspQ(ScheduleRsp& lScheduleRsp)
{
   //TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (ScheduleRsp);
   msgQueueError lQueErr;

   memset (&lScheduleRsp, 0, sizeof (ScheduleRsp));

   lQueErr = mScheduleRspQue.ReadMsg ((void *) &lScheduleRsp, lMsgLen, 0, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mScheduleRspQue);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToSchRspQ 
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToSchRspQ(ScheduleRsp& lScheduleRsp)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (ScheduleRsp);
   msgQueueError lQueErr;

   lQueErr = mScheduleRspQue.WriteMsg ((void *) &lScheduleRsp, lMsgLen, 1, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      //return IvrUtil::HandleQueueWriteError (lQueErr, mScheduleRspQue, 
      //       &lScheduleRsp, lMsgLen, 1, BLOCKING);

      if (false ==
            IvrUtil::HandleQueueWriteError (lQueErr, mScheduleRspQue,
               &lScheduleRsp, lMsgLen, 1, BLOCKING))
      {
         gLog.GenerateLog (API_IVR11, lLogText);
         TERR (gTrace, printf ("%s: %s Write to Schedule Response Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR12, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Schedule Response Q\n",
            gProcessName, lLogText););

   return true;
}
//----------------------------------------------------------------------
// METHOD      : WriteToCktCmd
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToCktCmd(CktCmdReq& lCktCmdMsg)
{
   //TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (CktCmdMsg);
   msgQueueError lQueErr;

   //   memset (&lCktCmdMsg, 0, sizeof (CktCmdReq));

   lQueErr = mCktCmdReqQue.WriteMsg ((void *) &lCktCmdMsg, lMsgLen, 1, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return  IvrUtil::HandleQueueWriteError (lQueErr, mCktCmdReqQue,
            &lCktCmdMsg, lMsgLen, 1, BLOCKING);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadCktCmdRsp 
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadCktCmdRsp(CktCmdRsp& lCktCmdRspMsg)
{
   //TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (CktCmdRspMsg);
   msgQueueError lQueErr;

   memset (&lCktCmdRspMsg, 0, sizeof (CktCmdRspMsg));

   lQueErr = mCktCmdRspQue.ReadMsg ((void *) &lCktCmdRspMsg, lMsgLen, 0, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mCktCmdRspQue);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromBillingRequestQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromBillingRequestQue (BillingRequest & billingRequest, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (BillingRequest);
   msgQueueError lQueErr;

   memset (&billingRequest, 0, sizeof (BillingRequest));

   lQueErr = mBillingReqQue.ReadMsg ((void *) &billingRequest, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mBillingReqQue);
   }

   if (0 == billingRequest.appId && '\0' == billingRequest.msisdn[0])
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum: %08X AppId:%d ORIG:%s DEST:%s EVT:%d", billingRequest.referenceNum,
         billingRequest.appId, billingRequest.msisdn, billingRequest.shortCode,
         billingRequest.event);

   gLog.GenerateLog (API_IVR20, 1, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Message read from BillingRequest Queue\n",
            gProcessName, lLogText, mBillingReqQue.GetQueueKey (), msgType););

   return true;
}


//----------------------------------------------------------------------
// METHOD      : WriteToBillingRequestQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToBillingRequestQue (BillingRequest & billingRequest)
{
   TEXT             lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16            lMsgLen = 0;
   INT16            lMsgType = 0;
   msgQueueError    lQueErr;

   lMsgLen = sizeof (BillingRequest);
   lMsgType = billingRequest.appId;

   lQueErr = mBillingReqQue.WriteMsg ((void *) &billingRequest, lMsgLen, lMsgType, BLOCKING);

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum: %08X AppId:%d ORIG:%s DEST:%s EVT:%d", billingRequest.referenceNum,
         billingRequest.appId, billingRequest.msisdn, billingRequest.shortCode,
         billingRequest.event);

   if (lQueErr != Q_SUCCESS)
   {
      if (false == IvrUtil::HandleQueueWriteError (lQueErr, mBillingReqQue,
               &billingRequest, lMsgLen, lMsgType, BLOCKING))
      {
         gLog.GenerateLog (API_IVR21, lLogText);
         TERR (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Write to BillingRequest Q Failed\n",
                  gProcessName, lLogText, mBillingReqQue.GetQueueKey (), lMsgType););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR22, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to BillingRequest Queue\n",
            gProcessName, lLogText););
   return true;
}


//----------------------------------------------------------------------
// METHOD      : ReadFromBillingResponseQue
// DESCRIPTION : common process to receiveresponse for charging gateway by IN
// PARAMETER   : BillingResponse &rBillingResponse
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromBillingResponseQue (BillingResponse & rBillingResponse, int msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (BillingResponse);
   msgQueueError lQueueErr;

   memset(&rBillingResponse, 0, sizeof(BillingResponse));

   lQueueErr = mBillingRspQue.ReadMsg (&rBillingResponse,lMsgLen, msgType, NON_BLOCKING);

   if (Q_SUCCESS != lQueueErr)
   {
      IvrUtil::HandleQueueReadError (lQueueErr, mBillingRspQue);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X Msisdn:%s Event:%d RspStatus:%d RelCause:%d AppId:%d",
         rBillingResponse.referenceNum, rBillingResponse.msisdn, rBillingResponse.event,
         rBillingResponse.status,
         rBillingResponse.relCause,
         rBillingResponse.appId);

   gLog.GenerateLog (API_IVR23, 1, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Message read from BillingResponse Queue\n",
            gProcessName, lLogText, mBillingRspQue.GetQueueKey (), msgType););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToBillingResponseQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToBillingResponseQue (BillingResponse & rBillingResponse)
{
   TEXT             lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16            lMsgLen = 0;
   INT16            lMsgType = 0;
   msgQueueError    lQueErr;

   lMsgLen = sizeof (BillingResponse);
   lMsgType = rBillingResponse.referenceNum;

   lQueErr = mBillingRspQue.WriteMsg ((void *) &rBillingResponse, lMsgLen, lMsgType, BLOCKING);

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X Msisdn:%s Event:%d RspStatus:%d RelCause:%d AppId:%d",
         rBillingResponse.referenceNum, rBillingResponse.msisdn, rBillingResponse.event,
         rBillingResponse.status,
         rBillingResponse.relCause,
         rBillingResponse.appId);
   if (lQueErr != Q_SUCCESS)
   {
      if (false == IvrUtil::HandleQueueWriteError (lQueErr, mBillingRspQue,
               &rBillingResponse, lMsgLen, lMsgType, BLOCKING))
      {
         gLog.GenerateLog (API_IVR24, lLogText);
         TERR (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Write to BillingResponse Q Failed\n",
                  gProcessName, lLogText, mBillingRspQue.GetQueueKey (), lMsgType););
         return false;
      }
   }

   gLog.GenerateLog (API_IVR25, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to BillingResponse Queue\n",
            gProcessName, lLogText););
   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromReqQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::ReadFromReqQue(MpRequestDetails & lMpReqDetails, INT16 lMsgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lMpReqDetails);
   msgQueueError lQueErr;

   memset (&lMpReqDetails, 0, sizeof(lMpReqDetails));

   lQueErr = mMpReqQue.ReadMsg ((void *) &lMpReqDetails, lMsgLen, lMsgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return IvrUtil::HandleQueueReadError (lQueErr, mMpReqQue);
   }

   sprintf(lLogText, "CmdType:%d \n", lMpReqDetails.cmdType);
   gLog.GenerateLog (API_IVR24, 1, lLogText);

   T (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Message Read from Req Queue\n",
            gProcessName, lLogText, mMpReqQue.GetQueueKey (), lMsgType););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToRspQue
// DESCRIPTION : writes to Rsp Q
// PARAMETER   :
// RETURN      : NONE
//----------------------------------------------------------------------
BOOLEAN IvrQueApi::WriteToRspQue (MpResponseDetails & lMpRspDetails, INT16 lMsgType)
{
   msgQueueError    lQueErr;
   INT16            lMsgLen = sizeof(lMpRspDetails);
   TEXT             lLogText[MAX_LOG_TEXT_LEN + 1] = "";
//   printf("TBR inside WriteToRspQue API\n");
   lQueErr = mMpRspQue.WriteMsg(&lMpRspDetails, lMsgLen, lMsgType, BLOCKING);
  // printf("lQueErr = %d\n", lQueErr);
   //printf("Q_SUCCESS = %d\n", Q_SUCCESS);
   if (lQueErr != Q_SUCCESS)
   {
      //ACULAB_MP_MGR = 1
      if (false == IvrUtil::HandleQueueWriteError (lQueErr, mMpRspQue,
               &lMpRspDetails, lMsgLen, lMsgType, BLOCKING))
      {
         TERR (gTrace,
               printf ("%s: %s MsgType:%d Write to Response Queue Failed\n",
                  gProcessName, lLogText, lMsgType););
         gLog.GenerateLog (API_IVR24, 1, lLogText);
         return false;
      }
   }
   sprintf(lLogText, "CmdType:%d Wrote to Response Queue\n", lMpRspDetails.cmdType);
   gLog.GenerateLog (API_IVR24, 1, lLogText);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););

   return true;
}



