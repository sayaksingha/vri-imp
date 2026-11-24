static const char *id = "@(#) $Id: SipQueApi.cc,v 1.1.4.1.2.3 2017/10/27 17:06:31 mram Exp $";
//----------------------------------------------------------------------
// NAME
//       SipQueApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "SipQueApi.h"
extern SipCallDetails gSipCallDtls;
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
SipQueApi::SipQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
SipQueApi::~SipQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipQueApi::Init ()
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

   // Read Configuration
   if (false == this->ReadConfig ())
   {
      strncpy (lLogText, "ReadConfig failed, Exiting ....", TSS_MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
}

   // Creating Queues
   if (false == this->CreateQueues ())
   {
      strncpy (lLogText, "CreateQueue failed, Exiting ....",
            TSS_MAX_LOG_TEXT_LEN);
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
BOOLEAN SipQueApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);

   // Read message queue key values
   if (lCfgRead.CfgInit (SIP_IPC_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, SIP_IPC_CFG););
      gLog.GenerateLog (GSYS09, SIP_IPC_CFG);
      return false;
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_SIP_REQ_Q_RCV", mSipReqQueKey,
            SIP_MIN_MSG_Q_KEY, SIP_MAX_MSG_Q_KEY))
   {
      TERR (gTrace, printf ("%s: Config read failed for MSG_SIP_REQ_Q_RCV. File:%s\n",
               gProcessName, SIP_IPC_CFG););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_SIP_REQ_Q_RCV = %d\n",
               gProcessName, mSipReqQueKey););
   }
   
   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_SIP_RSP_Q_RCV", mSipRspQueKey,
            SIP_MIN_MSG_Q_KEY, SIP_MAX_MSG_Q_KEY))
   {
      TERR (gTrace, printf ("%s: Config read failed for MSG_SIP_RSP_Q_RCV. File:%s\n",
               gProcessName, SIP_IPC_CFG););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_SIP_RSP_Q_RCV = %d\n",
               gProcessName, mSipRspQueKey););
   }
   
   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_MEDIA_REQ_Q_RCV", mMediaReqQueKey,
            SIP_MIN_MSG_Q_KEY, SIP_MAX_MSG_Q_KEY))
   {
      TERR (gTrace, printf ("%s: Config read failed for MSG_MEDIA_REQ_Q_RCV. File:%s\n",
               gProcessName, SIP_IPC_CFG););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_MEDIA_REQ_Q_RCV = %d\n",
               gProcessName, mMediaReqQueKey););
   }
   
   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_MEDIA_RSP_Q_RCV", mMediaRspQueKey,
            SIP_MIN_MSG_Q_KEY, SIP_MAX_MSG_Q_KEY))
   {
      TERR (gTrace, printf ("%s: Config read failed for MSG_MEDIA_RSP_Q_RCV. File:%s\n",
               gProcessName, SIP_IPC_CFG););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_MEDIA_RSP_Q_RCV = %d\n",
               gProcessName, mMediaRspQueKey););
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
BOOLEAN SipQueApi::CreateQueues ()
{
   msgQueueError lQueErr;

   // Creating Sip Request Queue
   lQueErr = mSipReqQue.Create (mSipReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      SipUtil::HandleQueueCreateError (lQueErr, mSipReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_SIP_REQ_Q_RCV",
               mSipReqQueKey););
      return false;
   }
   
   // Creating Sip Response Queue
   lQueErr = mSipRspQue.Create (mSipRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      SipUtil::HandleQueueCreateError (lQueErr, mSipRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_SIP_RSP_Q_RCV",
               mSipRspQueKey););
      return false;
   }
   
   // Creating Media Request Queue
   lQueErr = mMediaReqQue.Create (mMediaReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      SipUtil::HandleQueueCreateError (lQueErr, mMediaReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_MEDIA_REQ_Q_RCV",
               mMediaReqQueKey););
      return false;
   }
   
   // Creating Sip Response Queue
   lQueErr = mMediaRspQue.Create (mMediaRspQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      SipUtil::HandleQueueCreateError (lQueErr, mMediaRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_MEDIA_RSP_Q_RCV",
               mMediaRspQueKey););
      return false;
   }

   return true;
}
//----------------------------------------------------------------------
// METHOD      : ReadFromSipReqQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipQueApi::ReadFromSipReqQue (GenIsupMsg & genIsupMsg, INT16 msgType, modeOption mode)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (genIsupMsg);
   msgQueueError lQueErr;

   memset (&genIsupMsg, 0, sizeof (genIsupMsg));
   
   // Read Messages from the Sip'1x' Queue
   lQueErr = mSipReqQue.ReadMsg ((void *) &genIsupMsg, lMsgLen, msgType, mode);
   if (Q_SUCCESS != lQueErr)
   {
      return SipUtil::HandleQueueReadError (lQueErr, mSipReqQue);
   }

   //if (0 == genIsupMsg.)
   //{
   //   gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
   //   return false;
   //}

   //snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
   //      "",
   //      genIsupMsg.);

   sprintf(lLogText, "RefNum:%08X CID:%06d Handle:%d VmpTx:%d VmpRx:%d Channel:%d Msg:%d "
         "QueKey:%d Message read from Sip Signalling Req Queue",
         gSipCallDtls.sipCallHdls[genIsupMsg.circuitId].seqNum, 
         gSipCallDtls.sipCallHdls[genIsupMsg.circuitId].index, 
         gSipCallDtls.sipCallHdls[genIsupMsg.circuitId].handle, 
         gSipCallDtls.sipCallHdls[genIsupMsg.circuitId].vmp_tx, 
         gSipCallDtls.sipCallHdls[genIsupMsg.circuitId].vmp_rx,
         gSipCallDtls.sipCallHdls[genIsupMsg.circuitId].channel,
         genIsupMsg.isupMsgType, mSipReqQue.GetQueueKey ());
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (SIP_QAPI01, 2, lLogText);
   
   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToSipRspQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipQueApi::WriteToSipRspQue (GenIsupMsg & genIsupMsg, TEXT *logText)
{
   INT16 lMsgLen = sizeof (genIsupMsg);
   msgQueueError lQueErr;

   lQueErr = mSipRspQue.WriteMsg ((void *) &genIsupMsg, lMsgLen, 1, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (genIsupMsg);
      if (false ==
            SipUtil::HandleQueueWriteError (lQueErr, mSipRspQue,
               &genIsupMsg, lMsgLen, 1, BLOCKING))
      {
         gLog.GenerateLog (SIP_QAPI03, logText);
         TERR (gTrace,
               printf ("%s: %s Write to Sip Signalling Rsp Queue Failed\n",
                  gProcessName, logText););
         return false;
      }
   }

   //snprintf (logText, TSS_MAX_LOG_TEXT_LEN,
   //      "",
   //      genIsupMsg.);

   gLog.GenerateLog (SIP_QAPI02, 2, logText);
   T (gTrace, printf ("%s: %s Wrote to Sip Rsp Queue %d \n", 
            gProcessName, logText, mSipRspQue.GetQueueKey()););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromMediaReqQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipQueApi::ReadFromMediaReqQue (MpRequestDetails & mpReqDetls, INT16 msgType, modeOption mode)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (mpReqDetls);
   msgQueueError lQueErr;

   memset (&mpReqDetls, 0, sizeof (mpReqDetls));
   
   // Read Messages from the Sip'1x' Queue
   lQueErr = mMediaReqQue.ReadMsg ((void *) &mpReqDetls, lMsgLen, msgType, mode);
   if (Q_SUCCESS != lQueErr)
   {
      return SipUtil::HandleQueueReadError (lQueErr, mMediaReqQue);
   }

   //if (0 == mpReqDetls.)
   //{
   //   gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
   //   return false;
   //}

   //snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
   //      "",
   //      mpReqDetls.);
   INT16 index, seqnum;
   switch(mpReqDetls.cmdType)
   {
      case MC_CNTTN:
         index = mpReqDetls.mpReqData.playMsg.circuitId;
         seqnum = mpReqDetls.mpReqData.playMsg.seqNum;
         break;
      case MC_GETDIG:
         index = mpReqDetls.mpReqData.getDigit.circuitId;
         seqnum = mpReqDetls.mpReqData.getDigit.seqNum;
         break;
      case MC_RECSTR:
         index = mpReqDetls.mpReqData.recordMsg.circuitId;
         seqnum = mpReqDetls.mpReqData.recordMsg.seqNum;
         break;
      case MC_DISTN:
      case MC_GETDIGSTP:
      case MC_RECSTP:
         index = mpReqDetls.mpReqData.stopMsg.circuitId;
         seqnum = mpReqDetls.mpReqData.stopMsg.seqNum;
         break;
   }
   
   sprintf(lLogText, "RefNum:%08X CID:%06d Handle:%d VmpTx:%d VmpRx:%d Channel:%d Cmd:%d"
         " QueKey:%d Message read from Sip Media Queue",
         gSipCallDtls.sipCallHdls[index].seqNum, 
         gSipCallDtls.sipCallHdls[index].index, 
         gSipCallDtls.sipCallHdls[index].handle, 
         gSipCallDtls.sipCallHdls[index].vmp_tx, 
         gSipCallDtls.sipCallHdls[index].vmp_rx,
         gSipCallDtls.sipCallHdls[index].channel,
         mpReqDetls.cmdType, mMediaReqQue.GetQueueKey ());

   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (SIP_QAPI01, 2, lLogText);
   
   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToMediaRspQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipQueApi::WriteToMediaRspQue (MpResponseDetails & mpRespDetls, INT16 msgType, TEXT *logText)
{
   INT16 lMsgLen = sizeof (mpRespDetls);
   msgQueueError lQueErr;

   lQueErr = mMediaRspQue.WriteMsg ((void *) &mpRespDetls, lMsgLen, msgType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (mpRespDetls);
      if (false ==
            SipUtil::HandleQueueWriteError (lQueErr, mMediaRspQue,
               &mpRespDetls, lMsgLen, msgType, BLOCKING))
      {
         gLog.GenerateLog (SIP_QAPI03, logText);
         TERR (gTrace,
               printf ("%s: %s Write to Sip Media Rsp Queue Failed\n",
                  gProcessName, logText););
         return false;
      }
   }

   //snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
   //      "",
   //      mpRespDetls.);

   gLog.GenerateLog (SIP_QAPI02, 2, logText);
   T (gTrace, printf ("%s: %s Wrote to Sip Media Rsp Queue %d \n", 
            gProcessName, logText, mMediaRspQue.GetQueueKey()););

   return true;
}
