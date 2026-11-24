static const char *id = "@(#) $Id: SipEvrApi.cc,v 1.1.4.1 2016/04/12 21:59:25 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       SipEvrApi.cc
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
#include "SipEvrApi.h"
#include "SipUtil.h"
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
SipEvrApi::SipEvrApi ()
{
}

//----------------------------------------------------------------------
// METHOD      :
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipEvrApi::Init ()
{
   if (false == this->ReadConfig ())
   {
      return false;
   }

   if (false == this->CreateMsgQueue ())
   {
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
BOOLEAN SipEvrApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

   if (lCfgRead.CfgInit (SIP_IPC_CFG) != true)
   {
      snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN, "Could not open file %s",
            SIP_IPC_CFG);
      gLog.GenerateLog (GSYS09, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if (lCfgRead.GetConfigNum ("MSG_SIP_EVR_Q_RCV",
            mEvrHdlrQueKey,
            SIP_MIN_MSG_Q_KEY,
            SIP_MAX_MSG_Q_KEY) != CFG_OK)
   {
      snprintf (lLogText,TSS_MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_SIP_EVR_Q_RCV", SIP_IPC_CFG);
      gLog.GenerateLog (GSYS11, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_SIP_EVR_Q_RCV = %d\n",
               gProcessName, mEvrHdlrQueKey););
   }
   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateMsgQueue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipEvrApi::CreateMsgQueue ()
{
   msgQueueError lQueErr;
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

   lQueErr = mEvrHdlrQue.Create (mEvrHdlrQueKey, IPC_CREAT | IPC_PERM);
   if (lQueErr != Q_SUCCESS)
   {
      sprintf (lLogText,
            "Error creating MSG_SIP_EVR_Q_RCV queue with %d",
            mEvrHdlrQueKey);
      gLog.GenerateLog (GSYS06, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
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
BOOLEAN SipEvrApi::GenerateEvr (SipEvrMsg & rEvrMsg, TEXT * logText)
{
   INT16 lEvrMsgLen = sizeof (rEvrMsg);
   msgQueueError lQueErr = Q_SUCCESS;

   T (gTrace, SipUtil::DisplayEvr (rEvrMsg););

   lQueErr = mEvrHdlrQue.WriteMsg (&rEvrMsg, lEvrMsgLen, 1, BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      lEvrMsgLen = sizeof (rEvrMsg);
      if (true != SipUtil::HandleQueueWriteError (lQueErr,
               mEvrHdlrQue, &rEvrMsg, lEvrMsgLen, 1, BLOCKING))
      {
         gLog.GenerateLog (SIP_EAPI02, logText);
         TERR (gTrace, printf ("%s: %s: Failed to send evr message\n",
                  gProcessName, logText););

         return false;
      }
   }

   gLog.GenerateLog (SIP_EAPI01, 2, logText);
   T (gTrace, printf ("%s: %s: Evr wrote to evr Queue\n",
            gProcessName, logText););

   return true;
}
//----------------------------------------------------------------------
// METHOD      : GenerateProvEvr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SipEvrApi::GenerateProvEvr (EnumSipEvrEvt evrEvent,
      EnumSipEvrCause   evrCause,
      TEXT *             msisdn,
      SipProvEvrMsg &   provEvr)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   SipEvrMsg lEvrMsg;

   memset (&lEvrMsg, 0, sizeof (lEvrMsg));

   lEvrMsg.evrType = EVR_TYPE_SIP_PROV;
   lEvrMsg.evrEvent = evrEvent;
   lEvrMsg.evrCause = evrCause;

   strcpy (lEvrMsg.msisdn, msisdn);

   lEvrMsg.gentime = time (NULL);

   lEvrMsg.provEvr = provEvr;

   snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
         "Msisdn:%s EvrType:%s(%d) EvrEvent:%s(%d) EvrCause:%s(%d) ",
         lEvrMsg.msisdn,
         SipUtil::ConvertEvrTypeToStr (lEvrMsg.evrType), lEvrMsg.evrType,
         SipUtil::ConvertEvrEventToStr (lEvrMsg.evrEvent), lEvrMsg.evrEvent,
         SipUtil::ConvertEvrCauseToStr (lEvrMsg.evrCause), lEvrMsg.evrCause);

   return this->GenerateEvr (lEvrMsg, lLogText);
}
//-------------------------------------------------------------
// METHOD      : ReadEvrMessage
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN SipEvrApi::ReadEvrMessage (SipEvrMsg & evrMsg)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lEvrMsgLen = sizeof (SipEvrMsg);
   msgQueueError lQueErr;

   lQueErr = mEvrHdlrQue.ReadMsg (&evrMsg, lEvrMsgLen, 0, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      TERR (gTrace, printf ("%s: ErrNo:%d  ErrText:%s\n",
               gProcessName, errno, strerror(errno)););
      SipUtil::HandleQueueReadError (lQueErr, mEvrHdlrQue);
      return false;
   }

   if (0 == evrMsg.evrCause && 0 == evrMsg.evrEvent)
   {
      gHealthCheckObj.WriteMsgToHealthCheck(HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
         "Msisdn:%s EvrEvent:%s(%d) EvrCause:%s(%d)",
         evrMsg.msisdn,
         SipUtil::ConvertEvrEventToStr (evrMsg.evrEvent), evrMsg.evrEvent,
         SipUtil::ConvertEvrCauseToStr (evrMsg.evrCause), evrMsg.evrCause);


   T (gTrace, printf ("%s: %s Evr message read\n", gProcessName, lLogText););
   gLog.GenerateLog(SIP_EAPI03, 2, lLogText);

   T (gTrace, SipUtil::DisplayEvr (evrMsg););

   return true;
}

