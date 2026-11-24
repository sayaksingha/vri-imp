static const char* id="@(#) $Id: MmpQueApi.cc,v 1.1.4.3 2016/04/12 21:59:45 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// MmpQueApi.cc
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
// Originated : Divya K                             DATE:28-FEB-2012
//----------------------------------------------------------------------
#include "MmpQueApi.h"

//extern INT32 gReferenceNum;
//extern TEXT  gLogText[];
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
MmpQueApi::MmpQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
MmpQueApi::~MmpQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::Init ()
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
BOOLEAN MmpQueApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Read message queue key values
   if (lCfgRead.CfgInit (MMP_IPC_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, MMP_IPC_CFG););
      gLog.GenerateLog (GSYS09,(TEXT*) MMP_IPC_CFG);
      return false;
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_MMP_PROV_HDLR_Q_RCV", mProvHdlrQueKey,
            MMP_MIN_MSG_Q_KEY, MMP_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_MMP_PROV_HDLR_Q_RCV = %d\n",
               gProcessName, mProvHdlrQueKey););
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_MMP_SUBS_PROV_REQ_QUEUE", mSubsProvReqQueKey,
            MIN_MSG_Q_KEY, MAX_MSG_Q_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_MMP_SUBS_PROV_REQ_QUEUE", MMP_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_MMP_SUBS_PROV_REQ_QUEUE = %d\n",
               gProcessName, mSubsProvReqQueKey););
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_MMP_SUBS_PROV_RESP_QUEUE", mSubsProvRspQueKey,
            MIN_MSG_Q_KEY, MAX_MSG_Q_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_MMP_SUBS_PROV_RESP_QUEUE", MMP_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_MMP_SUBS_PROV_RESP_QUEUE = %d\n",
               gProcessName, mSubsProvRspQueKey););
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_BILLING_RSP_QUE", mBillingRspQKey,
            IVR_MIN_MSG_Q_KEY, BILLING_MAX_MSG_Q_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_BILLING_RSP_QUE", MMP_IPC_CFG);
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
            "MSG_BILLING_REQ_QUE", MMP_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_BILLING_REQ_QUE = %d\n",
               gProcessName, mBillingReqQKey););
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
BOOLEAN MmpQueApi::CreateQueues ()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   msgQueueError lQueErr;

   lQueErr = mGwTxQue.Create (mGwTxQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      MmpUtil::HandleQueueCreateError (lQueErr, mGwTxQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_ASE_TX_MSG_QUEUE_RCV",
               mGwTxQueKey););
      return false;
   }

   lQueErr = mGwTxRspQue.Create (mGwTxRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      MmpUtil::HandleQueueCreateError (lQueErr, mGwTxRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_ASE_TX_RSP_MSG_QUEUE_RCV",
               mGwTxRspQueKey););
      return false;
   }

   lQueErr = mGwRxQue.Create (mGwRxQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      MmpUtil::HandleQueueCreateError (lQueErr, mGwRxQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_ASE_RX_MSG_QUEUE_RCV",
               mGwRxQueKey););
      return false;
   }

   lQueErr = mProvHdlrQue.Create (mProvHdlrQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      MmpUtil::HandleQueueCreateError (lQueErr, mProvHdlrQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_MMP_PROV_HDLR_Q_RCV",
               mProvHdlrQueKey););
      return false;
   }

   lQueErr = mSubsProvReqQue.Create (mSubsProvReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      MmpUtil::HandleQueueCreateError (lQueErr, mSubsProvReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_MMP_SUBS_PROV_REQ_QUEUE",
               mSubsProvReqQueKey););
      return false;
   }

   lQueErr = mSubsProvRspQue.Create (mSubsProvRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      MmpUtil::HandleQueueCreateError (lQueErr, mSubsProvRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_MMP_SUBS_PROV_RESP_QUEUE",
               mSubsProvRspQueKey););
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

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToGwTxQue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::WriteToGwTxQue (AseRequest& lAseRequest )
{
   TEXT             lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16            lMsgLen = sizeof (AseRequest);
   msgQueueError    lQueErr;

   /*   sprintf(lLogText, "SID:%s OA:%s DA:%s UDH:%d SmLen:%d", lAseRequest.sessionId,
        lAseRequest.data.msg.submit.origAddr.addrValue, 
        lAseRequest.data.msg.submit.destAddr[0].addrValue, 
        lAseRequest.data.msg.submit.udhFlag, 
        lAseRequest.data.msg.submit.smLength);*/
   lQueErr = mGwTxQue.WriteMsg ((void *) &lAseRequest, lMsgLen, lAseRequest.sessionId, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      if (false ==
            MmpUtil::HandleQueueWriteError (lQueErr, mGwTxQue,
               &lAseRequest, lMsgLen, lAseRequest.sessionId, BLOCKING))
      {
         gLog.GenerateLog (MMP_API16, lLogText);
         TERR (gTrace, printf ("%s: %s Write Req to G/W Tx Q Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (MMP_API01, lLogText);
     T (gTrace, printf ("%s: %s Wrote Request to G/W Tx Q\n",
     gProcessName, lLogText););

   return true;
}



//----------------------------------------------------------------------
// METHOD      : ReadFromCcxmlReqQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::ReadGwTxRsp (AseResponse& lAseResponse, INT16 msgType)
{
   INT16           lMsgLen = sizeof (lAseResponse);
   msgQueueError   lQueErr;

   memset (&lAseResponse, 0, sizeof (AseResponse));

   lQueErr = mGwTxRspQue.ReadMsg ((void *) &lAseResponse, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return MmpUtil::HandleQueueReadError (lQueErr, mGwTxRspQue);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromProvHdlrQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::ReadFromProvHdlrQue (GgiReqMsg & reqMsg, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (GgiReqMsg);
   msgQueueError lQueErr;

   memset (&reqMsg, 0, sizeof (GgiReqMsg));

   lQueErr = mProvHdlrQue.ReadMsg ((void *) &reqMsg, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return MmpUtil::HandleQueueReadError (lQueErr, mProvHdlrQue);
   }

   if (0 == reqMsg.interfaceId && '\0' == reqMsg.msisdn[0])
   {
      //367gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "GsmNum:%s OrigAddr:%s KeyWord:%s Interface:%d",
         reqMsg.gsmNum, reqMsg.msisdn, reqMsg.keyword, reqMsg.interfaceId);

   gLog.GenerateLog (MMP_API02, 1, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Message read from Prov Handler Queue\n",
            gProcessName, lLogText, mProvHdlrQue.GetQueueKey (), msgType););

   return true;
}


//----------------------------------------------------------------------
// METHOD      : WriteToSubsProvReqQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::WriteToSubsProvReqQue (MmpSubsProvReq& lMmpSubsProvReq)
{
   TEXT             lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16            lMsgLen = 0;
   INT16            lMsgType = 0;
   msgQueueError    lQueErr;

   lMsgLen = sizeof (MmpSubsProvReq);
   lMsgType = lMmpSubsProvReq.seqNo;

   lQueErr = mSubsProvReqQue.WriteMsg ((void *) &lMmpSubsProvReq, lMsgLen, 
         lMsgType, BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      if (false == MmpUtil::HandleQueueWriteError (lQueErr, mSubsProvReqQue,
               &lMmpSubsProvReq, lMsgLen, lMsgType, BLOCKING))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,"Write to CGW Request Q Failed, lQErr: %d", lQueErr);
         gLog.GenerateLog (MMP_API03, lLogText);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         return false;
      }
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadSubsProvRspQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::ReadSubsProvRspQue (MmpSubsProvRsp & lMmpSubsProvRsp, INT16 lMsgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = 0;
   msgQueueError lQueErr;

   memset (&lMmpSubsProvRsp, 0, sizeof (MmpSubsProvRsp));

   lMsgLen = sizeof (lMmpSubsProvRsp);

   lQueErr = mSubsProvRspQue.ReadMsg ((void *) &lMmpSubsProvRsp, lMsgLen, lMsgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      if (false == MmpUtil::HandleQueueReadError (lQueErr, mSubsProvRspQue))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,"Failed to Read CGW Respone Q,lErr: %d", lQueErr);
         gLog.GenerateLog (MMP_API04, lLogText);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         return false;
      }
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToProvHdlrQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::WriteToProvHdlrQue(GgiReqMsg & lProvReq)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lProvReq);
   msgQueueError lQueErr;

   lQueErr = mProvHdlrQue.WriteMsg ((void *) &lProvReq, lMsgLen, MMP_PROV_HDLR,
         BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      if (false ==
            MmpUtil::HandleQueueWriteError (lQueErr, mProvHdlrQue,
               &lProvReq, lMsgLen, MMP_PROV_HDLR, BLOCKING))
      {
         gLog.GenerateLog (MMP_API08, lLogText);
         TERR (gTrace,
               printf ("%s: %s Write to Provision Handler Queue Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog (MMP_API09, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to Provision Handler Queue\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromBillingRequestQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN MmpQueApi::ReadFromBillingRequestQue (BillingRequest & billingRequest, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (BillingRequest);
   msgQueueError lQueErr;

   memset (&billingRequest, 0, sizeof (BillingRequest));

   lQueErr = mBillingReqQue.ReadMsg ((void *) &billingRequest, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return MmpUtil::HandleQueueReadError (lQueErr, mBillingReqQue);
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

   gLog.GenerateLog (MMP_API10, 1, lLogText);
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
BOOLEAN MmpQueApi::WriteToBillingRequestQue (BillingRequest & billingRequest)
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
      if (false == MmpUtil::HandleQueueWriteError (lQueErr, mBillingReqQue,
               &billingRequest, lMsgLen, lMsgType, BLOCKING))
      {
         gLog.GenerateLog (MMP_API11, lLogText);
         TERR (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Write to BillingRequest Q Failed\n",
                  gProcessName, lLogText, mBillingReqQue.GetQueueKey (), lMsgType););
         return false;
      }
   }

   gLog.GenerateLog (MMP_API12, 2, lLogText);
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
BOOLEAN MmpQueApi::ReadFromBillingResponseQue (BillingResponse & rBillingResponse, int msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (BillingResponse);
   msgQueueError lQueueErr;

   memset(&rBillingResponse, 0, sizeof(BillingResponse));

   lQueueErr = mBillingRspQue.ReadMsg (&rBillingResponse,lMsgLen, msgType, NON_BLOCKING);

   if (Q_SUCCESS != lQueueErr)
   {
      MmpUtil::HandleQueueReadError (lQueueErr, mBillingRspQue);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X Msisdn:%s Event:%d RspStatus:%d RelCause:%d AppId:%d",
         rBillingResponse.referenceNum, rBillingResponse.msisdn, rBillingResponse.event,
         rBillingResponse.status,
         rBillingResponse.relCause, 
         rBillingResponse.appId);

   gLog.GenerateLog (MMP_API13, 1, lLogText);
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
BOOLEAN MmpQueApi::WriteToBillingResponseQue (BillingResponse & rBillingResponse)
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
      if (false == MmpUtil::HandleQueueWriteError (lQueErr, mBillingRspQue,
               &rBillingResponse, lMsgLen, lMsgType, BLOCKING))
      {
         gLog.GenerateLog (MMP_API14, lLogText);
         TERR (gTrace, printf ("%s: %s QueKey:%d MsgType:%d Write to BillingResponse Q Failed\n",
                  gProcessName, lLogText, mBillingRspQue.GetQueueKey (), lMsgType););
         return false;
      }
   }

   gLog.GenerateLog (MMP_API15, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to BillingResponse Queue\n",
            gProcessName, lLogText););
   return true;
}

