static const char *id = "@(#) $Id: IsdnQueApi.cc,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $";
//----------------------------------------------------------------------
// NAME IsdnQueApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR : Chethan V M                      Date : 14/07/2013
//----------------------------------------------------------------------
#include "IsdnQueApi.h"
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnQueApi::IsdnQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnQueApi::~IsdnQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnQueApi::Init ()
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
BOOLEAN IsdnQueApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);

   // Read message queue key values
   if (lCfgRead.CfgInit (ISDN_IPC_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, ISDN_IPC_CFG););
      gLog.GenerateLog (GSYS09, ISDN_IPC_CFG);
      return false;
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_REQ_Q", mMediaCmdReqQueKey,
            ISDN_MIN_MSG_Q_KEY, ISDN_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_VOICE_PROCESS_REQ_Q = %d\n",
               gProcessName, mMediaCmdReqQueKey););
   }
   
   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_RSP_Q", mMediaCmdRspQueKey,
            ISDN_MIN_MSG_Q_KEY, ISDN_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_VOICE_PROCESS_RSP_Q = %d\n",
               gProcessName, mMediaCmdRspQueKey););
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_ISUP_HDLR_REQ_Q_RCV", mIsupReqQueKey,
            ISDN_MIN_MSG_Q_KEY, ISDN_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_ISUP_HDLR_REQ_Q_RCV = %d\n",
               gProcessName, mIsupReqQueKey););
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_ISUP_HDLR_RSP_Q_RCV", mIsupRspQueKey,
            ISDN_MIN_MSG_Q_KEY, ISDN_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_ISUP_HDLR_RSP_Q_RCV = %d\n",
               gProcessName, mIsupRspQueKey););
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_CHANNEL_MGMT_HDLR_REQ_Q_RCV", mChnlMgmtReqQueKey,
            ISDN_MIN_MSG_Q_KEY, ISDN_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_CHANNEL_MGMT_HDLR_REQ_Q_RCV = %d\n",
               gProcessName, mChnlMgmtReqQueKey););
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_CHANNEL_MGMT_HDLR_RSP_Q_RCV", mChnlMgmtRspQueKey,
            ISDN_MIN_MSG_Q_KEY, ISDN_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_CHANNEL_MGMT_HDLR_RSP_Q_RCV = %d\n",
               gProcessName, mChnlMgmtRspQueKey););
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
BOOLEAN IsdnQueApi::CreateQueues ()
{
   msgQueueError lQueErr;

   // Creating Cap Handler Queue
   lQueErr = mMediaCmdReqQue.Create (mMediaCmdReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IsdnUtil::HandleQueueCreateError (lQueErr, mMediaCmdReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VOICE_PROCESS_REQ_Q",
               mMediaCmdReqQueKey););
      return false;
   }

   lQueErr = mMediaCmdRspQue.Create (mMediaCmdRspQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      IsdnUtil::HandleQueueCreateError (lQueErr, mMediaCmdRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VOICE_PROCESS_RSP_Q",
               mMediaCmdRspQueKey););
      return false;
   }

   lQueErr = mIsupReqQue.Create (mIsupReqQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      IsdnUtil::HandleQueueCreateError (lQueErr, mIsupReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
            gProcessName, "MSG_ISUP_HDLR_REQ_Q_RCV",
            mIsupReqQueKey););
      return false;
   }

   lQueErr = mIsupRspQue.Create (mIsupRspQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      IsdnUtil::HandleQueueCreateError (lQueErr, mIsupRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
         gProcessName, "MSG_ISUP_HDLR_RSP_Q_RCV",
         mIsupRspQueKey););
      return false;
   }

   lQueErr = mChnlMgmtReqQue.Create (mChnlMgmtReqQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      IsdnUtil::HandleQueueCreateError (lQueErr, mChnlMgmtReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CHANNEL_MGMT_HDLR_REQ_Q_RCV",
               mChnlMgmtReqQueKey););
      return false;
   }

   lQueErr = mChnlMgmtRspQue.Create (mChnlMgmtRspQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      IsdnUtil::HandleQueueCreateError (lQueErr, mChnlMgmtRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_CHANNEL_MGMT_HDLR_RSP_Q_RCV",
               mChnlMgmtRspQueKey););
      return false;
   }



   return true;
}
//----------------------------------------------------------------------
// METHOD      : ReadFromMediaCmdReqQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnQueApi::ReadFromMediaCmdReqQue (MpRequestDetails & vpRequest, INT16 msgType)
{
   INT16         lMsgLen = sizeof (vpRequest);
   msgQueueError lQueErr;
   TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   memset (&vpRequest, 0, sizeof (vpRequest));

   lQueErr = mMediaCmdReqQue.ReadMsg ((void *) &vpRequest, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      if(false == IsdnUtil::HandleQueueReadError (lQueErr, mMediaCmdReqQue))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed Reading From MSG_VOICE_PROCESS_REQ_Q Key:%d Err:%d", 
            mMediaCmdReqQueKey, lQueErr);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_QAPI01, lLogText);

         return false;
      }
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC:%03d Media CmdReq Queue Read Suc's Cmd:%d %s",
         vpRequest.mpReqData.dummy.seqNum, vpRequest.mpReqData.dummy.circuitId,
         vpRequest.cmdType, IsdnUtil::ConvertCmdToStr(vpRequest.cmdType));
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_QAPI02, lLogText);
   
   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromChnlMgmtReqQue
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnQueApi::ReadFromChnlMgmtReqQue (CktCmdReq &cktCmdReq, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (CktCmdReq);
   msgQueueError lQueErr;

   memset (&cktCmdReq, 0, sizeof (CktCmdReq));

   lQueErr = mChnlMgmtReqQue.ReadMsg ((void *) &cktCmdReq, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      if(false == IsdnUtil::HandleQueueReadError (lQueErr, mChnlMgmtReqQue))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed Reading From MSG_CHANNEL_MGMT_HDLR_REQ_Q_RCV Key:%d Err:%d",
            mChnlMgmtReqQueKey, lQueErr);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_QAPI09, lLogText);
            return false;
      }
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CIC:%03d Chnt Mgmt Req Queue Read Suc's CmdType:%d", 
         cktCmdReq.channelId, cktCmdReq.cktCmd);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_QAPI10, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromIsupReqQue
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnQueApi::ReadFromIsupReqQue (GenIsupMsg &genIsupMsg, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (GenIsupMsg);
   msgQueueError lQueErr;

   memset (&genIsupMsg, 0, sizeof (GenIsupMsg));

   lQueErr = mIsupReqQue.ReadMsg ((void *) &genIsupMsg, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      if(false == IsdnUtil::HandleQueueReadError (lQueErr, mIsupReqQue))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed Reading From MSG_ISUP_HDLR_REQ_Q_RCV Key:%d Err:%d",
            mIsupReqQueKey, lQueErr);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_QAPI03, lLogText);
            return false;
      }
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CIC:%03d Isup Req Queue Read Suc's Type:%d %s", 
         genIsupMsg.circuitId, genIsupMsg.isupMsgType, 
         IsdnUtil::ConvertIsupMsgToStr(genIsupMsg.isupMsgType));
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_QAPI04, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToMediaCmdRspQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnQueApi::WriteToMediaCmdRspQue (MpResponseDetails & MpResponse)
{
   INT16 lMsgLen = sizeof (MpResponse);
   msgQueueError lQueErr;
   long          lMsgType = 0;
   unsigned int  lSeqNum = 0;
   TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lMsgType = this->SetChannelId(MpResponse.cmdType, lSeqNum, MpResponse.mpRspData);

   lQueErr = mMediaCmdRspQue.WriteMsg ((void *) &MpResponse, lMsgLen, lMsgType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (MpResponse);
      if (false ==
            IsdnUtil::HandleQueueWriteError (lQueErr, mMediaCmdRspQue,
               &MpResponse, lMsgLen, lMsgType, BLOCKING))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "Failed To Write MSG_VOICE_PROCESS_RSP_Q Cmd:%d Rsp:%d MediaErr:%d MsgType:%ld Key:%d QueErr:%d",
               MpResponse.cmdType, MpResponse.rspMsg, MpResponse.voiceProcessErr, 
               lMsgType, mMediaCmdRspQueKey, lQueErr);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_QAPI05, lLogText);
         return false;
      }
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CIC:%03ld Wrote to Media CmdRsp Queue Cmd:%d %s Rsp:%d MediaErr:%d MsgType:%ld",
         lSeqNum, lMsgType, MpResponse.cmdType, IsdnUtil::ConvertCmdToStr(MpResponse.cmdType),
         MpResponse.rspMsg, MpResponse.voiceProcessErr, lMsgType);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_QAPI06, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToIsupRspQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnQueApi::WriteToIsupRspQue (GenIsupMsg& genIsupMsg)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (genIsupMsg);
   msgQueueError lQueErr;

   lQueErr = mIsupRspQue.WriteMsg ((void *) &genIsupMsg, lMsgLen, 1, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (GenIsupMsg);
      if (false ==
            IsdnUtil::HandleQueueWriteError (lQueErr, mIsupRspQue,
               &genIsupMsg, lMsgLen, 1, BLOCKING))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "CIC:%03d Failed To Write MSG_ISUP_HDLR_RSP_Q_RCV Type:%d Key:%d QueErr:%d",
               genIsupMsg.circuitId, genIsupMsg.isupMsgType, mIsupRspQueKey, lQueErr);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_QAPI07, lLogText);
         return false;
      }
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CIC:%03d Wrote To Isup Rsp Queue Type:%d %s",
         genIsupMsg.circuitId, genIsupMsg.isupMsgType, 
         IsdnUtil::ConvertIsupMsgToStr(genIsupMsg.isupMsgType));
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_QAPI08, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToChnlMgmtRspQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnQueApi::WriteToChnlMgmtRspQue (CktCmdRsp &cktCmdRsp)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (CktCmdRsp);
   msgQueueError lQueErr;

   lQueErr = mChnlMgmtRspQue.WriteMsg ((void *) &cktCmdRsp, lMsgLen, 1, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (CktCmdRsp);
      if (false ==
            IsdnUtil::HandleQueueWriteError (lQueErr, mChnlMgmtRspQue,
               &cktCmdRsp, lMsgLen, 1, BLOCKING))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "CIC:%03d CmdStatus:%d Failed To Write MSG_CHANNEL_MGMT_HDLR_RSP_Q_RCV Key:%d QueErr:%d",
               cktCmdRsp.channelId, cktCmdRsp.status, mIsupRspQueKey, lQueErr);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_QAPI11, lLogText);
         return false;
      }
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CIC:%03d Wrote To Chnl Mgmt Rsp Queue CmdStatus:%d",
         cktCmdRsp.channelId, cktCmdRsp.status);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_QAPI12, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : SetChannelId
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
long IsdnQueApi::SetChannelId(EnumMediaCmdTypes lCmd, unsigned int &seqNum, MpData &lMpData)
{
   switch(lCmd)
   {
      case MC_CNTTN:
         seqNum = lMpData.playMsg.seqNum;
         return lMpData.playMsg.circuitId;
      case MC_DISTN:
         seqNum = lMpData.stopMsg.seqNum;
         return lMpData.stopMsg.circuitId;
      case MC_CNTSP:
         seqNum = lMpData.switchChnl.seqNum;
         return lMpData.switchChnl.circuitId_1;
      case MC_DISSP:
         seqNum = lMpData.switchChnl.seqNum;
         return lMpData.switchChnl.circuitId_1;
      case MC_GETDIG:
         seqNum = lMpData.getDigit.seqNum;
         return lMpData.getDigit.circuitId;
      case MC_GETDIGSTP:
         seqNum = lMpData.stopMsg.seqNum;
         return lMpData.stopMsg.circuitId;
      case MC_RECSTR:
         seqNum = lMpData.recordMsg.seqNum;
         return lMpData.recordMsg.circuitId;
      case MC_RECSTP:
         seqNum = lMpData.stopMsg.seqNum;
         return lMpData.stopMsg.circuitId;
      case MC_SETVOL:
         seqNum = lMpData.setVolSpd.seqNum;
         return lMpData.setVolSpd.circuitId;
      case MC_SETSPD:
         seqNum = lMpData.setVolSpd.seqNum;
         return lMpData.setVolSpd.circuitId;
      case MC_INVAL_CMD_TYPE:
      default:
         seqNum = lMpData.dummy.seqNum;
         return ISDN_INV_CHANNEL_NUMBER;
   }
}

