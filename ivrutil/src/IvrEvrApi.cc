static const char *id = "@(#) $";
//----------------------------------------------------------------------
// NAME
// IvrEvrApi.cc
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
// Originated : Roopa A M                                    14-MAR-2012
//----------------------------------------------------------------------
#include "IvrEvrApi.h"

//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
IvrEvrApi::IvrEvrApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : 
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrEvrApi::Init ()
{
   if (true != this->ReadConfig ())
      return false;

   if (true != this->CreateMsgQueue ())
      return false;

   T (gTrace, printf ("%s:\33[32m IvrEvrApi::Initialization ok...\33[0m\n",
            gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrEvrApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (lCfgRead.CfgInit (IVR_IPC_CFG) != true)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s",
            IVR_IPC_CFG);
      gLog.GenerateLog (GSYS09, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_IVR_EVR_Q_RCV",
            mEvrHdlrQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: MSG_IVR_EVR_Q_RCV = %d\n",
            gProcessName, mEvrHdlrQueKey););

   if (CFG_OK != lCfgRead.GetConfigNum ("MSG_IVR_CDR_Q_RCV",
            mCdrHdlrQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: MSG_IVR_CDR_Q_RCV = %d\n",
            gProcessName, mCdrHdlrQueKey););

   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateMsgQueue
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrEvrApi::CreateMsgQueue ()
{
   msgQueueError lQueErr;

   lQueErr = mEvrHdlrQue.Create (mEvrHdlrQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mEvrHdlrQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_IVR_EVR_Q_RCV", mEvrHdlrQueKey););
      return false;
   }

   lQueErr = mCdrHdlrQue.Create (mCdrHdlrQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      IvrUtil::HandleQueueCreateError (lQueErr, mCdrHdlrQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_IVR_CDR_Q_RCV", mCdrHdlrQueKey););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : GenerateEvr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrEvrApi::GenerateEvr (IvrEvrMsg & rEvrMsg)
{
   INT16 lEvrMsgLen = sizeof (rEvrMsg);
   msgQueueError lQueErr = Q_SUCCESS;

   rEvrMsg.genTime = time(NULL);

   T (gTrace, IvrUtil::DisplayEvr (rEvrMsg););

   lQueErr = mEvrHdlrQue.WriteMsg (&rEvrMsg, lEvrMsgLen, 1, BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      lEvrMsgLen = sizeof (rEvrMsg);
      if (true != IvrUtil::HandleQueueWriteError (lQueErr,
               mEvrHdlrQue, &rEvrMsg, lEvrMsgLen, 1, BLOCKING))
      {
         //gLog.GenerateLog (IVR_API05);
         TERR (gTrace, printf ("%s: Failed to send evr message\n",
                  gProcessName););

         return false;
      }
   }

   //gLog.GenerateLog (IVR_API06, 2, logText);
   T (gTrace, printf ("%s: Evr wrote to evr Queue\n",
            gProcessName););

   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadEvrMessage
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrEvrApi::ReadEvrMessage (IvrEvrMsg & rEvrMsg)
{
   msgQueueError lQueErr;
   INT16 lEvrMsgLen = sizeof (rEvrMsg);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   
   lQueErr = mEvrHdlrQue.ReadMsg (&rEvrMsg, lEvrMsgLen, 0, NON_BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      if(Q_NO_MSG_WITH_TYPE == lQueErr)
         return false;
      TERR (gTrace, printf ("%s: ErrNo:%d ErrText:%s\n",
               gProcessName, errno, strerror (errno)););
      IvrUtil::HandleQueueReadError (lQueErr, mEvrHdlrQue);
      return false;
   }

   if (0 == rEvrMsg.evrCause && 0 == rEvrMsg.evrEvent && 0 == rEvrMsg.evrReason)
   {
    //  gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "EvrEvent:%d[%s] EvrCause:%d[%s] EvrReson:%d[%s] ReqMode:%d[%s] ",
         rEvrMsg.evrEvent, IvrUtil::ConvertEvrEventToStr (rEvrMsg.evrEvent),
         rEvrMsg.evrCause, IvrUtil::ConvertEvrCauseToStr (rEvrMsg.evrCause),
         rEvrMsg.evrReason, IvrUtil::ConvertEvrReasonToStr (rEvrMsg.evrReason),
         rEvrMsg.reqMode, IvrUtil::ConvertReqModeToStr (rEvrMsg.reqMode));

   //gLog.GenerateLog (IVR_API07, 1, lLogText);
   T (gTrace, printf ("%s: %s Evr Message read\n", gProcessName, lLogText););

   T (gTrace, IvrUtil::DisplayEvr (rEvrMsg););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : GenerateCdr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IvrEvrApi::GenerateCdr (IvrCdrMsg & rCdrMsg)
{
   INT16 lCdrMsgLen = sizeof (rCdrMsg);
   msgQueueError lQueErr = Q_SUCCESS;

   T (gTrace, IvrUtil::DisplayCdr (rCdrMsg););

   lQueErr = mCdrHdlrQue.WriteMsg (&rCdrMsg, lCdrMsgLen, 1, BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      lCdrMsgLen = sizeof (rCdrMsg);
      if (true != IvrUtil::HandleQueueWriteError (lQueErr,
               mCdrHdlrQue, &rCdrMsg, lCdrMsgLen, 1, BLOCKING))
      {
         //gLog.GenerateLog (IVR_API05);
         TERR (gTrace, printf ("%s: Failed to send evr message\n",
                  gProcessName););

         return false;
      }
   }

   //gLog.GenerateLog (IVR_API06, 2, logText);
   T (gTrace, printf ("%s: Cdr wrote to cdr Queue\n",
            gProcessName););

   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadCdrMessage
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrEvrApi::ReadCdrMessage (IvrCdrMsg & rCdrMsg)
{
   msgQueueError lQueErr;
   INT16 lCdrMsgLen = sizeof (rCdrMsg);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lQueErr = mCdrHdlrQue.ReadMsg (&rCdrMsg, lCdrMsgLen, 0, NON_BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      if(Q_NO_MSG_WITH_TYPE == lQueErr)
         return false;
      TERR (gTrace, printf ("%s: ErrNo:%d ErrText:%s\n",
               gProcessName, errno, strerror (errno)););
      IvrUtil::HandleQueueReadError (lQueErr, mCdrHdlrQue);
      return false;
   }

// if ('\0' == rCdrMsg.subsNum && 0 == rCdrMsg.subsType)
  if ((rCdrMsg.subsNum != nullptr && rCdrMsg.subsNum[0] == '\0') && rCdrMsg.subsType == 0)

   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "SubsNum:%s SubsType:%d CallRefNum:%s ",
          rCdrMsg.subsNum, rCdrMsg.subsType, rCdrMsg.callRefNum);

   //gLog.GenerateLog (IVR_API07, 1, lLogText);
   T (gTrace, printf ("%s: %s Cdr Message read\n", gProcessName, lLogText););

   T (gTrace, IvrUtil::DisplayCdr (rCdrMsg););

   return true;
}

