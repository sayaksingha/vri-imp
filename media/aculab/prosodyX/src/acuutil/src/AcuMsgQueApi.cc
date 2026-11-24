static const char *id = "@(#) $Id: AcuMsgQueApi.cc,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
// AcuMsgQueApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2012
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Roopa A M                                    30-JUL-2012
//----------------------------------------------------------------------
#include "AcuMsgQueApi.h"

//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
AcuMsgQueApi::AcuMsgQueApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
AcuMsgQueApi::~AcuMsgQueApi ()
{

}
//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN AcuMsgQueApi::Init ()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == ReadConfig ())
   {
      strncpy (lLogText, "ReadConfig failed, Exiting ....", MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   if (false == CreateQueues ())
   {
      strncpy (lLogText, "CreateQueue failed, Exiting ....",
            MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   T (gTrace, printf ("%s:\33[32m AcuMsgQueApi::Initialization ok...\33[0m\n",
            gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN AcuMsgQueApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);

   // Read message queue key values
   if (lCfgRead.CfgInit (ACULAB_IPC_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, ACULAB_IPC_CFG););
      gLog.GenerateLog (ACU_API29, ACULAB_IPC_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_REQ_Q",
            mMpReqQueKey,
            ACULAB_MIN_MSG_Q_KEY, ACULAB_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: MSG_VOICE_PROCESS_REQ_Q = %d\n",
            gProcessName, mMpReqQueKey););

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_RSP_Q",
            mMpRspQueKey,
            ACULAB_MIN_MSG_Q_KEY, ACULAB_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: MSG_VOICE_PROCESS_RSP_Q = %d\n",
            gProcessName, mMpRspQueKey););

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_MODULE_REQ_Q",
            mMpModReqQueKey,
            ACULAB_MIN_MSG_Q_KEY, ACULAB_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: MSG_VOICE_PROCESS_MODULE_REQ_Q = %d\n",
            gProcessName, mMpModReqQueKey););

   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION : Reload configuration parameters
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
BOOLEAN AcuMsgQueApi::ReloadConfig ()
{
   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateQueues () 
// DESCRIPTION : Reload configuration parameters
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
BOOLEAN AcuMsgQueApi::CreateQueues()
{
   msgQueueError lQueErr;
   lQueErr = mMpReqQue.Create (mMpReqQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      AcuMediaUtil::HandleQueueCreateError (lQueErr, mMpReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n", 
               gProcessName, "MSG_VOICE_PROCESS_REQ_Q", mMpReqQueKey););
      return false;
   }

   lQueErr = mMpRspQue.Create (mMpRspQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      AcuMediaUtil::HandleQueueCreateError (lQueErr, mMpRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n", 
               gProcessName, "MSG_VOICE_PROCESS_RSP_Q", mMpRspQueKey););
      return false;
   }

   lQueErr = mMpModReqQue.Create (mMpModReqQueKey, IPC_CREAT | IPC_PERM);
   if (Q_SUCCESS != lQueErr)
   {
      AcuMediaUtil::HandleQueueCreateError (lQueErr, mMpModReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n", 
               gProcessName, "MSG_VOICE_PROCESS_MODULE_REQ_Q", mMpModReqQueKey););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromReqQue     
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN AcuMsgQueApi::ReadFromReqQue(MpRequestDetails & lMpReqDetails, INT16 lMsgType)
{
   UINT16 lCkt = 0;
   unsigned int lSeq = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lMpReqDetails);
   msgQueueError lQueErr;

   memset (&lMpReqDetails, 0, sizeof(lMpReqDetails));

   lQueErr = mMpReqQue.ReadMsg ((void *) &lMpReqDetails, lMsgLen, lMsgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return AcuMediaUtil::HandleQueueReadError (lQueErr, mMpReqQue);
   }

   sprintf(lLogText, "CmdType:%d[%s]\n", lMpReqDetails.cmdType, 
         AcuMediaUtil::ConvertCmdToStr(lMpReqDetails.cmdType, lSeq, lCkt, lMpReqDetails.mpReqData));
   gLog.GenerateLog (ACU_API30, 1, lLogText);

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
BOOLEAN AcuMsgQueApi::WriteToRspQue (MpResponseDetails & lMpRspDetails, INT16 lMsgType)
{
   UINT16 lCkt = 0;
   unsigned int lSeq = 0;
   msgQueueError    lQueErr;
   INT16            lMsgLen = sizeof(lMpRspDetails);
   TEXT             lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lQueErr = mMpRspQue.WriteMsg(&lMpRspDetails, lMsgLen, lMsgType, BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      //ACULAB_MP_MGR = 1
      if (false == AcuMediaUtil::HandleQueueWriteError (lQueErr, mMpRspQue,
               &lMpRspDetails, lMsgLen, lMsgType, BLOCKING))
      {
         TERR (gTrace,
               printf ("%s: %s MsgType:%d Write to Response Queue Failed\n",
                  gProcessName, lLogText, lMsgType););
         gLog.GenerateLog (ACU_API31, 1, lLogText);
         return false;
      }
   }

   sprintf(lLogText, "CmdType:%d[%s]\n", lMpRspDetails.cmdType, 
         AcuMediaUtil::ConvertCmdToStr(lMpRspDetails.cmdType, lSeq, lCkt, lMpRspDetails.mpRspData));
   gLog.GenerateLog (ACU_API32, 1, lLogText);

   T (gTrace, printf ("%s: %s Wrote to Response Queue\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadFromModuleReqQue     
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN AcuMsgQueApi::ReadFromModuleReqQue(MpRequestDetails & lMpReqDetails, INT16 lMsgType)
{
   UINT16 lCkt = 0;
   unsigned int lSeq = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (lMpReqDetails);
   msgQueueError lQueErr;

   memset (&lMpReqDetails, 0, sizeof(lMpReqDetails));

   lQueErr = mMpModReqQue.ReadMsg ((void *) &lMpReqDetails, lMsgLen, lMsgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return AcuMediaUtil::HandleQueueReadError (lQueErr, mMpModReqQue);
   }

   sprintf(lLogText, "CmdType:%d[%s] MsgType:%d\n", lMpReqDetails.cmdType, 
         AcuMediaUtil::ConvertCmdToStr(lMpReqDetails.cmdType, lSeq, lCkt, lMpReqDetails.mpReqData),
         lMsgType);
   gLog.GenerateLog (ACU_API30, 1, lLogText);

   T (gTrace, printf ("%s: %s QueKey:%d Message Read from Module Req Queue\n",
            gProcessName, lLogText, mMpModReqQue.GetQueueKey ()););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToModuleReqQue
// DESCRIPTION : writes to Req Q
// PARAMETER   :                     
// RETURN      : NONE
//----------------------------------------------------------------------
BOOLEAN AcuMsgQueApi::WriteToModuleReqQue (MpRequestDetails & lMpReqDetails, INT16 lMsgType)
{
   UINT16 lCkt = 0;
   unsigned int lSeq = 0;
   msgQueueError    lQueErr;
   INT16            lMsgLen = sizeof(lMpReqDetails);
   TEXT             lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lQueErr = mMpModReqQue.WriteMsg(&lMpReqDetails, lMsgLen, lMsgType, BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      //ACULAB_MP_MGR = 1
      if (false == AcuMediaUtil::HandleQueueWriteError (lQueErr, mMpModReqQue,
               &lMpReqDetails, lMsgLen, lMsgType, BLOCKING))
      {
         TERR (gTrace,
               printf ("%s: %s MsgType:%d Write to Module Request Queue Failed\n",
                  gProcessName, lLogText, lMsgType););
         //gLog.GenerateLog (ACU_API31, 1, lLogText);
         return false;
      }
   }

   sprintf(lLogText, "CmdType:%d[%s] MsgType:%d\n", lMpReqDetails.cmdType, 
         AcuMediaUtil::ConvertCmdToStr(lMpReqDetails.cmdType, lSeq, lCkt, lMpReqDetails.mpReqData),
         lMsgType);
   //gLog.GenerateLog (ACU_API32, 1, lLogText);

   T (gTrace, printf ("%s: %s Wrote to Module Request Queue\n",
            gProcessName, lLogText););

   return true;
}
